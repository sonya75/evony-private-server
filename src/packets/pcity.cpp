/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pcity.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"
#include "../Map.h"
#include "../Tile.h"


pcity::pcity(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pcity::~pcity()
{

}

void pcity::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if (command == "advMoveCastle")
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        int32_t targetId = obj["data"]["targetId"];
        int64_t castleId = obj["data"]["castleId"];


        //check if teleported within last 12h
        gserver.SendObject(client, gserver.CreateError("city.advMoveCastle", -90, "Adv City Teleporter is in cooldown."));

        //else

        //this sends a lot of server.CastleFieldUpdate - are those city owned valleys being released due to teleport?

        PlayerCity * city = client->GetFocusCity();

        city->CastleUpdate();


        client->EnemyArmyUpdate();
        client->FriendArmyUpdate();
        client->SelfArmyUpdate();
        client->AddItem("player.more.castle.1.a", -1);
        client->ItemUpdate("player.more.castle.1.a");
        client->BuffUpdate("MoveCastleCoolDownBuff", "Adv City Teleporter in cooldown.", Utils::time() + (12 * 60 * 60 * 1000));


        amf3object obj3;
        obj3["cmd"] = "city.advMoveCastle";
        obj3["data"] = amf3object();
        amf3object & data3 = obj3["data"];
        data3["packageId"] = 0.0;
        data3["ok"] = 1;
        gserver.SendObject(client, obj3);
        return;
    }
    if (command == "moveCastle")
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        if (client->GetItemCount("consume.move.1") == 0)
        {
            gserver.SendObject(client, gserver.CreateError("city.moveCastle", -90, "No City Teleporter."));
            return;
        }

        int8_t zoneId = obj["data"]["zoneId"];
        int64_t castleId = obj["data"]["castleId"];

        if (zoneId < 0 || zoneId > 15)
        {
            //TODO: trying to teleport to an invalid zone - punishment? remove item and put on cd?
            client->AddItem("consume.move.1", -1);
            gserver.SendObject(client, gserver.CreateError("city.moveCastle", -90, "Trying to teleport to an invalid zone."));
            return;
        }


        //check if teleported within last 12h
        //gserver.SendObject(client, gserver.CreateError("city.moveCastle", -90, "Adv City Teleporter is in cooldown."));

        //else

        //this sends a lot of server.CastleFieldUpdate - are those city owned valleys being released due to teleport?
        
        PlayerCity * city = client->GetFocusCity();


        if (gserver.map->m_openflats[zoneId] > 0)
        {
            gserver.map->m_openflats[zoneId]--;
            //create new account, create new city, then send account details

            char tempc[50];
            int randomid = gserver.map->GetRandomOpenTile(zoneId);
            int mapsize = gserver.mapsize;
            GETXYFROMID(randomid);
            int x = xfromid;
            int y = yfromid;
            if (gserver.map->m_tile[randomid].m_type != FLAT || gserver.map->m_tile[randomid].m_ownerid != 0)
            {
                gserver.SendObject(client, gserver.CreateError("city.moveCastle", -25, "No open flats exist."));
                return;
            }

            //TODO: have to check army statuses before porting - for now port anyway for testing purposes

            if (city->armymovement.size() > 0)
            {
                gserver.SendObject(client, gserver.CreateError("city.moveCastle",-77,"You must recall all of your troops before you teleport your city."));
                return;
            }
            for (stArmyMovement* pl : client->friendarmymovement) {
                if (pl->targetfieldid==city->m_tileid && pl->direction == DIRECTION_STAY) {
                    gserver.SendObject(client, gserver.CreateError("city.moveCastle",-77,"You must recall all of your troops before you teleport your city."));
                    return;
                } 
            }
            gserver.map->m_tile[city->m_tileid].m_type = FLAT;
            city->m_tileid = randomid;
            gserver.map->m_tile[randomid].m_type = CASTLE;
            gserver.map->m_tile[randomid].m_city = city;
            gserver.map->m_tile[randomid].m_castleid = city->m_castleid;


            //enemy armies continue to attack the flat left behind
            //allied armies continue to try to reinforce the flat (and fail upon arrival)
            //self armies - you cannot teleport with armies out
            city->CastleUpdate();
            client->EnemyArmyUpdate();
            client->FriendArmyUpdate();
            client->SelfArmyUpdate();
            client->AddItem("consume.move.1", -1);

            gserver.map->CalculateOpenTiles();

            amf3object obj3;
            obj3["cmd"] = "city.moveCastle";
            obj3["data"] = amf3object();
            amf3object & data3 = obj3["data"];
            data3["packageId"] = 0.0;
            data3["ok"] = 1;
            gserver.SendObject(client, obj3);
            return;
        }

        gserver.SendObject(client, gserver.CreateError("city.moveCastle", -25, "No open flats exist."));
        return;
    }
    if ((command == "modifyCastleName"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t castleid = data["castleId"];
        std::string logurl = data["logUrl"];
        std::string name = data["name"];

        city->m_cityname = name;
        city->m_logurl = logurl;
        // TODO check valid name and error reporting - city.modifyCastleName

        obj2["cmd"] = "city.modifyCastleName";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);

        city->SaveToDB();

        return;
    }
    if ((command == "modifyFlag"))
    {
        std::string flag = data["newFlag"];

        if (client->GetItemCount("consume.changeflag.1") <= 0)
        {
            gserver.SendObject(client, gserver.CreateError("city.modifyFlag", -99, "Not enough item."));
            return;
        }

        client->AddItem("consume.changeflag.1", -1);
    
        client->flag = flag;
        // TODO check valid name and error reporting - city.modifyFlag

        client->PlayerInfoUpdate();

        obj2["cmd"] = "city.modifyFlag";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);

        client->SaveToDB();

        return;
    }
    if ((command == "setStopWarState"))
    {
        std::string pass = data["passWord"];
        std::string itemid = data["ItemId"];

        if (client->password != pass)
        {
            gserver.SendObject(client, gserver.CreateError("city.setStopWarState", -50, "Incorrect account or password."));
            return;
        }

        if (client->GetItemCount(itemid) > 0)
        {
            client->AddItem(itemid, -1);
            client->status = DEF_TRUCE;
        }
        else
        {
            int32_t cost = gserver.GetItem(itemid)->cost;
            if (client->cents < cost)
            {
                gserver.SendObject(client, gserver.CreateError("city.setStopWarState", -99, "Not enough cents."));
                return;
            }
            client->cents -= cost;
        }
        if (itemid == "player.peace.1")
        {
            client->SetBuff("PlayerPeaceBuff", "Truce Agreement activated", timestamp + (12 * 60 * 60 * 1000));
        }
        else
        {
            gserver.SendObject(client, gserver.CreateError("city.setStopWarState", -99, "Invalid state."));
            return;
        }

        obj2["cmd"] = "city.setStopWarState";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);

        client->SaveToDB();

        return;
    }
    if (command == "modifyUserName")
    {
        std::string newname = data["userName"];
        std::string itemid = data["itemId"];

        if (itemid != "player.name.1.a")
        {
            gserver.SendObject(client, gserver.CreateError("city.modifyUserName", -99, "No item exists."));
            return;
        }

        if (client->GetItemCount("player.name.1.a") <= 0)
        {
            gserver.SendObject(client, gserver.CreateError("city.modifyUserName", -99, "Not enough item."));
            return;
        }

        // TODO check valid name and error reporting - city.modifyUserName
        client->AddItem("player.name.1.a", -1);

        client->playername = newname;
        client->PlayerInfoUpdate();


        obj2["cmd"] = "city.modifyUserName";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);

        client->SaveToDB();
        return;
    }
}

