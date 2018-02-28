/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "ptroop.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"
#include "../Hero.h"


ptroop::ptroop(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

ptroop::~ptroop()
{

}

void ptroop::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if ((command == "getProduceQueue"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];

        obj2["cmd"] = "troop.getProduceQueue";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

//        client->lists.lock();
        amf3array producequeue = amf3array();
        for (int i = 0; i < 33; ++i)
        {
            stBuilding * building = city->GetBuilding(i);
            if ((building) && (building->type == B_BARRACKS))
            {
                stTroopQueue * train = city->GetBarracksQueue(i);

                if (train == 0)
                {
                    gserver.log->error("Crash! PlayerCity::stTroopQueue * train = city->GetBarracksQueue({}); - {}", i, __LINE__);
//                    client->lists.unlock();
                    return;
                }

                amf3object producequeueobj = amf3object();
                amf3array producequeueinner = amf3array();

                std::list<stTroopTrain>::iterator iter;

                if (train->queue.size() > 0)
                {
                    for (iter = train->queue.begin(); iter != train->queue.end(); ++iter)
                    {
                        amf3object producequeueinnerobj = amf3object();
                        producequeueinnerobj["num"] = iter->count;
                        producequeueinnerobj["queueId"] = iter->queueid;
                        producequeueinnerobj["endTime"] = (iter->endtime > 0) ? (iter->endtime - client->clientdelay) : (iter->endtime);// HACK: attempt to fix "lag" issues
                        producequeueinnerobj["type"] = iter->troopid;
                        producequeueinnerobj["costTime"] = iter->costtime / 1000;
                        producequeueinner.Add(producequeueinnerobj);
                    }
                }
                producequeueobj["allProduceQueue"] = producequeueinner;
                producequeueobj["positionId"] = building->id;
                producequeue.Add(producequeueobj);
            }
        }

//        client->lists.unlock();

        data2["allProduceQueue"] = producequeue;

        gserver.SendObject(client, obj2);
        return;
    }
    if (command == "accTroopProduce")
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];
        uint32_t positionid = data["positionId"];
        uint32_t queueid = data["queueId"];
        std::string itemid = data["itemId"];

        int reducetime = 0;
        int cents = 30;

        if (itemid != "player.troop.1.b")
        {
            gserver.SendObject(client, gserver.CreateError("troop.accTroopProduce", -99, "Invalid speedup."));
            return;
        }

        obj2["cmd"] = "troop.accTroopProduce";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);

        stTroopQueue * queue = city->GetBarracksQueue(positionid);
        for (stTroopTrain & trainqueue : queue->queue)
        {
            if (trainqueue.queueid == queueid)
            {
                //matched id, reduce time
                if (client->GetItemCount(itemid) <= 0)
                {
                    if (client->cents < cents)
                    {
                        // TODO find error value -- castle.speedUpBuildCommand
                        gserver.SendObject(client, gserver.CreateError("troop.accTroopProduce", -99, "Not enough cents."));// not enough item and not enough cents
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
                    client->AddItem(itemid, -1);

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
    if ((command == "getTroopProduceList"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];

        obj2["cmd"] = "troop.getTroopProduceList";
        amf3array trooplist = amf3array();


        for (int i = 0; i < 20; ++i)
        {
            if (gserver.troopconfig[i].inside != 1)
                continue;
            if (gserver.troopconfig[i].time > 0)
            {
                amf3object parent;
                amf3object conditionbean;

                double costtime = gserver.troopconfig[i].time;
                double mayorinf = 1;
                if (city->mayor)
                    mayorinf = pow(0.995, city->mayor->GetPower());

                switch (i)
                {
                    case TR_CATAPULT:
                    case TR_RAM:
                    case TR_TRANSPORTER:
                    case TR_BALLISTA:
                        costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_METALCASTING)));
                        break;
                    default:
                        costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_MILITARYSCIENCE)));
                        break;
                }

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
                        int16_t temp = city->GetBuildingLevel(req.id);
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
                        int64_t temp = client->itemlist[req.id].count;
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
                        int64_t temp = client->GetResearchLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["id"] = req.id;
                        techs.Add(ta);
                    }
                }


                conditionbean["buildings"] = buildings;
                conditionbean["items"] = items;
                conditionbean["techs"] = techs;
                conditionbean["population"] = gserver.troopconfig[i].population;
                parent["conditionBean"] = conditionbean;
                parent["permition"] = false;
                parent["typeId"] = i;
                trooplist.Add(parent);
            }
        }


        data2["troopList"] = trooplist;
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);
        return;
    }
    if (command == "disbandTroop")
    {
        uint32_t num = data["num"];
        uint32_t type = data["typeId"];
        stResources destruct;

        switch (type)
        {
        case TR_WORKER:
            if (city->forts.traps < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many workers."));
                return;
            }
            destruct = { gserver.troopconfig[TR_WORKER].gold, gserver.troopconfig[TR_WORKER].food,
                gserver.troopconfig[TR_WORKER].wood, gserver.troopconfig[TR_WORKER].stone, gserver.troopconfig[TR_WORKER].iron };
            city->resources += destruct;
            city->troops.worker -= num;
            break;
        case TR_WARRIOR:
            if (city->troops.warrior < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many warriors."));
                return;
            }
            destruct = { gserver.troopconfig[TR_WARRIOR].gold, gserver.troopconfig[TR_WARRIOR].food,
                gserver.troopconfig[TR_WARRIOR].wood, gserver.troopconfig[TR_WARRIOR].stone, gserver.troopconfig[TR_WARRIOR].iron };
            city->resources += destruct;
            city->troops.warrior -= num;
            break;
        case TR_SCOUT:
            if (city->troops.scout < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many scouts."));
                return;
            }
            destruct = { gserver.troopconfig[TR_SCOUT].gold, gserver.troopconfig[TR_SCOUT].food,
                gserver.troopconfig[TR_SCOUT].wood, gserver.troopconfig[TR_SCOUT].stone, gserver.troopconfig[TR_SCOUT].iron };
            city->resources += destruct;
            city->troops.scout -= num;
            break;
        case TR_PIKE:
            if (city->troops.pike < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many pikemen."));
                return;
            }
            destruct = { gserver.troopconfig[TR_PIKE].gold, gserver.troopconfig[TR_PIKE].food,
                gserver.troopconfig[TR_PIKE].wood, gserver.troopconfig[TR_PIKE].stone, gserver.troopconfig[TR_PIKE].iron };
            city->resources += destruct;
            city->troops.pike -= num;
            break;
        case TR_SWORDS:
            if (city->troops.sword < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many swordsmen."));
                return;
            }
            destruct = { gserver.troopconfig[TR_SWORDS].gold, gserver.troopconfig[TR_SWORDS].food,
                gserver.troopconfig[TR_SWORDS].wood, gserver.troopconfig[TR_SWORDS].stone, gserver.troopconfig[TR_SWORDS].iron };
            city->resources += destruct;
            city->troops.sword -= num;
            break;
        case TR_ARCHER:
            if (city->troops.archer < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many archers."));
                return;
            }
            destruct = { gserver.troopconfig[TR_ARCHER].gold, gserver.troopconfig[TR_ARCHER].food,
                gserver.troopconfig[TR_ARCHER].wood, gserver.troopconfig[TR_ARCHER].stone, gserver.troopconfig[TR_ARCHER].iron };
            city->resources += destruct;
            city->troops.archer -= num;
            break;
        case TR_TRANSPORTER:
            if (city->troops.transporter < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many transporters."));
                return;
            }
            destruct = { gserver.troopconfig[TR_TRANSPORTER].gold, gserver.troopconfig[TR_TRANSPORTER].food,
                gserver.troopconfig[TR_TRANSPORTER].wood, gserver.troopconfig[TR_TRANSPORTER].stone, gserver.troopconfig[TR_TRANSPORTER].iron };
            city->resources += destruct;
            city->troops.transporter -= num;
            break;
        case TR_CAVALRY:
            if (city->troops.cavalry < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many cavalry."));
                return;
            }
            destruct = { gserver.troopconfig[TR_CAVALRY].gold, gserver.troopconfig[TR_CAVALRY].food,
                gserver.troopconfig[TR_CAVALRY].wood, gserver.troopconfig[TR_CAVALRY].stone, gserver.troopconfig[TR_CAVALRY].iron };
            city->resources += destruct;
            city->troops.cavalry -= num;
            break;
        case TR_CATAPHRACT:
            if (city->troops.cataphract < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many cataphracts."));
                return;
            }
            destruct = { gserver.troopconfig[TR_CATAPHRACT].gold, gserver.troopconfig[TR_CATAPHRACT].food,
                gserver.troopconfig[TR_CATAPHRACT].wood, gserver.troopconfig[TR_CATAPHRACT].stone, gserver.troopconfig[TR_CATAPHRACT].iron };
            city->resources += destruct;
            city->troops.cataphract -= num;
            break;
        case TR_BALLISTA:
            if (city->troops.ballista < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many ballistae."));
                return;
            }
            destruct = { gserver.troopconfig[TR_BALLISTA].gold, gserver.troopconfig[TR_BALLISTA].food,
                gserver.troopconfig[TR_BALLISTA].wood, gserver.troopconfig[TR_BALLISTA].stone, gserver.troopconfig[TR_BALLISTA].iron };
            city->resources += destruct;
            city->troops.ballista -= num;
            break;
        case TR_RAM:
            if (city->troops.ram < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many rams."));
                return;
            }
            destruct = { gserver.troopconfig[TR_RAM].gold, gserver.troopconfig[TR_RAM].food,
                gserver.troopconfig[TR_RAM].wood, gserver.troopconfig[TR_RAM].stone, gserver.troopconfig[TR_RAM].iron };
            city->resources += destruct;
            city->troops.ram -= num;
            break;
        case TR_CATAPULT:
            if (city->troops.catapult < num)
            {
                gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "You do not have that many catapults."));
                return;
            }
            destruct = { gserver.troopconfig[TR_CATAPULT].gold, gserver.troopconfig[TR_CATAPULT].food,
                gserver.troopconfig[TR_CATAPULT].wood, gserver.troopconfig[TR_CATAPULT].stone, gserver.troopconfig[TR_CATAPULT].iron };
            city->resources += destruct;
            city->troops.catapult -= num;
            break;
        default:
            gserver.SendObject(client, gserver.CreateError("troop.disbandTroop", -99, "Invalid troops"));
            return;
            break;
        }

        city->TroopUpdate();
        city->ResourceUpdate();

        obj2["cmd"] = "troop.disbandTroop";

        gserver.SendObject(client, obj2);
        return;
    }
    if (command == "produceTroop")
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];


        obj2["cmd"] = "troop.produceTroop";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        int trooptype = data["troopType"];
        bool isshare = data["isShare"];
        bool toidle = data["toIdle"];
        int positionid = data["positionId"];
        int num = data["num"];


        stResources res;
        res.food = gserver.troopconfig[trooptype].food * num;
        res.wood = gserver.troopconfig[trooptype].wood * num;
        res.stone = gserver.troopconfig[trooptype].stone * num;
        res.iron = gserver.troopconfig[trooptype].iron * num;
        res.gold = gserver.troopconfig[trooptype].gold * num;


        if (city->availablepopulation < num)
        {
            gserver.SendObject(client, gserver.CreateError("troop.produceTroop", -1, "Not enough population."));
            return;
        }
        if ((res.food > city->resources.food)
            || (res.wood > city->resources.wood)
            || (res.stone > city->resources.stone)
            || (res.iron > city->resources.iron)
            || (res.gold > city->resources.gold))
        {
            gserver.SendObject(client, gserver.CreateError("troop.produceTroop", -1, "Not enough resources."));
            return;
        }

        if (isshare || toidle)
        {
            gserver.SendObject(client, gserver.CreateError("troop.produceTroop", -99, "Not supported action."));
            return;
        }

        city->resources -= res;
        city->population -= num;
        city->ResourceUpdate();
        city->CastleUpdate();

        if (city->AddToBarracksQueue(positionid, trooptype, num, isshare, toidle) == -1)
        {
            gserver.SendObject(client, gserver.CreateError("troop.produceTroop", -99, "Troops could not be trained."));
            return;
        }

        gserver.SendObject(client, obj2);

        client->SaveToDB();
        city->SaveToDB();

        return;
    }
    if ((command == "cancelTroopProduce"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];
        int positionid = data["positionId"];
        int queueid = data["queueId"];


        obj2["cmd"] = "troop.cancelTroopProduce";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;


//        client->lists.lock();
        stTroopQueue * tq = city->GetBarracksQueue(positionid);
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

//                client->lists.unlock();

                client->SaveToDB();
                city->SaveToDB();

                return;
            }
            ++iter;
        }
//        client->lists.unlock();
    }
}
