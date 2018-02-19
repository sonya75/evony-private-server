/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pquest.h"
#include "../spitfire.h"

pquest::pquest(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pquest::~pquest()
{

}

void pquest::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if (command == "getQuestType") //quest info requested every 3 mins
    {
        VERIFYCASTLEID();

        //TODO create or copy quests

        uint32_t castleid = data["castleId"];
        int questtype = data["type"];

        if (questtype == 1)
        {
            obj2["cmd"] = "quest.getQuestType";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;
            amf3array types = amf3array();

            amf3object dailygift = amf3object();
            dailygift["description"] = "Rebuild";
            dailygift["mainId"] = 1;
            dailygift["isFinish"] = false;
            dailygift["name"] = "Rebuild";
            dailygift["typeId"] = 66;
            types.Add(dailygift);

            dailygift["description"] = "Domain Expansion";
            dailygift["mainId"] = 1;
            dailygift["isFinish"] = false;
            dailygift["name"] = "Domain Expansion";
            dailygift["typeId"] = 72;
            types.Add(dailygift);

            data2["types"] = types;

            gserver.SendObject(client, obj2);
        }
        else if (questtype == 3)//dailies
        {
            obj2["cmd"] = "quest.getQuestType";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;
            amf3array types = amf3array();

            amf3object dailygift = amf3object();
            dailygift["description"] = "Daily Gift";
            dailygift["mainId"] = 3;
            dailygift["isFinish"] = false;
            dailygift["name"] = "Daily Gift";
            dailygift["typeId"] = 94;
            types.Add(dailygift);
            data2["types"] = types;

            gserver.SendObject(client, obj2);
        }

        return;
    }
    if (command == "getQuestList")
    {
        VERIFYCASTLEID();

        uint32_t castleid = data["castleId"];

        obj2["cmd"] = "quest.getQuestList";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        amf3array quests = amf3array();

        amf3object questobject = amf3object();
        questobject["description"] = "quest.getQuestList DESCRIPTION";
        questobject["manual"] = "WIN THE GAME LA";
        questobject["isCard"] = false;
        questobject["isFinish"] = false;
        questobject["name"] = "CHAMPION OF SERVERS";
        questobject["award"] = "Gold 187,438,274,822,314";

        amf3array targets = amf3array();
        amf3object objective = amf3object();
        objective["name"] = "Winning the game.";
        objective["finished"] = false;
        targets.Add(objective);

        questobject["targets"] = targets;
        questobject["questId"] = 63;


        quests.Add(questobject);



        data2["quests"] = quests;



        gserver.SendObject(client, obj2);
        return;
    }
}
