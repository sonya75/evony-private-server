/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pcommon.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../Alliance.h"
#include "../Map.h"
#include "../Tile.h"

#include <Poco/Data/RecordSet.h>


using Poco::Data::RecordSet;
using namespace Poco::Data;
using namespace Poco::Data::Keywords;

pcommon::pcommon(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pcommon::~pcommon()
{

}

void pcommon::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if (command == "worldChat")
    {
        obj2["cmd"] = "common.worldChat";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);

        amf3object obj3;
        obj3["cmd"] = "server.ChannelChatMsg";
        obj3["data"] = amf3object();
        amf3object & data3 = obj3["data"];
        if (client->prestigerank == 1)
            data3["msg"] = "<font color=\"#FF0000\">" + (std::string)data["msg"] + "</font>";
        else
        {
            if (client->HasAlliance() && client->GetAlliance()->prestigerank == 1)
                data3["msg"] = "<font color=\"#0066FF\">" + (std::string)data["msg"] + "</font>";
            else
                data3["msg"] = data["msg"];
        }
        data3["languageType"] = 0;
        data3["ownitemid"] = client->icon;
        data3["fromUser"] = client->playername;
        data3["channel"] = "world";

        std::list<Client*>::iterator playeriter;

        if (gserver.ParseChat(client, data["msg"]))
        {
            for (playeriter = gserver.players.begin(); playeriter != gserver.players.end(); ++playeriter)
            {
                Client * client = *playeriter;
                if (client)
                    gserver.SendObject(client, obj3);
            }
        }
        return;
    }
    if (command == "privateChat")
    {
        Client * clnt = gserver.get_client_by_name(data["targetName"]);
        if (!clnt)
        {
            gserver.SendObject(client, gserver.CreateError("common.privateChat", -41, "Player " + (std::string)data["targetName"] + " doesn't exist."));
            return;
        }
        obj2["cmd"] = "common.privateChat";
        data2["msg"] = data["msg"];
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        gserver.SendObject(client, obj2);

        amf3object obj3;
        obj3["cmd"] = "server.PrivateChatMessage";
        obj3["data"] = amf3object();
        amf3object & data3 = obj3["data"];
        data3["msg"] = data["msg"];
        data3["chatType"] = 0;
        data3["ownitemid"] = client->icon;
        data3["fromUser"] = client->playername;
        gserver.SendObject(clnt, obj3);
        return;
    }
    if ((command == "channelChat"))
    {
        obj2["cmd"] = "common.channelChat";
        data2["msg"] = data["msg"];
        data2["ok"] = 1;
        data2["channel"] = "beginner";

        gserver.SendObject(client, obj2);

        amf3object obj3;
        obj3["cmd"] = "server.ChannelChatMsg";
        obj3["data"] = amf3object();
        amf3object & data3 = obj3["data"];
        if (client->prestigerank == 1)
            data3["msg"] = "<font color=\"#FF0000\">" + (std::string)data["sendMsg"] + "</font>";
        else
        {
            if (client->HasAlliance() && client->GetAlliance()->prestigerank == 1)
                data3["msg"] = "<font color=\"#0066FF\">" + (std::string)data["sendMsg"] + "</font>";
            else
                data3["msg"] = data["sendMsg"];
        }
        data3["languageType"] = 0;
        data3["ownitemid"] = client->icon;
        data3["fromUser"] = client->playername;
        data3["channel"] = "beginner";


        std::list<Client*>::iterator playeriter;

        if (gserver.ParseChat(client, data["sendMsg"]))
        {
            for (playeriter = gserver.players.begin(); playeriter != gserver.players.end(); ++playeriter)
            {
                Client * client = *playeriter;
                if (client)
                    gserver.SendObject(client, obj3);
            }
        }
        return;
    }
    if ((command == "allianceChat"))
    {

        if (client->allianceid <= 0)
        {
            gserver.SendObject(client, gserver.CreateError("common.allianceChat", -99, "To send an alliance message, you must be a member of an alliance"));
            return;
        }

        obj2["cmd"] = "common.allianceChat";
        data2["msg"] = data["msg"];
        data2["ok"] = 1;
        data2["channel"] = "alliance";

        gserver.SendObject(client, obj2);

        amf3object obj3;
        obj3["cmd"] = "server.AllianceChatMsg";
        obj3["data"] = amf3object();
        amf3object & data3 = obj3["data"];
        data3["msg"] = data["msg"];
        data3["languageType"] = 0;
        data3["ownitemid"] = client->icon;
        data3["fromUser"] = client->playername;
        data3["channel"] = "alliance";

        Alliance * alliance = client->GetAlliance();

        if (gserver.ParseChat(client, data["msg"]))
        {
            for (Alliance::stMember & member : alliance->members)
            {
                gserver.SendObject(gserver.get_client(member.clientid), obj3);
            }
        }
        return;
    }
    if ((command == "mapInfoSimple"))
    {

        int x1 = data["x1"];
        int x2 = data["x2"];
        int y1 = data["y1"];
        int y2 = data["y2"];
        uint32_t castleId = data["castleId"];


        obj2["cmd"] = "common.mapInfoSimple";
        try
        {
            obj2["data"] = gserver.game_map->GetTileRangeObject(req.conn->client_->accountid, x1, x2, y1, y2);
        }
        catch (...)
        {

        }

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "zoneInfo"))
    {
        obj2["cmd"] = "common.zoneInfo";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        amf3array amfarray = amf3array();
        amf3object zone2;
        for (int i = 0; i < 16; ++i)
        {
            amf3object zone = amf3object();
            zone["id"] = i;
            zone["rate"] = gserver.game_map->stats[i].playerrate;
            zone["name"] = gserver.game_map->states[i];
            zone["playerCount"] = gserver.game_map->stats[i].players;
            zone["castleCount"] = gserver.game_map->stats[i].numbercities;
            amfarray.Add(zone);
        }
        data2["zones"] = amfarray;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getPackage")) //TODO
    {
        obj["ruleId"];//package id to claim
        obj["serial"];//unsure of what this does - is sent as a null string (0 length)

        //on success
        obj2["cmd"] = "common.getPackage";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getPackageList")) //TODO
    {
        obj2["cmd"] = "common.getPackageList";
        data2["packages"] = client->Packages();
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getPackageNumber")) //TODO
    {
        obj2["cmd"] = "common.getPackageNumber";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        data2["number"] = 0;//unclaimed package counts

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "changeUserFace")) //TODO check for valid faceurl (currently can send any link, even offsite)
    {
        obj2["cmd"] = "common.changeUserFace";
        data2["packageId"] = 0.0;

        std::string faceurl = data["faceUrl"];
        int sex = data["sex"];

        if (client->haschangedface || faceurl.length() > 30 || faceurl.length() < 0 || sex < 0 || sex > 1)
        {
            //TODO not a valid error message. need to obtain official response for invalid request
            gserver.SendObject(client, gserver.CreateError("common.changeUserFace", -99, "Invalid setting."));
            return;
        }
        data2["ok"] = 1;
        data2["msg"] = "\xE5\xA4\xB4\xE5\x83\x8F\xE8\xAE\xBE\xE7\xBD\xAE\xE6\x88\x90\xE5\x8A\x9F";

        client->faceurl = faceurl;
        client->sex = sex;

        client->haschangedface = true;

        client->SaveToDB();

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "delUniteServerPeaceStatus")) //TODO is this correct?
    {
        obj2["cmd"] = "common.delUniteServerPeaceStatus";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        data2["number"] = 0;

        client->status = DEF_NORMAL;

        client->SaveToDB();

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getItemDefXml")) //TODO lots of things to fix here.
    {
        //send online user count for fun
        uint32_t tc = 0;
        for (Client * client : gserver.players)
        {
            if (client->socket)
            {
                tc++;
            }
        }
        gserver.SendMessage(client, fmt::format("<font color='#00A2FF'>Welcome to EPS! There are currently <u>{}</u> players online.</font>", tc));
        /*
        Items =        itemType=\"\xE5\xAE\x9D\xE7\x89\xA9\"
        Hot Sale =
        Speed Up =
        Produce =
        Chest =        itemType=\"\xE5\xAE\x9D\xE7\xAE\xB1\"
        */
        obj2["cmd"] = "common.getItemDefXml";

        data2["ok"] = 1;
        data2["packageId"] = 0.0;
        std::string s = R"(<?xml version="1.0" encoding="UTF-8"?>
<itemdef>
<items>
<itemEum id="player.box.compensation.e" name="Compensation Package" itemType="\xE5\xAE\x9D\xE7\xAE\xB1" dayLimit="0" userLimit="1" desc="Includes: 10 amulets, 100 cents." itemDesc="This package was sent to every member of your server to apologize for extended downtime." iconUrl="images/items/chongzhidalibao.png" price="0" playerItem="true"/>
<itemEum id="player.box.present.money.44" name="Pamplona Prize Pack" itemType="\xE5\xAE\x9D\xE7\xAE\xB1" dayLimit="0" userLimit="1" desc="Includes: Wooden Bull Opener, Lion Medal, Rose Medal, Cross Medal, Primary Guidelines, Intermediate Guidelines, War Horn, Corselet, Holy Water, Hero Hunting, Truce Agreement, City Teleporter, Amulet." itemDesc="These packages are delivered as gifts to players for every $30 worth of purchases made during our Run with the Bulls promotion." iconUrl="images/icon/shop/PamplonaPrizePack.png" price="0" playerItem="true"/>
<itemEum id="player.box.present.money.45" name="Hollow Wooden Bull" itemType="\xE5\xAE\x9D\xE7\xAE\xB1" dayLimit="0" userLimit="1" desc="Includes: Chest A (Freedom Medal, Justice Medal, Nation Medal, Michelangelo's Script, Plowshares, Arch Saw, Quarrying Tools, Blower, War Ensign, Excalibur, The Wealth of Nations, Amulet) or Chest B (Primary Guidelines, Intermediate Guidelines, Hero Hunting, Merchant Fleet, Plowshares, Double Saw, Quarrying Tools, Blower, Michelangelo's Script, Tax Policy, The Wealth of Nations) or Chest C (Excalibur, War Horn, Corselet, Truce Agreement, War Ensign, Adv City Teleporter, Michelangelo's Script) " itemDesc="These chests are sent to you as Run with the Bulls gifts from your friends in the game. They require a Wooden Bull Opener to open. You can obtain a Wooden Bull Opener for every $30 worth of purchases made during the Run with the Bulls promotion. When opened, you will receive the contents of Hollow Wooden Bull A, B or C at random." iconUrl="images/icon/shop/HollowWoodenBull.png" price="300" playerItem="true"/>
<itemEum id="player.key.bull" name="Wooden Bull Opener" itemType="\xE5\xAE\x9D\xE7\xAE\xB1" dayLimit="0" userLimit="0" desc="You can use this key to open one Hollow Wooden Bull sent to you by your friends. If you don’t have any Hollow Wooden Bull, you should ask your friends to send you some!" itemDesc="You can open any Hollow Wooden Bull your friends gave you with this key once." iconUrl="images/icon/shop/WoodenBullOpener.png" price="0"/>
<itemEum id="player.running.shoes" name="Extra-Fast Running Shoes" itemType="\xE5\xAE\x9D\xE7\xAE\xB1" dayLimit="0" userLimit="0" desc="A gift from your friends around Run with the Bulls. Use it to get 24 hours of 50% upkeep in ALL your cities any time from July 9th through July 13th. Extra-Fast Running Shoes is stackable (meaning if you already have this buff, using it again will add an additional 24 hours). Once July 14th comes, this item will expire if you haven't used it yet." itemDesc="Get a 24 hours 50% upkeep buff during July 9th and July 13th." iconUrl="images/icon/shop/RunningShoes.png" price="0" playerItem="true"/>
<itemEum id="player.box.test.1" name="Test Item" itemType="\xE5\xAE\x9D\xE7\xAE\xB1" dayLimit="0" userLimit="0" desc="Includes: test items." itemDesc="This package exists as a test." iconUrl="images/items/chongzhidalibao.png" price="10" playerItem="true"/>
<itemEum id="alliance.ritual_of_pact.ultimate" name="Ritual of Pact (Ultimate) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="90" userLimit="0" desc="Ritual of Pact (Ultimate): member limit is 1,000; effective for 90 days; leeway period is 7 days." itemDesc="It allows alliance to increase member limit to 1,000 once applied, which is effective for 90 days, while multiple applications of this item can lengthen the effective period. Once the item effect is due, 7-day leeway is given to the alliance. During this time, new members are denied to be recruited to the alliance. If no further application of the item, the alliance disbands automatically once the 7-day leeway period passes." iconUrl="images/items/Ritual_of_Pact_Ultimate.png" price="75"/>
<itemEum id="player.speak.bronze_publicity_ambassador.permanent" name="Bronze Publicity Ambassador (Permanent) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="3650" userLimit="0" desc="Effect of Bronze Publicity Ambassador (Permanent) can only be replaced by Silver Publicity Ambassador (Permanent) or Gold Publicity Ambassador (Permanent)." itemDesc="Once you apply this item, a special bronze icon will be displayed in front of your name when you speak in the chat box. While this item functions permanently, multiple applications make no difference to the duration of the effective period. Its effect can be replaced by Silver Publicity Ambassador (Permanent) or Gold Publicity Ambassador (Permanent)." iconUrl="images/items/Bronze_Publicity_Ambassador_Permanentb.png" price="75"/>
<itemEum id="player.level.hero.100" name="Mega Hero Levelup" itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="0" userLimit="0" desc="Level up your mayor by 100 levels." itemDesc="Level up your mayor by 100 levels." iconUrl="images/icon/shop/WoodenBullOpener.png" price="50000"/>
<itemEum id="player.speak.bronze_publicity_ambassador.permanent.15" name="Bronze Publicity Ambassador (15-day) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="15" userLimit="0" desc="Once you apply this item, a special bronze icon will be displayed in front of your name when you speak in the chat box. It’s effective for 15 days, while multiple applications can lengthen the effective period. Its effect can be replaced by Bronze Publicity Ambassador (Permanent), Silver Publicity Ambassador (15-day), Silver Publicity Ambassador (Permanent), Gold Publicity Ambassador (15-day) or Gold Publicity Ambassador (Permanent)." itemDesc="Once you apply this item, a special bronze icon will be displayed in front of your name when you speak in the chat box. It’s effective for 15 days, while multiple applications can lengthen the effective period. Its effect can be replaced by Bronze Publicity Ambassador (Permanent), Silver Publicity Ambassador (15-day), Silver Publicity Ambassador (Permanent), Gold Publicity Ambassador (15-day) or Gold Publicity Ambassador (Permanent)." iconUrl="images/items/Bronze_Publicity_Ambassador_15b.png" price="75"/>
<itemEum id="player.speak.gold_publicity_ambassador.15" name="Gold Publicity Ambassador (15-day) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="15" userLimit="0" desc="Once you apply this item, a special gold icon will be displayed in front of your name when you speak in the chat box. It’s effective for 15 days, while multiple applications can lengthen the effective period. Its effect can be replaced by Gold Publicity Ambassador (Permanent)." itemDesc="Effect of Gold Publicity Ambassador (15-day) can only be replaced by Gold Publicity Ambassador (Permanent)." iconUrl="images/items/gold_publicity_ambassador_15b.png" price="75"/>
<itemEum id="player.speak.gold_publicity_ambassador.permanent" name="Gold Publicity Ambassador (Permanent) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="3650" userLimit="0" desc="Once you apply this item, a special gold icon will be displayed in front of your name when you speak in the chat box. While this item functions permanently, multiple applications make no difference to the duration of the effective period. " itemDesc="You're the highest level Publicity Ambassador now." iconUrl="images/items/Gold_Publicity_Ambassador_Permanentb.png" price="75"/>
<itemEum id="player.speak.silver_publicity_ambassador.15" name="Silver Publicity Ambassador (15-day) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="15" userLimit="0" desc="Effect of Silver Publicity Ambassador (15-day) can only be replaced by Silver Publicity Ambassador (Permanent), Gold Publicity Ambassador (15-day) or Gold Publicity Ambassador (Permanent)." itemDesc="Once you apply this item, a special silver icon will be displayed in front of your name when you speak in the chat box. It’s effective for 15 days, while multiple applications can lengthen the effective period. Its effect can be replaced by Silver Publicity Ambassador (Permanent), Gold Publicity Ambassador (15-day) or Gold Publicity Ambassador (Permanent)." iconUrl="images/items/Silver_Publicity_Ambassador_15b.png" price="75"/>
<itemEum id="player.speak.silver_publicity_ambassador.permanent" name="Silver Publicity Ambassador (Permanent) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="3650" userLimit="0" desc="Once you apply this item, a special silver icon will be displayed in front of your name when you speak in the chat box. While this item functions permanently, multiple applications make no difference to the duration of the effective period. Its effect can be replaced by Silver Publicity Ambassador (Permanent) or Gold Publicity Ambassador (Permanent)." itemDesc="Effect of Silver Publicity Ambassador (Permanent) can only be replaced by Gold Publicity Ambassador (Permanent)." iconUrl="images/items/silver_publicity_ambassador_permanentb.png" price="75"/>
<itemEum id="alliance.ritual_of_pact.advanced" name="Ritual of Pact (Advanced) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="15" userLimit="0" desc="Ritual of Pact(Advanced): member limit is 1,000;effective for 15 days; leeway perod is 7 days." itemDesc="It allows alliance to increase member limit to 1,000 once applied, which is effective for 15 days, while multiple applications of this item can lengthen the effective period. Once the item effect is due, 7-day leeway is given to the alliance. During this time, new members are denied to be recruited to the alliance. If no further application of the item, the alliance disbands automatically once the 7-day leeway period passes." iconUrl="images/items/Ritual_of_Pact_Advanced.png" price="75"/>
<itemEum id="alliance.ritual_of_pact.premium" name="Ritual of Pact (Premium) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="30" userLimit="0" desc="Ritual of Pact (Premium): member limit is 1,000; effective for 30 days; leeway period is 7 days." itemDesc="It allows alliance to increase member limit to 1,000 once applied, which is effective for 30 days, while multiple applications of this item can lengthen the effective period. Once the item effect is due, 7-day leeway is given to the alliance. During this time, new members are denied to be recruited to the alliance. If no further application of the item, the alliance disbands automatically once the 7-day leeway period passes." iconUrl="images/items/Ritual_of_Pact_Premium.png" price="75"/>
<itemEum id="consume.1.c" name="Speaker (100 pieces package) " itemType="\xE5\xAE\x9D\xE7\x89\xA9" dayLimit="0" userLimit="0" desc="Used while speaking in World channel and sending group message." itemDesc="It includes 100 Speakers. It costs one Speaker per sentence when chatting in World channel, while sending a group message costs two. Unpack automatically when purchased." iconUrl="images/items/biglaba.png" price="200" playerItem="true"/>
</items>
<special>
<pack id="Special Christmas Chest"/>
<pack id="Special New Year Chest"/>
<pack id="Special Easter Chest"/>
<pack id="Special Evony Happiness Chest"/>
<pack id="Halloween Chest O'Treats"/>
<pack id="Special Thanksgiving Package"/>
<pack id="Secret Santa Chest"/>
<pack id="Valentine's Day Chest "/>
<pack id="St Patrick's Day Chest"/>
<pack id="Special Easter Chest"/>
<pack id="Hollow Wooden Bull"/>
</special>
</itemdef>)";
        data2["itemXml"] = s;


        //gserver.itemxml;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "createNewPlayer"))
    {
        std::string captcha = data["captcha"];
        std::string faceUrl = data["faceUrl"];
        std::string castleName = data["castleName"];
        std::string flag = data["flag"];
        int sex = data["sex"];
        std::string userName = data["userName"];
        int zone = data["zone"];

        if ((client->accountid > 0) && (client->citycount > 0))
        {
            // already has a city
            gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -86, "City/Account exists."));
            return;
        }


        //check for data error
        if (zone < 0 || zone > 15 || userName.length() < 1 || userName.length() > 20 || flag.length() < 1 || flag.length() > 5 || castleName.length() < 1 || castleName.length() > 10
            || faceUrl.length() < 1 || faceUrl.length() > 30 || sex < 0 || sex > 1)
        {
            gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -87, "Invalid data sent."));
            return;
        }

        if (client->accountid == 0)
        {
            gserver.log->info("Account [{}] doesn't exist", userName);
            Session ses(gserver.serverpool->get());
            Statement select(ses);
            select << "SELECT * FROM `accounts` WHERE `username`=?;", use(userName);
            select.execute();
            RecordSet rs(select);

            if (rs.rowCount() > 0)
            {
                //player name exists
                gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -88, "Player name taken"));
                return;
            }
        }
        //else
        {
            //see if state can support a new city
            if (gserver.game_map->openflats[zone] > 0)
            {
                gserver.game_map->openflats[zone]--;
                //create new account, create new city, then send account details

                char tempc[50];
                int randomid = gserver.game_map->GetRandomOpenTile(zone);
                int mapsize = gserver.mapsize;
                GETXYFROMID(randomid);
                int x = xfromid;
                int y = yfromid;
                if (gserver.game_map->tile[randomid].type != FLAT || gserver.game_map->tile[randomid].ownerid != -1)
                {
                    gserver.log->error("Error. Flat not empty!");
                    gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -25, "Error with account creation. #-25"));
                    return;
                }

                std::string user;
                std::string flag2;
                std::string faceUrl2;
                std::string castlename2;
                user = Utils::makesafe(userName);
                castlename2 = Utils::makesafe(castleName);
                faceUrl2 = Utils::makesafe(faceUrl);
                flag2 = Utils::makesafe(flag);
                if (client->accountid == 0)
                {

                    Session ses(gserver.serverpool->get());
                    Statement select(ses);
                    select << "SELECT * FROM `accounts` WHERE `username`=?;", use(userName);
                    select.execute();
                    RecordSet rs(select);
                    uint64_t nixtime = Utils::time();
                    int32_t zero = 0;
                    std::string empty = "";
                    Statement stmt = (ses << "INSERT INTO `accounts` (`parentid`, `username`, `lastlogin`, `creation`, `ipaddress`, `status`, `reason`, `sex`, `flag`, `faceurl`, `buffs`, `research`, `items`, `misc`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, '', '', '', '');", use(client->masteraccountid), use(user), use(nixtime), use(nixtime), use(client->ipaddress), use(zero), use(empty), use(sex), use(flag2), use(faceUrl2));
                    stmt.execute();
                    if (!stmt.done())
                    {
                        gserver.log->error("Unable to create account.");
                        gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -26, "Error with account creation. #-26"));
                        return;
                    }
                    Statement lastinsert = (ses << "SELECT " + LAST_INSERT_ID);
                    lastinsert.execute();
                    RecordSet lsi(lastinsert);
                    lsi.moveFirst();
                    int32_t lsiv = lsi.value(LAST_INSERT_ID).convert<int32_t>();
                    if (lsiv > 0)
                    {
                        client->accountid = lsiv;
                    }
                    else
                    {
                        gserver.log->error("Unable to create account.");
                        gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -27, "Error with account creation. #-27"));
                        return;
                    }

                    //res2 = gserver.sql2->QueryRes("SELECT LAST_INSERT_ID()");
                    //res2->next();
                    //client->accountid = res2->getInt(1);
                    //delete res2;
                }
                client->accountexists = true;

                std::stringstream ss;
                std::string temp;
                ss << "50,10.000000,100.000000,100.000000,100.000000,100.000000,90,0," << Utils::time();
                temp = ss.str();

                //                     if (!gserver.sql2->Query("INSERT INTO `cities` (`accountid`,`misc`,`fieldid`,`name`,`buildings`,`gold`,`food`,`wood`,`iron`,`stone`,`creation`,`transingtrades`,`troop`,`fortification`,`trades`) \
                                                        //                                  VALUES ("XI64", '%s',%d, '%s', '%s',100000,100000,100000,100000,100000,"DBL",'','','','');",
                //                                  client->accountid, (char*)temp.c_str(), randomid, castleName, "31,1,-1,0,0.000000,0.000000", (double)unixtime()))
                Session ses(gserver.serverpool->get());
                uint64_t nixtime = Utils::time();
                int32_t zero = 0;
                std::string empty = "";
                std::string defaultbuildings = "31,1,-1,0,0.000000,0.000000";
                Statement stmt = (ses << "INSERT INTO `cities` (`accountid`,`misc`,`fieldid`,`name`,`buildings`,`gold`,`food`,`wood`,`iron`,`stone`,`creation`,`transingtrades`,`troop`,`fortification`,`trades`,`troopqueues`) VALUES (?, ?, ?, ?, ?,100000,100000,100000,100000,100000,?,'','','','','');",
                                    use(client->accountid), use(temp), use(randomid), use(castleName), use(defaultbuildings), use(nixtime));
                stmt.execute();
                if (!stmt.done())
                {
                    //gserver.FileLog()->Log("Unable to create city.");
                    //error making city
                    gserver.log->error("Error. Unable to insert new city row.");
                    gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -28, "Error with account creation. #-28"));
                    return;
                }

                client->playername = user;
                client->flag = flag2;
                client->faceurl = faceUrl2;
                client->sex = sex;

                PlayerCity * city;

                Statement lastinsert = (ses << "SELECT " + LAST_INSERT_ID);
                lastinsert.execute();
                RecordSet lsi(lastinsert);
                lsi.moveFirst();
                uint32_t lsiv = lsi.value(LAST_INSERT_ID).convert<uint32_t>();
                if (lsiv > 0)
                {
                    client->accountid = lsiv;
                    city = (PlayerCity*)gserver.AddPlayerCity(client, randomid, lsiv);
                }
                else
                {
                    gserver.log->error("Unable to create account.");
                    gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -29, "Error with account creation. #-29"));
                    return;
                }

                //res = gserver.sql2->QueryRes("SELECT LAST_INSERT_ID()");
                //res->next();

                city->ParseBuildings("31,1,-1,0,0.000000,0.000000");
                city->logurl = "images/icon/cityLogo/citylogo_01.png";
                //city->accountid = client->accountid;
                city->cityname = castlename2;
                //city->tileid = randomid;
                client->currentcityid = city->castleid;
                city->creation = Utils::time();
                client->currentcityindex = 0;
                city->SetResources(100000, 100000, 100000, 100000, 100000);
                city->CalculateResources();
                city->CalculateStats();

                /* Send new account data */

                if (client->GetItemCount("consume.1.a") < 10000)
                    client->SetItem("consume.1.a", 10000);
                client->cents = 5000;


                gserver.SortPlayers();
                gserver.SortHeroes();
                gserver.SortCastles();


                amf3object obj3;
                obj3["cmd"] = "common.createNewPlayer";
                obj3["data"] = amf3object();
                amf3object & data3 = obj3["data"];
                data3["packageId"] = 0.0;
                data3["ok"] = 1;
                data3["msg"] = "success";
                data3["player"] = client->ToObject();

                gserver.SendObject(client, obj3);

                client->connected = true;

                if (client->GetItemCount("consume.1.a") < 10000)
                    client->SetItem("consume.1.a", 10000);


                gserver.game_map->CalculateOpenTiles();

                client->SaveToDB();
                city->SaveToDB();


                return;
            }
            else
            {
                //state is full
                gserver.SendObject(client, gserver.CreateError("common.createNewPlayer", -25, "No open flats exist."));
                return;
            }

        }
        return;
    }
    if (command == "setSecurityCode")
    {
        std::string code = data["code"];
        return;
    }
    if (command == "deleteUserAndRestart")
    {
        std::string pwd = data["pwd"];
        data2["packageId"] = 0.0;
        data2["ok"] = 0;


        //call delete stuff
        //EnemyArmysUpdate;//send enemy armies back 
        //FriendArmysUpdate;//send reinforced alliance armies back
        //SelfArmysUpdate;//notify any enemies of army removal
        //NewMail;//clear mail
        //NewReport;//clear reports

        obj["cmd"] = "common.deleteUserAndRestart";

        gserver.SendObject(client, obj2);//return command success ( sends back to login screen )
        return;
    }
}
