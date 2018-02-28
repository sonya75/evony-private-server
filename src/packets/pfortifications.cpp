/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pfortifications.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../PlayerCity.h"
#include "../Hero.h"

pfortifications::pfortifications(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pfortifications::~pfortifications()
{

}

void pfortifications::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    data2["packageId"] = 0.0;
    data2["ok"] = 1;

    VERIFYCASTLEID();
    CHECKCASTLEID();

    if ((command == "getProduceQueue"))
    {
        uint32_t castleid = data["castleId"];

        obj2["cmd"] = "fortifications.getProduceQueue";

        amf3array producequeue = amf3array();
        stTroopQueue * train = city->GetBarracksQueue(-2);
        amf3object producequeueobj = amf3object();
        amf3array producequeueinner = amf3array();
        amf3object producequeueinnerobj = amf3object();

        std::list<stTroopTrain>::iterator iter;

//         client->lists.lock();
        if (train->queue.size() > 0)
        {
            for (iter = train->queue.begin(); iter != train->queue.end(); ++iter)
            {
                producequeueinnerobj["num"] = iter->count;
                producequeueinnerobj["queueId"] = iter->queueid;
                producequeueinnerobj["endTime"] = (iter->endtime > 0) ? (iter->endtime - client->clientdelay) : (iter->endtime);// HACK: attempt to fix "lag" issues
                producequeueinnerobj["type"] = iter->troopid;
                producequeueinnerobj["costTime"] = iter->costtime / 1000;
                producequeueinner.Add(producequeueinnerobj);
            }
        }
        producequeueobj["allProduceQueue"] = producequeueinner;
        producequeueobj["positionId"] = -2;
        producequeue.Add(producequeueobj);

//         client->lists.unlock();

        data2["allProduceQueue"] = producequeue;

        gserver.SendObject(client, obj2);
        return;
    }
    if (command == "destructWallProtect")
    {
        uint32_t num = data["num"];
        uint32_t type = data["typeId"];
        stResources destruct;

        switch (type)
        {
        case TR_TRAP:
            if (city->forts.traps < num)
            {
                gserver.SendObject(client, gserver.CreateError("fortifications.destructWallProtect", -99, "You do not have that many traps."));
                return;
            }
            destruct = { gserver.troopconfig[TR_TRAP].gold, gserver.troopconfig[TR_TRAP].food, gserver.troopconfig[TR_TRAP].wood, gserver.troopconfig[TR_TRAP].stone, gserver.troopconfig[TR_TRAP].iron };
            city->resources += destruct;
            city->forts.traps -= num;
            break;
        case TR_ABATIS:
            if (city->forts.abatis < num)
            {
                gserver.SendObject(client, gserver.CreateError("fortifications.destructWallProtect", -99, "You do not have that many abatis."));
                return;
            }
            destruct = { gserver.troopconfig[TR_ABATIS].gold, gserver.troopconfig[TR_ABATIS].food, gserver.troopconfig[TR_ABATIS].wood, gserver.troopconfig[TR_ABATIS].stone, gserver.troopconfig[TR_ABATIS].iron };
            city->resources += destruct;
            city->forts.abatis -= num;
            break;
        case TR_ARCHERTOWER:
            if (city->forts.towers < num)
            {
                gserver.SendObject(client, gserver.CreateError("fortifications.destructWallProtect", -99, "You do not have that many archer towers."));
                return;
            }
            destruct = { gserver.troopconfig[TR_ARCHERTOWER].gold, gserver.troopconfig[TR_ARCHERTOWER].food, gserver.troopconfig[TR_ARCHERTOWER].wood, gserver.troopconfig[TR_ARCHERTOWER].stone, gserver.troopconfig[TR_ARCHERTOWER].iron };
            city->resources += destruct;
            city->forts.towers -= num;
            break;
        case TR_ROLLINGLOG:
            if (city->forts.logs < num)
            {
                gserver.SendObject(client, gserver.CreateError("fortifications.destructWallProtect", -99, "You do not have that many rolling logs."));
                return;
            }
            destruct = { gserver.troopconfig[TR_ROLLINGLOG].gold, gserver.troopconfig[TR_ROLLINGLOG].food, gserver.troopconfig[TR_ROLLINGLOG].wood, gserver.troopconfig[TR_ROLLINGLOG].stone, gserver.troopconfig[TR_ROLLINGLOG].iron };
            city->resources += destruct;
            city->forts.logs -= num;
            break;
        case TR_TREBUCHET:
            if (city->forts.trebs < num)
            {
                gserver.SendObject(client, gserver.CreateError("fortifications.destructWallProtect", -99, "You do not have that many trebuchets."));
                return;
            }
            destruct = { gserver.troopconfig[TR_TREBUCHET].gold, gserver.troopconfig[TR_TREBUCHET].food, gserver.troopconfig[TR_TREBUCHET].wood, gserver.troopconfig[TR_TREBUCHET].stone, gserver.troopconfig[TR_TREBUCHET].iron };
            city->resources += destruct;
            city->forts.trebs -= num;
            break;
        default:
            gserver.SendObject(client, gserver.CreateError("fortifications.destructWallProtect", -99, "Invalid trap"));
            return;
            break;
        }

        city->FortUpdate();
        city->ResourceUpdate();

        obj2["cmd"] = "fortifications.destructWallProtect";

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getFortificationsProduceList"))
    {
        uint32_t castleid = data["castleId"];

        obj2["cmd"] = "fortifications.getFortificationsProduceList";
        amf3array fortlist = amf3array();


        for (int i = 0; i < 20; ++i)
        {
            if (gserver.troopconfig[i].inside != 2)
                continue;
            if (gserver.troopconfig[i].time > 0)
            {
                amf3object parent;
                amf3object conditionbean;

                double costtime = gserver.troopconfig[i].time;
                double mayorinf = 1;
                if (city->mayor)
                    mayorinf = pow(0.995, city->mayor->GetManagement());

                costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));

                conditionbean["time"] = floor(costtime);
                conditionbean["destructTime"] = 0;
                conditionbean["wood"] = gserver.troopconfig[i].wood;
                conditionbean["food"] = gserver.troopconfig[i].food;
                conditionbean["iron"] = gserver.troopconfig[i].iron;
                conditionbean["gold"] = gserver.troopconfig[i].gold;
                conditionbean["stone"] = gserver.troopconfig[i].stone;

                amf3array buildings = amf3array();
                amf3array items = amf3array();
                amf3array techs = amf3array();

                for (stPrereq & req : gserver.troopconfig[i].buildings)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        int temp = city->GetBuildingLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["typeId"] = req.id;
                        buildings.Add(ta);
                    }
                }
                for (stPrereq & req : gserver.troopconfig[i].items)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        int temp = client->itemlist[req.id].count;
                        ta["curNum"] = temp;
                        ta["num"] = req.level;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["id"] = gserver.items[req.id].name;
                        items.Add(ta);
                    }
                }
                for (stPrereq & req : gserver.troopconfig[i].techs)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        int temp = client->GetResearchLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["id"] = req.id;
                        techs.Add(ta);
                    }
                }

                conditionbean["buildings"] = buildings;
                conditionbean["items"] = items;
                conditionbean["techs"] = techs;
                conditionbean["population"] = 0;
                parent["conditionBean"] = conditionbean;
                parent["permition"] = false;
                parent["typeId"] = i;
                fortlist.Add(parent);
            }
        }


        data2["fortList"] = fortlist;

        gserver.SendObject(client, obj2);
        return;
    }
    if (command == "accTroopProduce")
    {
        std::string speeditemid;
        uint32_t castleid = data["castleId"];
        uint32_t queueid = data["queueId"];
        std::string itemid = data["itemId"];
        int itemcount = client->GetItemCount((std::string)speeditemid);

        int reducetime = 0;
        int cents = 35;

        if (itemid != "player.fort.1.c")
        {
            gserver.SendObject(client, gserver.CreateError("fortifications.accTroopProduce", -99, "Invalid speedup."));
            return;
        }
        
        obj2["cmd"] = "fortifications.accTroopProduce";

        gserver.SendObject(client, obj2);

        stTroopQueue * queue = city->GetBarracksQueue(-2);
        for (stTroopTrain & trainqueue : queue->queue)
        {
            if (trainqueue.queueid == queueid)
            {
                //matched id, reduce time
                if (itemcount <= 0)
                {
                    if (client->cents < cents)
                    {
                        // TODO find error value -- castle.speedUpBuildCommand
                        gserver.SendObject(client, gserver.CreateError("fortifications.accTroopProduce", -99, "Not enough cents."));// not enough item and not enough cents
                        return;
                    }
                    //not enough item, but can buy with cents
                    client->cents -= cents;
                    client->PlayerUpdate();

                    if (trainqueue.endtime > 0)
                    {
                        trainqueue.endtime -= (trainqueue.endtime - trainqueue.starttime) * 0.3;
                        trainqueue.costtime *= 0.7;
                    }
                    else
                        trainqueue.costtime *= 0.7;
                }
                else
                { //has item
                    client->AddItem((std::string)speeditemid, -1);

                    if (trainqueue.endtime > 0)
                    {
                        trainqueue.endtime -= (trainqueue.endtime - trainqueue.starttime) * 0.3;
                        trainqueue.costtime *= 0.7;
                    }
                    else
                        trainqueue.costtime *= 0.7;
                }


            }
        }
        
        client->SaveToDB();
        city->SaveToDB();

        return;
    }
    if ((command == "produceWallProtect"))
    {
        uint32_t castleid = data["castleId"];

        obj2["cmd"] = "fortifications.produceWallProtect";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        int trooptype = data["wallProtectType"];
        int num = data["num"];

        stResources res;
        res.food = gserver.troopconfig[trooptype].food * num;
        res.wood = gserver.troopconfig[trooptype].wood * num;
        res.stone = gserver.troopconfig[trooptype].stone * num;
        res.iron = gserver.troopconfig[trooptype].iron * num;
        res.gold = gserver.troopconfig[trooptype].gold * num;


        if ((res.food > city->resources.food)
            || (res.wood > city->resources.wood)
            || (res.stone > city->resources.stone)
            || (res.iron > city->resources.iron)
            || (res.gold > city->resources.gold))
        {
            gserver.SendObject(client, gserver.CreateError("fortifications.produceWallProtect", -1, "Not enough resources."));
            return;
        }

        city->resources -= res;
        city->ResourceUpdate();

        if (city->AddToBarracksQueue(-2, trooptype, num, false, false) == -1)
        {
            gserver.SendObject(client, gserver.CreateError("fortifications.produceWallProtect", -99, "Troops could not be trained."));
            return;
        }

        gserver.SendObject(client, obj2);

        client->SaveToDB();
        city->SaveToDB();

        return;
    }
    if ((command == "cancelFortificationProduce"))
    {
        uint32_t castleid = data["castleId"];
        int positionid = data["positionId"];
        int queueid = data["queueId"];


        obj2["cmd"] = "fortifications.cancelFortificationProduce";

//         client->lists.lock();
        stTroopQueue * tq = city->GetBarracksQueue(-2);
        std::list<stTroopTrain>::iterator iter;

        for (iter = tq->queue.begin(); iter != tq->queue.end();)
        {
            if (iter->queueid == queueid)
            {
                if (iter->endtime > 0)
                {
                    //in progress
                    //refund 1/3 resources and set next queue to run
                    stResources res;
                    res.food = (gserver.troopconfig[iter->troopid].food * iter->count) / 3;
                    res.wood = (gserver.troopconfig[iter->troopid].wood * iter->count) / 3;
                    res.stone = (gserver.troopconfig[iter->troopid].stone * iter->count) / 3;
                    res.iron = (gserver.troopconfig[iter->troopid].iron * iter->count) / 3;
                    res.gold = (gserver.troopconfig[iter->troopid].gold * iter->count) / 3;

                    city->resources += res;
                    city->ResourceUpdate();
                    tq->queue.erase(iter++);

                    if (iter != tq->queue.end())
                        iter->endtime = Utils::time() + iter->costtime;
                }
                else
                {
                    //not in progress
                    //refund all resources
                    stResources res;
                    res.food = gserver.troopconfig[iter->troopid].food * iter->count;
                    res.wood = gserver.troopconfig[iter->troopid].wood * iter->count;
                    res.stone = gserver.troopconfig[iter->troopid].stone * iter->count;
                    res.iron = gserver.troopconfig[iter->troopid].iron * iter->count;
                    res.gold = gserver.troopconfig[iter->troopid].gold * iter->count;

                    city->resources += res;
                    tq->queue.erase(iter);
                    city->ResourceUpdate();
                }

                gserver.SendObject(client, obj2);
//                 client->lists.unlock();

                client->SaveToDB();
                city->SaveToDB();

                return;
            }
            ++iter;
        }
//        client->lists.unlock();
    }
}
