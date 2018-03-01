/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "ptech.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"
#include "../Hero.h"


ptech::ptech(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

ptech::~ptech()
{

}

void ptech::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if ((command == "getResearchList"))
    {
        VERIFYCASTLEID();

        uint32_t castleid = data["castleId"];

        obj2["cmd"] = "tech.getResearchList";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        amf3array researchbeans = amf3array();

        for (int i = 0; i < 25; ++i)
        {
            if (gserver.m_researchconfig[i][0].time > 0)
            {
                int level = client->GetResearchLevel(i);


                amf3object parent;
                amf3object conditionbean;

                double costtime = gserver.m_researchconfig[i][level].time;
                double mayorinf = 1;
                if (city->m_mayor)
                    mayorinf = pow(0.995, city->m_mayor->GetStratagem());

                costtime = (costtime)* (mayorinf);

                conditionbean["time"] = floor(costtime);
                conditionbean["destructTime"] = 0;
                conditionbean["wood"] = gserver.m_researchconfig[i][level].wood;
                conditionbean["food"] = gserver.m_researchconfig[i][level].food;
                conditionbean["iron"] = gserver.m_researchconfig[i][level].iron;
                conditionbean["gold"] = gserver.m_researchconfig[i][level].gold;
                conditionbean["stone"] = gserver.m_researchconfig[i][level].stone;

                amf3array buildings = amf3array();
                amf3array items = amf3array();
                amf3array techs = amf3array();

                for (stPrereq & req : gserver.m_researchconfig[i][0].buildings)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        auto temp = city->GetBuildingLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["typeId"] = req.id;
                        buildings.Add(ta);
                    }
                }
                for (stPrereq & req : gserver.m_researchconfig[i][0].items)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        auto temp = client->itemlist[req.id].count;
                        ta["curNum"] = temp;
                        ta["num"] = req.level;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["id"] = gserver.m_items[req.id].name;
                        items.Add(ta);
                    }
                }
                for (stPrereq & req : gserver.m_researchconfig[i][0].techs)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        auto temp = client->GetResearchLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["id"] = req.id;
                        techs.Add(ta);
                    }
                }

                conditionbean["buildings"] = buildings;
                conditionbean["items"] = items;
                conditionbean["techs"] = techs;
                conditionbean["population"] = gserver.m_researchconfig[i][level].population;
                parent["startTime"] = (double)client->research[i].starttime;
                parent["castleId"] = (double)client->research[i].castleid;
                //TODO: verify if works with multiple academies
                if ((client->research[i].endtime != 0) && (client->research[i].endtime < timestamp))
                {
                    //if research was sped up and isn't processed through the timer thread yet, emulate it being completed
                    auto research = level + 1;
                    parent["level"] = research;
                    parent["upgradeing"] = false;
                    parent["endTime"] = 0;
                    auto blevel = city->GetBuildingLevel(B_ACADEMY);//academy
                    if (blevel != 0)
                    {
                        parent["avalevel"] = blevel <= research ? blevel : research;
                    }
                    parent["permition"] = true;
                }
                else
                {
                    parent["level"] = level;
                    parent["upgradeing"] = ((client->research[i].endtime != 0) && (client->research[i].endtime > timestamp));
                    parent["endTime"] = (client->research[i].endtime > 0) ? (client->research[i].endtime - client->clientdelay) : (client->research[i].endtime);// HACK: attempt to fix "lag" issues
                    parent["avalevel"] = city->GetEffectiveTechLevel(i);
                    parent["permition"] = !city->m_researching;
                }
                parent["conditionBean"] = conditionbean;
                parent["typeId"] = i;
                researchbeans.Add(parent);
            }
        }


        data2["acailableResearchBeans"] = researchbeans;
        data2["ok"] = 1;
        data2["packageId"] = 0.0;


        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "research"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];
        int techid = data["techId"];

        if (techid < 0 || techid > 25 || client->research[techid].level >= 10 || gserver.m_researchconfig[techid][client->research[techid].level].time == 0)
        {
            gserver.SendObject(client, gserver.CreateError("tech.research", -99, "Invalid technology."));
            return;
        }

        stResearch * research;
        stBuildingConfig * researchconfig;

        research = &client->research[techid];
        researchconfig = &gserver.m_researchconfig[techid][research->level];


        if (!city->m_researching)
        {
            if ((researchconfig->food > city->m_resources.food)
                || (researchconfig->wood > city->m_resources.wood)
                || (researchconfig->stone > city->m_resources.stone)
                || (researchconfig->iron > city->m_resources.iron)
                || (researchconfig->gold > city->m_resources.gold))
            {
                gserver.SendObject(client, gserver.CreateError("tech.research", -99, "Not enough resources."));
                return;
            }
            obj2["cmd"] = "tech.research";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;

            city->m_resources.food -= researchconfig->food;
            city->m_resources.wood -= researchconfig->wood;
            city->m_resources.stone -= researchconfig->stone;
            city->m_resources.iron -= researchconfig->iron;
            city->m_resources.gold -= researchconfig->gold;

            research->castleid = castleid;


            auto costtime = researchconfig->time;
            double mayorinf = 1.0f;
            if (city->m_mayor)
                mayorinf = pow(0.995, city->m_mayor->GetStratagem());

            costtime = (costtime)* (mayorinf);

            research->endtime = double(timestamp + floor(costtime) * 1000);

            research->starttime = (double)timestamp;

            auto * ra = new stResearchAction;

            stTimedEvent te;
            ra->city = city;
            ra->client = client;
            ra->researchid = techid;
            te.data = ra;
            te.type = DEF_TIMEDRESEARCH;
            city->m_researching = true;

            gserver.AddTimedEvent(te);

            amf3object parent;
            amf3object conditionbean;

            conditionbean["time"] = floor(costtime);
            conditionbean["destructTime"] = 0.0;
            conditionbean["wood"] = researchconfig->wood;
            conditionbean["food"] = researchconfig->food;
            conditionbean["iron"] = researchconfig->iron;
            conditionbean["gold"] = researchconfig->gold;
            conditionbean["stone"] = researchconfig->stone;

            auto buildings = amf3array();
            auto items = amf3array();
            auto techs = amf3array();


            for (auto & req : researchconfig->buildings)
            {
                if (req.id > 0)
                {
                    auto ta = amf3object();
                    ta["level"] = req.level;
                    auto temp = ((PlayerCity*)client->citylist.at(client->currentcityindex))->GetBuildingLevel(req.id);
                    ta["curLevel"] = temp;
                    ta["successFlag"] = temp >= req.level ? true : false;
                    ta["typeId"] = req.id;
                    buildings.Add(ta);
                }
            }
            for (auto & req : researchconfig->items)
            {
                if (req.id > 0)
                {
                    auto ta = amf3object();
                    auto temp = client->itemlist[req.id].count;
                    ta["curNum"] = temp;
                    ta["num"] = req.level;
                    ta["successFlag"] = temp >= req.level ? true : false;
                    ta["id"] = gserver.m_items[req.id].name;
                    items.Add(ta);
                }
            }
            for (auto & req : researchconfig->techs)
            {
                if (req.id > 0)
                {
                    auto ta = amf3object();
                    ta["level"] = req.level;
                    auto temp = client->GetResearchLevel(req.id);
                    ta["curLevel"] = temp;
                    ta["successFlag"] = temp >= req.level ? true : false;
                    ta["id"] = req.id;
                    techs.Add(ta);
                }
            }

            conditionbean["buildings"] = buildings;
            conditionbean["items"] = items;
            conditionbean["techs"] = techs;
            conditionbean["population"] = researchconfig->population;
            parent["startTime"] = (double)research->starttime;
            parent["castleId"] = (double)research->castleid;
            parent["level"] = client->GetResearchLevel(techid);
            parent["conditionBean"] = conditionbean;
            parent["avalevel"] = city->GetEffectiveTechLevel(techid);
            parent["upgradeing"] = (bool)(research->starttime != 0);
            parent["endTime"] = (research->endtime > 0) ? (research->endtime - client->clientdelay) : (research->endtime);// HACK: attempt to fix "lag" issues
            parent["typeId"] = techid;
            parent["permition"] = !city->m_researching;

            data2["tech"] = parent;

            gserver.SendObject(client, obj2);

            city->ResourceUpdate();

            client->SaveToDB();

            return;
        }
        else
        {
            gserver.SendObject(client, gserver.CreateError("tech.research", -99, "Research already in progress."));
            return;
        }
    }
    if ((command == "cancelResearch"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];
        auto * city = client->GetCity(castleid);
        uint16_t techid = 0;

        for (auto i = 0; i < 25; ++i)
        {
            if (client->research[i].castleid == castleid)
            {
                techid = i;
                break;
            }
        }

        if (!city || !city->m_researching || techid == 0)
        {
            gserver.SendObject(client, gserver.CreateError("tech.cancelResearch", -99, "Invalid city."));
            return;
        }

        stResearch * research;
        stBuildingConfig * researchconfig;


        research = &client->research[techid];
        researchconfig = &gserver.m_researchconfig[techid][research->level];


        if (city->m_researching)
        {
            obj2["cmd"] = "tech.cancelResearch";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;

            city->m_resources.food += double(researchconfig->food) / 3;
            city->m_resources.wood += double(researchconfig->wood) / 3;
            city->m_resources.stone += double(researchconfig->stone) / 3;
            city->m_resources.iron += double(researchconfig->iron) / 3;
            city->m_resources.gold += double(researchconfig->gold) / 3;

            std::list<stTimedEvent>::iterator iter;

            for (iter = gserver.researchlist.begin(); iter != gserver.researchlist.end();)
            {
                auto * ra = (stResearchAction *)iter->data;
                auto * city = ra->city;
                if (city->m_castleid == castleid)
                {
                    ra->researchid = 0;
                    break;
                }
                ++iter;
            }
            research->castleid = 0;
            research->endtime = 0.0;
            research->starttime = 0.0;

            city->m_researching = false;

            gserver.SendObject(client, obj2);

            city->ResourceUpdate();

            client->SaveToDB();

            return;
        }
    }
    if ((command == "speedUpResearch"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        std::string speeditemid;
        uint32_t castleid = data["castleId"];

        speeditemid = static_cast<std::string>(data["itemId"]);

        stResearch * research = nullptr;
        for (auto i = 0; i < 25; ++i)
        {
            if (client->research[i].castleid == city->m_castleid)
            {
                research = &client->research[i];
                break;
            }
        }
        if (research == 0)
        {
            //TODO: get error message for city not having research -- tech.speedUpResearch
            gserver.SendObject(client, gserver.CreateError("tech.speedUpResearch", -99, "Invalid tech."));
            return;
        }

        if (speeditemid == "free.speed")
        {
            //check if under 5 mins

            if ((research->endtime - research->starttime) <= 5 * 60 * 1000)
            {
                //under 5 mins
                obj2["cmd"] = "tech.speedUpResearch";
                data2["packageId"] = 0.0;
                data2["ok"] = 1;

                gserver.SendObject(client, obj2);

                research->endtime -= 5 * 60 * 1000;

                client->SaveToDB();
            }
            else
            {
                //over 5 mins
                //TODO: get 5 min speed up error - tech.speedUpResearch
                gserver.SendObject(client, gserver.CreateError("tech.speedUpResearch", -99, "Invalid speed up."));
                return;
            }
        }
        else
        {
            //is not under 5 mins, apply an item
            uint32_t cents = 0;
            double reducetime = 0;

            obj2["cmd"] = "tech.speedUpResearch";
            data2["packageId"] = 0.0;
            data2["ok"] = 1;

            gserver.SendObject(client, obj2);

            //TODO: reduce time on building being sped up -- tech.speedUpResearch

            if (speeditemid == "consume.2.a")
            {
                cents = 5;
                reducetime = 15 * 60 * 1000;
            }
            else if (speeditemid == "consume.2.b")
            {
                cents = 10;
                reducetime = 60 * 60 * 1000;
            }
            else if (speeditemid == "consume.2.b.1")
            {
                cents = 20;
                reducetime = 150 * 60 * 1000;
            }
            else if (speeditemid == "consume.2.c")
            {
                cents = 50;
                reducetime = 8 * 60 * 60 * 1000;
            }
            else if (speeditemid == "consume.2.c.1")
            {
                cents = 80;
                reducetime = ((rand() % 21) + 10) * 60 * 60 * 1000;
            }
            else if (speeditemid == "consume.2.d")
            {
                cents = 120;
                reducetime = ((research->endtime - research->starttime)*0.30);
            }
            else if (speeditemid == "player.speedup.1")
            {
                cents = 175;
                reducetime = ((research->endtime - research->starttime)*0.50);
            }
            else if (speeditemid == "coins.speed")
            {
                reducetime = (research->endtime - research->starttime);
                cents=calculatecoinsforspeedup(reducetime);
            }

            if (client->GetItemCount(speeditemid) <= 0)
            {
                if (client->cents < cents)
                {
                    // not enough item and not enough cents
                    //TODO: find error value -- tech.speedUpResearch
                    gserver.SendObject(client, gserver.CreateError("tech.speedUpResearch", -99, "Not enough cents."));
                    return;
                }
                //not enough item, but can buy with cents
                client->cents -= cents;
                client->PlayerUpdate();
            }
            else
            { //has item
                client->AddItem(speeditemid, -1);
            }

            research->endtime -= reducetime;

            client->SaveToDB();

            return;
        }
        return;
    }
    if ((command == "getCoinsNeed"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        int positionid = data["positionId"];
        uint32_t castleid = data["castleId"];

        stResearch * research = nullptr;
        for (auto i = 0; i < 25; ++i)
        {
            if (client->research[i].castleid == city->m_castleid)
            {
                research = &client->research[i];
                break;
            }
        }
        if (research == 0)
        {
            gserver.SendObject(client, gserver.CreateError("tech.getCoinsNeed", -99, "Invalid tech."));
            return;
        }

        uint64_t curtime = Utils::time();
        if (research->endtime > curtime) curtime = research->endtime - curtime;
        else curtime = 0;
        obj2["cmd"] = "tech.getCoinsNeed";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        data2["coinsNeed"] = calculatecoinsforspeedup(curtime);

        gserver.SendObject(client, obj2);
        return;
    }
}
