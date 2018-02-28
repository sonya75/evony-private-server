/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pcastle.h"
#include "../spitfire.h"
#include "../City.h"
#include "../Client.h"
#include "../Hero.h"
#include "../Tile.h"
#include "../Map.h"
#include <cmath>

pcastle::pcastle(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pcastle::~pcastle()
{

}

void pcastle::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if ((command == "getAvailableBuildingBean"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        obj2["cmd"] = "castle.getAvailableBuildingBean";
        amf3array buildinglist = amf3array();


        for (int i = 0; i < 35; ++i)
        {
            if (gserver.buildingconfig[i][0].inside != 2)
                continue;
            if (gserver.buildingconfig[i][0].limit > 0 && gserver.buildingconfig[i][0].limit <= city->GetBuildingCount(i))
                continue;
            if (gserver.buildingconfig[i][0].time > 0)
            {
                amf3object parent;
                amf3object conditionbean;

                double costtime = gserver.buildingconfig[i][0].time * 1000;
                double mayorinf = 1;
                if (city->mayor)
                    mayorinf = pow(0.995, city->mayor->GetManagement());
                costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));


                conditionbean["time"] = floor(costtime);
                conditionbean["destructTime"] = gserver.buildingconfig[i][0].destructtime;
                conditionbean["wood"] = (uint32_t)gserver.buildingconfig[i][0].wood;
                conditionbean["food"] = (uint32_t)gserver.buildingconfig[i][0].food;
                conditionbean["iron"] = (uint32_t)gserver.buildingconfig[i][0].iron;
                conditionbean["gold"] = (uint32_t)gserver.buildingconfig[i][0].gold;
                conditionbean["stone"] = (uint32_t)gserver.buildingconfig[i][0].stone;

                amf3array buildings = amf3array();
                amf3array items = amf3array();
                amf3array techs = amf3array();

                for (stPrereq & req : gserver.buildingconfig[i][0].buildings)
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
                for (stPrereq & req : gserver.buildingconfig[i][0].items)
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
                for (stPrereq & req : gserver.buildingconfig[i][0].techs)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        int temp = client->GetResearchLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["typeId"] = req.id;
                        buildings.Add(ta);
                    }
                }

                conditionbean["buildings"] = buildings;
                conditionbean["items"] = items;
                conditionbean["techs"] = techs;
                conditionbean["population"] = gserver.buildingconfig[i][0].population;
                parent["conditionBean"] = conditionbean;
                parent["typeId"] = i;
                buildinglist.Add(parent);
            }
        }


        data2["builingList"] = buildinglist;
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getAvailableBuildingListInside"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        obj2["cmd"] = "castle.getAvailableBuildingListInside";
        amf3array buildinglist = amf3array();

        for (int i = 0; i < 35; ++i)
        {
            if (gserver.buildingconfig[i][0].inside != 1)
                continue;
            if (gserver.buildingconfig[i][0].limit > 0 && gserver.buildingconfig[i][0].limit <= city->GetBuildingCount(i))
                continue;
            if (gserver.buildingconfig[i][0].time > 0)
            {
                amf3object parent;
                amf3object conditionbean;

                double costtime = gserver.buildingconfig[i][0].time;//*1000;
                double mayorinf = 1;
                if (city->mayor)
                    mayorinf = pow(0.995, city->mayor->GetManagement());
                costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));


                conditionbean["time"] = floor(costtime);
                conditionbean["destructTime"] = gserver.buildingconfig[i][0].destructtime;
                conditionbean["wood"] = gserver.buildingconfig[i][0].wood;
                conditionbean["food"] = gserver.buildingconfig[i][0].food;
                conditionbean["iron"] = gserver.buildingconfig[i][0].iron;
                conditionbean["gold"] = 0;
                conditionbean["stone"] = gserver.buildingconfig[i][0].stone;

                amf3array buildings = amf3array();
                amf3array items = amf3array();
                amf3array techs = amf3array();

                for (stPrereq & req : gserver.buildingconfig[i][0].buildings)
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
                for (stPrereq & req : gserver.buildingconfig[i][0].items)
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
                for (stPrereq & req : gserver.buildingconfig[i][0].techs)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        int temp = client->GetResearchLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = temp >= req.level ? true : false;
                        ta["typeId"] = req.id;
                        buildings.Add(ta);
                    }
                }

                conditionbean["buildings"] = buildings;
                conditionbean["items"] = items;
                conditionbean["techs"] = techs;
                conditionbean["population"] = gserver.buildingconfig[i][0].population;
                parent["conditionBean"] = conditionbean;
                parent["typeId"] = i;
                buildinglist.Add(parent);
            }
        }


        data2["builingList"] = buildinglist;
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);




        /*amf3object obj2;
        obj2["cmd"] = "castle.getAvailableBuildingListInside";
        obj2["data"] = amf3object();
        amf3object & data2 = obj2["data"];
        data2["errorMsg"] = "com.evony.entity.tech.impl.FortificationTech";
        data2["ok"] = -99;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);*/

        return;
    }
    if ((command == "getAvailableBuildingListOutside"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        obj2["cmd"] = "castle.getAvailableBuildingListOutside";
        amf3array buildinglist = amf3array();
        // using a fixed building order, temporary fix so that buildings appear in proper order in client
        for (int i: {7,4,5,6})
        {
            if (gserver.buildingconfig[i][0].limit > 0 && gserver.buildingconfig[i][0].limit <= city->GetBuildingCount(i))
                continue;
            if (gserver.buildingconfig[i][0].time > 0)
            {
                amf3object parent;
                amf3object conditionbean;

                double costtime = gserver.buildingconfig[i][0].time * 1000;
                double mayorinf = 1;
                if (city->mayor)
                    mayorinf = pow(0.995, city->mayor->GetManagement());
                costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));


                conditionbean["time"] = floor(costtime);
                conditionbean["destructTime"] = gserver.buildingconfig[i][0].destructtime;
                conditionbean["wood"] = (uint32_t)gserver.buildingconfig[i][0].wood;
                conditionbean["food"] = (uint32_t)gserver.buildingconfig[i][0].food;
                conditionbean["iron"] = (uint32_t)gserver.buildingconfig[i][0].iron;
                conditionbean["gold"] = (uint32_t)gserver.buildingconfig[i][0].gold;
                conditionbean["stone"] = (uint32_t)gserver.buildingconfig[i][0].stone;

                amf3array buildings = amf3array();
                amf3array items = amf3array();
                amf3array techs = amf3array();

                for (stPrereq & req : gserver.buildingconfig[i][0].buildings)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        int temp = city->GetBuildingLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = req.level ? true : false;
                        ta["typeId"] = req.id;
                        buildings.Add(ta);
                    }
                }
                for (stPrereq & req : gserver.buildingconfig[i][0].items)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        int temp = client->itemlist[req.id].count;
                        ta["curNum"] = temp;
                        ta["num"] = req.level;
                        ta["successFlag"] = req.level ? true : false;
                        ta["id"] = gserver.items[req.id].name;
                        items.Add(ta);
                    }
                }
                for (stPrereq & req : gserver.buildingconfig[i][0].techs)
                {
                    if (req.id > 0)
                    {
                        amf3object ta = amf3object();
                        ta["level"] = req.level;
                        int temp = client->GetResearchLevel(req.id);
                        ta["curLevel"] = temp;
                        ta["successFlag"] = req.level ? true : false;
                        ta["typeId"] = req.id;
                        buildings.Add(ta);
                    }
                }

                conditionbean["buildings"] = buildings;
                conditionbean["items"] = items;
                conditionbean["techs"] = techs;
                conditionbean["population"] = gserver.buildingconfig[i][0].population;
                parent["conditionBean"] = conditionbean;
                parent["typeId"] = i;
                buildinglist.Add(parent);
            }
        }


        data2["builingList"] = buildinglist;
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);




        /*amf3object obj2;
        obj2["cmd"] = "castle.getAvailableBuildingListInside";
        obj2["data"] = amf3object();
        amf3object & data2 = obj2["data"];
        data2["errorMsg"] = "com.evony.entity.tech.impl.FortificationTech";
        data2["ok"] = -99;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);*/

        return;
    }
    if ((command == "newBuilding")) //TODO implement hammer queue system
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        obj2["cmd"] = "castle.newBuilding";

        int buildingtype = data["buildingType"];
        int positionid = data["positionId"];

        city->CalculateResources();
        city->CalculateStats();

        if (((positionid < -2) || (positionid > 31)) && ((positionid < 1001) || (positionid > 1040)))
        {
            gserver.SendObject(client, gserver.CreateError("castle.newBuilding", -99, "Position invalid."));
            return;
        }

        if ((buildingtype > 34 || buildingtype <= 0) || city->GetBuilding(positionid)->type || ((gserver.buildingconfig[buildingtype][0].limit > 0) && (gserver.buildingconfig[buildingtype][0].limit <= city->GetBuildingCount(buildingtype))))
        {
            gserver.SendObject(client, gserver.CreateError("castle.newBuilding", -99, "Can't build building."));
            return;
        }

        if ((positionid < 1001) || (positionid > 1040))
        {
            if (positionid - 1000 > ((city->GetBuildingLevel(B_TOWNHALL) * 3) + 10))
            {
                gserver.SendObject(client, gserver.CreateError("castle.newBuilding", -99, "Out of bounds."));
                return;
            }
        }


        for (int i = 0; i < 35; ++i)
        {
            if (city->innerbuildings[i].status != 0)
            {
                // TODO: Support hammer item for multiple constructions at once
                gserver.SendObject(client, gserver.CreateError("castle.newBuilding", -48, "One building allowed to be built at a time."));
                return;
            }
        }
        for (int i = 0; i < 41; ++i)
        {
            if (city->outerbuildings[i].status != 0)
            {
                // TODO: Support hammer item for multiple constructions at once
                gserver.SendObject(client, gserver.CreateError("castle.newBuilding", -48, "One building allowed to be built at a time."));
                return;
            }
        }

        if (!city->CheckBuildingPrereqs(buildingtype, 0))
        {
            gserver.SendObject(client, gserver.CreateError("castle.newBuilding", -99, "Building Prerequisites not met."));
            return;
        }

        if ((gserver.buildingconfig[buildingtype][0].food > city->resources.food)
            || (gserver.buildingconfig[buildingtype][0].wood > city->resources.wood)
            || (gserver.buildingconfig[buildingtype][0].stone > city->resources.stone)
            || (gserver.buildingconfig[buildingtype][0].iron > city->resources.iron)
            || (gserver.buildingconfig[buildingtype][0].gold > city->resources.gold))
        {
            gserver.SendObject(client, gserver.CreateError("castle.newBuilding", -1, "Not enough resources."));
            return;
        }
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);

        city->resources.food -= gserver.buildingconfig[buildingtype][0].food;
        city->resources.wood -= gserver.buildingconfig[buildingtype][0].wood;
        city->resources.stone -= gserver.buildingconfig[buildingtype][0].stone;
        city->resources.iron -= gserver.buildingconfig[buildingtype][0].iron;
        city->resources.gold -= gserver.buildingconfig[buildingtype][0].gold;


        stBuildingAction * ba = new stBuildingAction;

        double costtime = gserver.buildingconfig[buildingtype][0].time;
        double mayorinf = 1;
        if (city->mayor)
            mayorinf = pow(0.995, city->mayor->GetManagement());
        costtime = 1000 * (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));

        city->SetBuilding(buildingtype, 0, positionid, 1, timestamp, timestamp + floor(costtime));

        obj2["cmd"] = "server.BuildComplate";

        data2["buildingBean"] = city->GetBuilding(positionid)->ToObject();
        data2["castleId"] = client->currentcityid;

        gserver.SendObject(client, obj2);

        city->ResourceUpdate();

        client->SaveToDB();
        city->SaveToDB();

        stTimedEvent te;
        ba->city = city;
        ba->client = client;
        ba->positionid = positionid;
        te.data = ba;
        te.accountid = client->accountid;
        te.castleid = city->castleid;
        te.type = DEF_TIMEDBUILDING;

        gserver.AddTimedEvent(te);
        return;
    }
    if ((command == "destructBuilding")) //TODO implement hammer queue system
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        obj2["cmd"] = "castle.destructBuilding";

        int buildingtype = data["buildingType"];
        int positionid = data["positionId"];
        stBuilding * bldg = city->GetBuilding(positionid);

        city->CalculateResources();
        city->CalculateStats();


        if ((bldg->type > 34 || bldg->type <= 0) || (bldg->level == 0))
        {
            gserver.SendObject(client, gserver.CreateError("castle.destructBuilding", -99, "Can't destroy building."));
            return;
        }


        for (int i = 0; i < 35; ++i)
        {
            if (city->innerbuildings[i].status != 0)
            {
                gserver.SendObject(client, gserver.CreateError("castle.destructBuilding", -48, "One building allowed to be built at a time."));
                return;
            }
        }
        for (int i = 0; i < 41; ++i)
        {
            if (city->outerbuildings[i].status != 0)
            {
                gserver.SendObject(client, gserver.CreateError("castle.destructBuilding", -48, "One building allowed to be built at a time."));
                return;
            }
        }

        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);

        stBuildingAction * ba = new stBuildingAction;

        stTimedEvent te;
        ba->city = city;
        ba->client = client;
        ba->positionid = positionid;
        te.data = ba;
        te.type = DEF_TIMEDBUILDING;

        gserver.AddTimedEvent(te);

        double costtime = gserver.buildingconfig[bldg->type][bldg->level - 1].destructtime * 1000;
        double mayorinf = 1;
        if (city->mayor)
            mayorinf = pow(0.995, city->mayor->GetManagement());
        costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));

        ba->city->SetBuilding(bldg->type, bldg->level, positionid, 2, timestamp, timestamp + floor(costtime));

        obj2["cmd"] = "server.BuildComplate";

        data2["buildingBean"] = ba->city->GetBuilding(positionid)->ToObject();
        data2["castleId"] = client->currentcityid;

        gserver.SendObject(client, obj2);

        city->ResourceUpdate();

        client->SaveToDB();
        city->SaveToDB();
        return;
    }
    if ((command == "upgradeBuilding")) //TODO implement hammer queue system
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        city->CalculateResources();
        city->CalculateStats();

        int positionid = data["positionId"];
        stBuilding * bldg = city->GetBuilding(positionid);
        int buildingtype = bldg->type;
        int buildinglevel = bldg->level;

        obj2["cmd"] = "castle.upgradeBuilding";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        if (bldg->status != 0)
        {
            gserver.SendObject(client, gserver.CreateError("castle.upgradeBuilding", -45, "Invalid building status: Your network connection might have experienced delay or congestion. If this error message persists, please refresh or reload this page to login to the game again."));
            return;
        }

        for (int i = 0; i < 35; ++i)
        {
            if (city->innerbuildings[i].status != 0)
            {
                gserver.SendObject(client, gserver.CreateError("castle.upgradeBuilding", -48, "One building allowed to be built at a time."));
                return;
            }
        }
        for (int i = 0; i < 41; ++i)
        {
            if (city->outerbuildings[i].status != 0)
            {
                gserver.SendObject(client, gserver.CreateError("castle.upgradeBuilding", -48, "One building allowed to be built at a time."));
                return;
            }
        }

        if (!city->CheckBuildingPrereqs(buildingtype, buildinglevel))
        {
            gserver.SendObject(client, gserver.CreateError("castle.upgradeBuilding", -99, "Building Prerequisites not met."));
            return;
        }

        if ((gserver.buildingconfig[buildingtype][buildinglevel].food > city->resources.food)
            || (gserver.buildingconfig[buildingtype][buildinglevel].wood > city->resources.wood)
            || (gserver.buildingconfig[buildingtype][buildinglevel].stone > city->resources.stone)
            || (gserver.buildingconfig[buildingtype][buildinglevel].iron > city->resources.iron)
            || (gserver.buildingconfig[buildingtype][buildinglevel].gold > city->resources.gold))
        {
            gserver.SendObject(client, gserver.CreateError("castle.upgradeBuilding", -1, "Not enough resources."));
            return;
        }

        for (stPrereq & req : gserver.buildingconfig[bldg->type][bldg->level].items)
        {
            if (req.id > 0)
            {
                client->AddItem(gserver.items[req.id].name, -req.level);
            }
        }

        gserver.SendObject(client, obj2);

        city->resources.food -= gserver.buildingconfig[buildingtype][buildinglevel].food;
        city->resources.wood -= gserver.buildingconfig[buildingtype][buildinglevel].wood;
        city->resources.stone -= gserver.buildingconfig[buildingtype][buildinglevel].stone;
        city->resources.iron -= gserver.buildingconfig[buildingtype][buildinglevel].iron;
        city->resources.gold -= gserver.buildingconfig[buildingtype][buildinglevel].gold;

        stBuildingAction * ba = new stBuildingAction;

        stTimedEvent te;
        ba->city = city;
        ba->client = client;
        ba->positionid = positionid;
        te.data = ba;
        te.type = DEF_TIMEDBUILDING;

        gserver.AddTimedEvent(te);

        double costtime = gserver.buildingconfig[buildingtype][buildinglevel].time;
        double mayorinf = 1;
        if (city->mayor)
            mayorinf = pow(0.995, city->mayor->GetManagement());
        costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));

        city->ResourceUpdate();

        obj2["cmd"] = "server.BuildComplate";

        ba->city->SetBuilding(buildingtype, buildinglevel, positionid, 1, timestamp, timestamp + (floor(costtime) * 1000));

        data2["buildingBean"] = ba->city->GetBuilding(positionid)->ToObject();
        data2["castleId"] = client->currentcityid;

        gserver.SendObject(client, obj2);

        client->SaveToDB();
        city->SaveToDB();
        return;
    }
    if ((command == "checkOutUpgrade"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        obj2["cmd"] = "castle.checkOutUpgrade";

        int tileposition = data["positionId"];

        //         if (tileposition == -1)
        //             tileposition = 31;
        //         if (tileposition == -2)
        //             tileposition = 32;

        int level = city->GetBuilding(tileposition)->level;
        int id = city->GetBuilding(tileposition)->type;

        if (level >= 10)
        {
            data2["ok"] = 1;
            data2["packageId"] = 0.0;

            amf3object conditionbean;

            conditionbean["time"] = 31536000;
            conditionbean["destructTime"] = gserver.buildingconfig[id][level].destructtime;
            conditionbean["wood"] = 50000000;
            conditionbean["food"] = 50000000;
            conditionbean["iron"] = 50000000;
            conditionbean["gold"] = 0;
            conditionbean["stone"] = 50000000;

            data2["conditionBean"] = conditionbean;

            gserver.SendObject(client, obj2);
            return;
        }

        amf3object conditionbean;

        double costtime = gserver.buildingconfig[id][level].time;

        double mayorinf = 1;
        if (city->mayor)
            mayorinf = pow(0.995, city->mayor->GetManagement());
        costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));

        double desttime = gserver.buildingconfig[id][level].destructtime;
        mayorinf = 1;
        if (city->mayor)
            mayorinf = pow(0.995, city->mayor->GetManagement());
        desttime = (desttime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));

        conditionbean["time"] = floor(costtime);
        conditionbean["destructTime"] = desttime;
        conditionbean["wood"] = (uint32_t)gserver.buildingconfig[id][level].wood;
        conditionbean["food"] = (uint32_t)gserver.buildingconfig[id][level].food;
        conditionbean["iron"] = (uint32_t)gserver.buildingconfig[id][level].iron;
        conditionbean["gold"] = (uint32_t)gserver.buildingconfig[id][level].gold;
        conditionbean["stone"] = (uint32_t)gserver.buildingconfig[id][level].stone;

        amf3array buildings = amf3array();
        amf3array items = amf3array();
        amf3array techs = amf3array();

        for (stPrereq & req : gserver.buildingconfig[id][level].buildings)
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
        for (stPrereq & req : gserver.buildingconfig[id][level].items)
        {
            if (req.id > 0)
            {
                amf3object ta = amf3object();
                int temp = client->GetItemCount(req.id);
                ta["curNum"] = temp;
                ta["num"] = req.level;
                ta["successFlag"] = temp >= req.level ? true : false;
                ta["id"] = gserver.items[req.id].name;
                items.Add(ta);
            }
        }
            for (stPrereq & req : gserver.buildingconfig[id][level].techs)
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
        conditionbean["population"] = gserver.buildingconfig[id][level].population;


        data2["conditionBean"] = conditionbean;
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);


        return;
    }
    if ((command == "speedUpBuildCommand"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        std::string speeditemid;
        int positionid = data["positionId"];
        uint32_t castleid = data["castleId"];

        speeditemid = static_cast<std::string>(data["itemId"]);

        if (speeditemid == "free.speed")
        {
            //check if under 5 mins

            stBuilding * building = city->GetBuilding(positionid);

            if ((building->endtime - building->starttime) <= 5 * 60 * 1000)
            {
                //under 5 mins
                obj2["cmd"] = "castle.speedUpBuildCommand";
                data2["packageId"] = 0.0;
                data2["ok"] = 1;

                gserver.SendObject(client, obj2);

                building->endtime -= 5 * 60 * 1000;

                client->SaveToDB();
                city->SaveToDB();
            }
            else
            {
                //over 5 mins
                gserver.SendObject(client, gserver.CreateError("castle.speedUpBuildCommand", -99, "Invalid speed up."));// TODO get 5 min speed up error - castle.speedUpBuildCommand
            }
        }
        else
        {
            //is not under 5 mins, apply an item

            int cents = 0;
            int reducetime = 0;

            obj2["cmd"] = "castle.speedUpBuildCommand";
            data2["packageId"] = 0.0;
            data2["ok"] = 1;

            gserver.SendObject(client, obj2);

            // TODO reduce time on building being sped up -- castle.speedUpBuildCommand

            stBuilding * building = city->GetBuilding(positionid);
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
                reducetime = ((building->endtime - building->starttime)*0.30);
            }
            else if (speeditemid == "player.speedup.1")
            {
                cents = 175;
                reducetime = ((building->endtime - building->starttime)*0.50);
            }
            else if (speeditemid == "player.destroy.1.a")
            {
                cents = 20;

                if (client->GetItemCount(speeditemid) <= 0)
                {
                    if (client->cents < cents)
                    {
                        // TODO find error value -- castle.speedUpBuildCommand
                        gserver.SendObject(client, gserver.CreateError("castle.speedUpBuildCommand", -99, "Not enough cents."));// not enough item and not enough cents
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

    
                reducetime = (building->endtime - building->starttime);
                building->endtime -= reducetime;
                city->SetBuilding(building->type, 1, building->id, building->status, building->starttime, building->endtime);

                obj2["cmd"] = "server.BuildComplate";

                data2["buildingBean"] = city->GetBuilding(positionid)->ToObject();
                data2["castleId"] = client->currentcityid;

                gserver.SendObject(client, obj2);

                client->SaveToDB();
                city->SaveToDB();
                return;
            }
            else if (speeditemid == "coins.speed")
            {
                //TODO: find value of cents based on time remaining
                cents = 200;
                reducetime = (building->endtime - building->starttime);
            }

            if (client->GetItemCount(speeditemid) <= 0)
            {
                if (client->cents < cents)
                {
                    // TODO find error value -- castle.speedUpBuildCommand
                    gserver.SendObject(client, gserver.CreateError("castle.speedUpBuildCommand", -99, "Not enough cents."));// not enough item and not enough cents
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

            building->endtime -= reducetime;

            city->SetBuilding(building->type, building->level, building->id, building->status, building->starttime, building->endtime);

            obj2["cmd"] = "server.BuildComplate";

            data2["buildingBean"] = city->GetBuilding(positionid)->ToObject();
            data2["castleId"] = client->currentcityid;

            gserver.SendObject(client, obj2);

            client->SaveToDB();
            city->SaveToDB();

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

        obj2["cmd"] = "castle.getCoinsNeed";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        data2["coinsNeed"] = 200;//TODO calculate correct cents cost based on time

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "cancleBuildCommand"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];
        int positionid = data["positionId"];

        obj2["cmd"] = "castle.cancleBuildCommand";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        std::list<stTimedEvent>::iterator iter;
        if (gserver.buildinglist.size() > 0)
        {
            for (iter = gserver.buildinglist.begin(); iter != gserver.buildinglist.end();)
            {
                stBuildingAction * ba = (stBuildingAction *)iter->data;
                if (ba->positionid == positionid)
                {
                    Client * client = ba->client;
                    PlayerCity * city = ba->city;
                    stBuilding * bldg = ba->city->GetBuilding(ba->positionid);
                    if (bldg->status != 0)
                    {
                        bldg->status = 0;

                        if (bldg->status == 1)
                        {
                            stResources res;
                            res.food = gserver.buildingconfig[bldg->type][bldg->level].food;
                            res.wood = gserver.buildingconfig[bldg->type][bldg->level].wood;
                            res.stone = gserver.buildingconfig[bldg->type][bldg->level].stone;
                            res.iron = gserver.buildingconfig[bldg->type][bldg->level].iron;
                            res.gold = gserver.buildingconfig[bldg->type][bldg->level].gold;
                            city->resources += res;
                        }

                        gserver.buildinglist.erase(iter++);

                        if (bldg->level == 0)
                            ba->city->SetBuilding(0, 0, ba->positionid, 0, 0.0, 0.0);
                        else
                            ba->city->SetBuilding(bldg->type, bldg->level, ba->positionid, 0, 0.0, 0.0);

                        client->CalculateResources();
                        city->CalculateStats();

                        //gserver.SendObject(client->socket, obj);

                        obj2["cmd"] = "server.BuildComplate";

                        data2["buildingBean"] = ba->city->GetBuilding(positionid)->ToObject();
                        data2["castleId"] = client->currentcityid;

                        gserver.SendObject(client, obj2);

                        client->SaveToDB();
                        city->SaveToDB();

                        delete ba;

                        return;
                    }
                    else
                    {
                        gserver.SendObject(client, gserver.CreateError("castle.cancleBuildCommand", -99, "Not being constructed."));
                        return;
                    }
                }
                ++iter;
            }
        }
        return;
    }
    if (command == "saveCastleSignList")
    {
        int32_t tx = data["x"];
        int32_t ty = data["y"];
        uint64_t tid = data["id"];//tile id
        Tile * tile = nullptr;

        stCastleSign cst;

        cst.x = tx;
        cst.y = ty;

        tile = gserver.game_map->GetTileFromID(tid);

        if (tile->type != CASTLE)
        {
            //need an error message
            gserver.CreateError("castle.saveCastleSignList", -99, "Invalid coordinates");
            return;
        }

        client->castlesignlist.push_back(cst);

        client->CastleSignUpdate();
        return;
    }
    if (command == "deleteCastleSignList")
    {
        uint64_t tid = data["id"];//tile id

        for (std::vector<stCastleSign>::iterator iter = client->castlesignlist.begin(); iter != client->castlesignlist.end(); ++iter )
        {
            if (iter->id == tid)
            {
                client->castlesignlist.erase(iter);
                client->CastleSignUpdate();
                return;
            }
        }
        //need an error message
        gserver.CreateError("castle.saveCastleSignList", -99, "Invalid castle");
        return;
    }
}
