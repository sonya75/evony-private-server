/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/


#include "spitfire.h"

#include <fstream>
#include <thread>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Exception.h>
#include <Poco/NumberFormatter.h>

#include "Tile.h"
#include "amf3.h"
#include "Hero.h"
#include "AllianceMgr.h"
#include "Alliance.h"
#include "PlayerCity.h"
#include "NpcCity.h"
#include "Client.h"
#include "defines.h"

#define DEF_NOMAPDATA

using namespace std::chrono_literals;

using Poco::Data::RecordSet;
using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

spitfire::spitfire()
    : signals_(io_service_)
    , acceptor_(io_service_)
    , socket_(io_service_)
    , request_handler_()
    , acceptorpolicy_(io_service_)
    , socketpolicy_(io_service_)
    , request_handlerpolicy_()
    , players_online(0)
    , game_map(nullptr)
{
    start_logging();
    log->info("EPS Server starting.");
 
    MySQL::Connector::registerConnector();

    accountpool = nullptr;
    serverpool = nullptr;

    SaveThreadRunning = false;
    serverstatus = SERVERSTATUS_STOPPED;//offline
    servername = "";

    mapsize = 800;
    heroid = 1000;
    cityid = 100000;
    allianceid = 100;
    itemcount = 0;

    armycounter = 0;
    tecounter = 0;
    players_online = 0;
    ltime = 0;
//     alliances = nullptr;

    TimerThreadRunning = false;

    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals_.async_wait(
        [this](asio::error_code /*ec*/, int /*signo*/)
    {
        // The server is stopped by canceling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
        Shutdown();
    });
}

spitfire::~spitfire()
{
    if (game_map)
        delete game_map;
    delete accountpool;
    delete serverpool;
    MySQL::Connector::unregisterConnector();
}

void spitfire::run(size_t thread_pool_size)
{
    printf("Start up procedure\n");


#pragma region data load

    //Read Item XML info
    //     FILE * itemxml;
    //     fopen_s(&itemxml, "itemxml.xml", "r");
    //     fseek(itemxml, 0, SEEK_END);
    //     int32_t filesize = ftell(itemxml);
    //     char * itemxmlbuff = new char[filesize+1];
    //     itemxmlbuff[filesize+1] = 0;
    //     fseek(itemxml, 0, SEEK_SET);
    //     if (fread_s(itemxmlbuff, filesize, 1, filesize, itemxml) != filesize)
    //     {
    //         fclose(itemxml);
    //         throw(std::exception("No settings exist for server."));
    //         //error reading
    //     }
    //     fclose(itemxml);
    // 
    //     this->itemxml = itemxmlbuff;
    //     delete[] itemxmlbuff;




    //good to keep on hand for future
    //     typedef Poco::Tuple<std::string, std::string, std::string, uint32_t, std::string, uint32_t, uint32_t, std::string, uint32_t> PAccount;
    //     PAccount account;
    //    Statement stmt = ( ses << "SELECT * FROM account WHERE `name`=? AND `password`=?", use(username), use(password), into(account), now );
    //    account.get<0>()

    int count = 0;


    //     if (rs.rowCount() == 0)
    //     {
    //         //no settings exist
    //         throw(std::exception("No settings exist for server."));
    //         return;
    //     }
    // 
    // 
    //     rs.moveFirst();
    //     //client.name = (string)rs.value("name");
    //     client.name = rs.value("name").convert<string>();
    //     client.password = rs.value("password").convert<string>();
    //     client.email = rs.value("email").convert<string>();
    //     client.message = rs.value("reason").convert<string>();
    //     client.lastip = rs.value("lastip").convert<string>();
    //     client.xcoins = rs.value("xcoins").convert<uint32_t>();
    //     client.lastlogin = rs.value("lasttime").convert<uint32_t>();
    //     client.status = rs.value("status").convert<uint32_t>();
    //     client.online = rs.value("online").convert<uint32_t>();







    game_map = new Map(mapsize);
    alliances = new AllianceMgr();

    log->info("Loading configurations.");

    try
    {
        Poco::Data::Session ses(accountpool->get());

        log->info("Loading config_building.");
        {
            Statement select(ses);
            select << "SELECT * FROM `config_building` order by buildingid;";
            select.execute();
            RecordSet rs(select);

            rs.moveFirst();


            for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
            {
                char * tok;
                char * ch = 0, *cr = 0;

                int32_t cfgid = rs.value("buildingid").convert<int32_t>();
                int32_t level = rs.value("level").convert<int32_t>() - 1;
                buildingconfig[cfgid][level].time = rs.value("buildtime").convert<double>();
                buildingconfig[cfgid][level].destructtime = rs.value("buildtime").convert<double>() / 2;
                buildingconfig[cfgid][level].food = rs.value("food").convert<int32_t>();
                buildingconfig[cfgid][level].wood = rs.value("wood").convert<int32_t>();
                buildingconfig[cfgid][level].stone = rs.value("stone").convert<int32_t>();
                buildingconfig[cfgid][level].iron = rs.value("iron").convert<int32_t>();
                buildingconfig[cfgid][level].gold = rs.value("gold").convert<int32_t>();
                buildingconfig[cfgid][level].prestige = rs.value("gold").convert<int32_t>();

                std::string strt = rs.value("prereqbuilding").convert<std::string>();
                char * str = (char*)strt.c_str();

                int x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    buildingconfig[cfgid][level].buildings.push_back(prereq);
                    x++;
                }

                strt = rs.value("prereqtech").convert<std::string>();
                str = (char*)strt.c_str();
                x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    buildingconfig[cfgid][level].techs.push_back(prereq);
                    x++;
                }

                strt = rs.value("prereqitem").convert<std::string>();
                str = (char*)strt.c_str();
                x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    buildingconfig[cfgid][level].items.push_back(prereq);
                    x++;
                }

                buildingconfig[cfgid][level].limit = rs.value("blimit").convert<int32_t>();
                buildingconfig[cfgid][level].inside = rs.value("inside").convert<int32_t>();
            }
        }

        log->info("Loading config_troops.");
        {
            Statement select(ses);
            select << "SELECT * FROM `config_troops`;";
            select.execute();
            RecordSet rs(select);

            rs.moveFirst();


            for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
            {
                char * tok;
                char * ch = 0, *cr = 0;

                int32_t cfgid = rs.value("troopid").convert<int32_t>();
                troopconfig[cfgid].time = rs.value("buildtime").convert<double>();
                troopconfig[cfgid].destructtime = 0.0;
                troopconfig[cfgid].food = rs.value("food").convert<int32_t>();
                troopconfig[cfgid].wood = rs.value("wood").convert<int32_t>();
                troopconfig[cfgid].stone = rs.value("stone").convert<int32_t>();
                troopconfig[cfgid].iron = rs.value("iron").convert<int32_t>();
                troopconfig[cfgid].gold = rs.value("gold").convert<int32_t>();
                troopconfig[cfgid].inside = rs.value("inside").convert<int32_t>();
                troopconfig[cfgid].population = rs.value("population").convert<int32_t>();

                std::string strt = rs.value("prereqbuilding").convert<std::string>();
                char * str = (char*)strt.c_str();
                int x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    troopconfig[cfgid].buildings.push_back(prereq);
                    x++;
                }

                strt = rs.value("prereqtech").convert<std::string>();
                str = (char*)strt.c_str();
                x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    troopconfig[cfgid].techs.push_back(prereq);
                    x++;
                }

                strt = rs.value("prereqitem").convert<std::string>();
                str = (char*)strt.c_str();
                x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    troopconfig[cfgid].items.push_back(prereq);
                    x++;
                }
            }
        }


        log->info("Loading config_research.");
        {
            Statement select(ses);
            select << "SELECT * FROM `config_research`;";
            select.execute();
            RecordSet rs(select);

            rs.moveFirst();


            for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
            {
                char * tok;
                char * ch = 0, *cr = 0;

                int32_t cfgid = rs.value("researchid").convert<int32_t>();
                int32_t level = rs.value("level").convert<int32_t>() - 1;
                researchconfig[cfgid][level].time = rs.value("buildtime").convert<int32_t>();
                researchconfig[cfgid][level].destructtime = rs.value("buildtime").convert<int32_t>() / 2;
                researchconfig[cfgid][level].food = rs.value("food").convert<int32_t>();
                researchconfig[cfgid][level].wood = rs.value("wood").convert<int32_t>();
                researchconfig[cfgid][level].stone = rs.value("stone").convert<int32_t>();
                researchconfig[cfgid][level].iron = rs.value("iron").convert<int32_t>();
                researchconfig[cfgid][level].gold = rs.value("gold").convert<int32_t>();

                std::string strt = rs.value("prereqbuilding").convert<std::string>();
                char * str = (char*)strt.c_str();

                int x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    researchconfig[cfgid][level].buildings.push_back(prereq);
                    x++;
                }

                strt = rs.value("prereqtech").convert<std::string>();
                str = (char*)strt.c_str();
                x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    researchconfig[cfgid][level].techs.push_back(prereq);
                    x++;
                }

                strt = rs.value("prereqitem").convert<std::string>();
                str = (char*)strt.c_str();
                x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    researchconfig[cfgid][level].items.push_back(prereq);
                    x++;
                }
            }
        }


        log->info("Loading config_items.");
        {
            Statement select(ses);
            select << "SELECT * FROM `config_items`;";
            select.execute();
            RecordSet rs(select);

            rs.moveFirst();


            for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
            {
                int32_t id = rs.value("id").convert<int32_t>();
                items[id].name = rs.value("name").convert<std::string>();
                items[id].desc = rs.value("description").convert<std::string>();
                items[id].cost = rs.value("cost").convert<int32_t>();
                items[id].saleprice = rs.value("cost").convert<int32_t>();
                items[id].buyable = rs.value("buyable").convert<bool>();
                items[id].cangamble = rs.value("cangamble").convert<bool>();
                items[id].daylimit = rs.value("daylimit").convert<int32_t>();
                items[id].type = rs.value("itemtype").convert<int32_t>();
                items[id].rarity = rs.value("rarity").convert<int32_t>();
                ++itemcount;

                if (items[id].cangamble)
                {
                    switch (items[id].rarity)
                    {
                        default:
                        case 5:
                            gambleitems.common.push_back(&items[id]);
                            break;
                        case 4:
                            gambleitems.special.push_back(&items[id]);
                            break;
                        case 3:
                            gambleitems.rare.push_back(&items[id]);
                            break;
                        case 2:
                            gambleitems.superrare.push_back(&items[id]);
                            break;
                        case 1:
                            gambleitems.ultrarare.push_back(&items[id]);
                            break;
                    }
                }
            }
        }
    }
    catch (Poco::Data::MySQL::ConnectionException& e)\
    {
        std::string file = __FILE__;
        log->error("ConnectionException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::Data::MySQL::StatementException& e)
    {
        std::string file = __FILE__;
        log->error("StatementException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::Data::MySQL::MySQLException& e)
    {
        std::string file = __FILE__;
        log->error("MySQLException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::InvalidArgumentException& e)
    {
        std::string file = __FILE__;
        log->error("InvalidArgumentException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::Data::ConnectionFailedException& e)
    {
        std::string file = __FILE__;
        log->error("ConnectionFailedException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (std::exception & e)
    {
        std::string file = __FILE__;
        log->error("std::exception: {} {} {}", file, __LINE__, e.what());
    }

    log->info("Loading map data.");


#ifndef DEF_NOMAPDATA
    {
        Poco::Data::Session ses2(serverpool->get());
        Statement select(ses2);
        select << "SELECT `id`,`ownerid`,`type`,`level` FROM `tiles` ORDER BY `id` ASC;";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();

        do
        {
            int64_t id = rs.value("id").convert<int64_t>();
            int64_t ownerid = rs.value("ownerid").convert<int64_t>();
            int64_t type = rs.value("type").convert<int64_t>();
            int64_t level = rs.value("level").convert<int64_t>();

            game_map->tile[id].id = id;
            game_map->tile[id].ownerid = ownerid;
            game_map->tile[id].type = type;
            game_map->tile[id].level = level;

            if (type == NPC)
            {
                NpcCity * city = (NpcCity *)AddNpcCity(id);
                city->Initialize(true, true);
                city->level = level;
                city->ownerid = ownerid;
                game_map->tile[id].zoneid = game_map->GetStateFromID(id);
            }

            if ((id + 1) % ((mapsize*mapsize) / 100) == 0)
            {
                log->info(fmt::format("{}%", int((double(double(id + 1) / (mapsize*mapsize)))*double(100))));
            }
        } while (rs.moveNext());
    }
#else
    //this fakes map data
    for (int x = 0; x < (mapsize*mapsize); x += 1/*(mapsize*mapsize)/10*/)
    {
        game_map->tile[x].id = x;
        game_map->tile[x].ownerid = -1;
        //make every tile an npc
        //map->tile[x].type = NPC;
        game_map->tile[x].type = rand() % 9 + 1;
        game_map->tile[x].level = (rand() % 10) + 1;

        if (game_map->tile[x].type > 6)
            game_map->tile[x].type = 10;

        if (game_map->tile[x].type == 6)
        {
            game_map->tile[x].type = NPC;
            NpcCity * city = (NpcCity *)AddNpcCity(game_map->tile[x].id);
            city->Initialize(true, true);
            city->level = game_map->tile[x].level;
            city->ownerid = game_map->tile[x].ownerid;
            game_map->tile[game_map->tile[x].id].zoneid = game_map->GetStateFromID(game_map->tile[x].id);
        }


        if ((x + 1) % ((mapsize*mapsize) / 100) == 0)
        {
            log->info("{}%", int((double(double(x + 1) / (mapsize*mapsize)))*double(100)));
        }
    }
#endif

    game_map->CalculateOpenTiles();

    log->info("Loading account data.");

    uint64_t accountcount = 0;
    {
        Poco::Data::Session ses2(serverpool->get());
        Statement select(ses2);
        select << "SELECT COUNT(*) AS a FROM `accounts`;";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();
        accountcount = rs.value("a").convert<uint64_t>();
    }

    {
        Poco::Data::Session ses2(serverpool->get());
        //SQLITE//Statement stmt = (ses2 << "SELECT accounts.*,account.email,account.password FROM accounts LEFT JOIN account ON (account.id=accounts.parentid) ORDER BY accounts.accountid ASC;");
        Statement stmt = (ses2 << "SELECT accounts.*," + dbmaintable + ".account.email," + dbmaintable + ".account.password FROM accounts LEFT JOIN " + dbmaintable + ".account ON (" + dbmaintable + ".account.id=accounts.parentid) ORDER BY accounts.accountid ASC;");
        stmt.execute();
        bool test = stmt.done();
        RecordSet rs(stmt);

        rs.moveFirst();

        for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
        {
            count++;

            Client * client = new_client();
            client->accountexists = true;
            client->accountid = rs.value("accountid").convert<int64_t>();
            client->masteraccountid = rs.value("parentid").convert<int64_t>();
            client->playername = rs.value("username").convert<std::string>();
            client->password = rs.value("password").convert<std::string>();
            client->email = rs.value("email").convert<std::string>();

            client->allianceid = rs.value("allianceid").convert<int32_t>();
            client->alliancerank = rs.value("alliancerank").convert<int16_t>();
            client->lastlogin = rs.value("lastlogin").convert<double>();
            client->creation = rs.value("creation").convert<double>();
            client->status = rs.value("status").convert<int32_t>();
            client->sex = rs.value("sex").convert<int32_t>();
            client->flag = rs.value("flag").convert<std::string>();
            client->faceurl = rs.value("faceurl").convert<std::string>();
            client->cents = rs.value("cents").convert<int32_t>();
            client->Prestige(rs.value("prestige").convert<double>());
            client->honor = rs.value("honor").convert<double>();

            client->ParseBuffs(rs.value("buffs").convert<std::string>());
            client->ParseResearch(rs.value("research").convert<std::string>());
            client->ParseItems(rs.value("items").convert<std::string>());
            client->ParseMisc(rs.value("misc").convert<std::string>());

            client->CheckBeginner(false);

            try
            {
                Poco::Data::Session ses(serverpool->get());
                Statement select(ses);
                select << "SELECT * FROM `mail` WHERE `receiverid`=" << client->accountid;
                select.execute();
                RecordSet rs(select);

                rs.moveFirst();

                stMail mail;

                for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
                {
                    mail.title = rs.value("title").convert<std::string>();
                    mail.content = rs.value("content").convert<std::string>();
                    mail.senttime = rs.value("senttime").convert<uint64_t>();
                    mail.readtime = rs.value("readtime").convert<uint64_t>();
                    mail.mailid = rs.value("pid").convert<int32_t>();
                    mail.playerid = rs.value("senderid").convert<int64_t>();
                    mail.type_id = rs.value("type").convert<int8_t>();

                    client->maillist.push_back(mail);
                    if (client->mailpid <= mail.mailid)
                        client->mailpid = mail.mailid + 1;
                }
            }
            SQLCATCH(return;);




            if (accountcount > 101)
            {
                if ((count) % ((accountcount) / 100) == 0)
                {
                    log->info("{}%", int((double(double(count) / accountcount + 1))*double(100)));
                }
            }
        }
    }


    log->info("Loading city data.");



    uint64_t citycount = 0;
    {
        Poco::Data::Session ses2(serverpool->get());
        Statement select(ses2);
        select << "SELECT COUNT(*) AS a FROM `cities`;";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();
        citycount = rs.value("a").convert<uint64_t>();
    }

    count = 0;


    {
        Poco::Data::Session ses2(serverpool->get());
        Statement select(ses2);
        select << "SELECT * FROM `cities`";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();

        for (auto i = 0; i < rs.rowCount(); ++i, rs.moveNext())
        {
            count++;

            auto accountid = rs.value("accountid").convert<int64_t>();
            auto cityid = rs.value("id").convert<int64_t>();
            auto fieldid = rs.value("fieldid").convert<int32_t>();
            auto * client = get_client(accountid);
            GETXYFROMID(fieldid);
            if (client == nullptr)
            {
                log->error("City exists with no account attached. - accountid:{} cityid:{} coord:({},{})", accountid, cityid, xfromid, yfromid);
                continue;
            }
            auto * city = (PlayerCity *)AddPlayerCity(client, fieldid, cityid);
            city->client = client;
            city->resources.food = rs.value("food").convert<double>();
            city->resources.wood = rs.value("wood").convert<double>();
            city->resources.iron = rs.value("iron").convert<double>();
            city->resources.stone = rs.value("stone").convert<double>();
            city->resources.gold = rs.value("gold").convert<double>();
            city->cityname = rs.value("name").convert<std::string>();
            city->logurl = rs.value("logurl").convert<std::string>();
            city->tileid = fieldid;
            //        city->accountid = accountid;
            //        city->client = client;
            city->creation = rs.value("creation").convert<double>();

            //        client->citycount++;

            //         server->map->tile[fieldid].city = city;
            //         server->map->tile[fieldid].npc = false;
            //         server->map->tile[fieldid].ownerid = accountid;
            //         server->map->tile[fieldid].type = CASTLE;
            // 
            //         //server->map->tile[fieldid].zoneid = server->map->GetStateFromID(fieldid);
            // 
            //         server->map->tile[fieldid].castleid = cityid;


            city->ParseTroops(rs.value("troop").convert<std::string>());
            city->ParseBuildings(rs.value("buildings").convert<std::string>());
            city->ParseTroopQueues(rs.value("troopqueues").convert<std::string>());
            city->ParseFortifications(rs.value("fortification").convert<std::string>());
            city->ParseMisc(rs.value("misc").convert<std::string>());

            //city->ParseHeroes(msql->GetString(i, "heroes"));
            //city->ParseTrades(msql->GetString(i, "trades"));
            //city->ParseArmyMovement(msql->GetString(i, "buffs"));

            {
                Poco::Data::Session ses2(serverpool->get());
                Statement select2(ses2);
                select2 << "SELECT * FROM `heroes` WHERE `castleid`=?", use(city->castleid);
                select2.execute();
                RecordSet rs2(select2);

                rs2.moveFirst();

                for (int a = 0; a < rs2.rowCount(); ++a, rs2.moveNext())
                {
                    Hero * temphero;
                    temphero = new Hero();
                    temphero->id = rs2.value("id").convert<uint64_t>();
                    temphero->status = rs2.value("status").convert<int8_t>();
                    temphero->itemid = rs2.value("itemid").convert<int32_t>();
                    temphero->itemamount = rs2.value("itemamount").convert<int32_t>();
                    temphero->castleid = cityid;
                    temphero->ownerid = accountid;

                    temphero->basestratagem = rs2.value("basestratagem").convert<uint32_t>();
                    temphero->stratagem = rs2.value("stratagem").convert<uint32_t>();
                    temphero->stratagemadded = rs2.value("stratagemadded").convert<uint32_t>();
                    temphero->stratagembuffadded = rs2.value("stratagembuffadded").convert<uint32_t>();
                    temphero->basepower = rs2.value("basepower").convert<uint32_t>();
                    temphero->power = rs2.value("power").convert<uint32_t>();
                    temphero->poweradded = rs2.value("poweradded").convert<uint32_t>();
                    temphero->powerbuffadded = rs2.value("powerbuffadded").convert<uint32_t>();
                    temphero->basemanagement = rs2.value("basemanagement").convert<uint32_t>();
                    temphero->management = rs2.value("management").convert<uint32_t>();
                    temphero->managementadded = rs2.value("managementadded").convert<uint32_t>();
                    temphero->managementbuffadded = rs2.value("managementbuffadded").convert<uint32_t>();
                    temphero->logourl = rs2.value("logurl").convert<std::string>();
                    temphero->name = rs2.value("name").convert<std::string>();
                    temphero->remainpoint = rs2.value("remainpoint").convert<uint32_t>();
                    temphero->level = rs2.value("level").convert<uint32_t>();
                    temphero->upgradeexp = rs2.value("upgradeexp").convert<double>();
                    temphero->experience = rs2.value("experience").convert<double>();
                    temphero->loyalty = rs2.value("loyalty").convert<int8_t>();
                    city->heroes[a] = temphero;
                    city->heroes[a]->client = client;
                    if (temphero->status == DEF_HEROMAYOR && city->mayor)
                    {
                        temphero->status = DEF_HEROIDLE;
                    }
                    if (!city->mayor && temphero->status == DEF_HEROMAYOR)
                    {
                        city->mayor = temphero;
                    }
                    if (temphero->id >= heroid)
                        heroid = temphero->id + 1;
                }
            }

            if (cityid >= cityid)
                cityid = cityid + 1;


            client->CalculateResources();

            if (citycount > 101)
            {
                if ((count) % ((citycount) / 100) == 0)
                {
                    log->info("{}%", int((double(double(count) / citycount + 1))*double(100)));
                }
            }
        }
    }



    std::list<stTimedEvent>::iterator iter;
    std::list<Client*>::iterator playeriter;

    for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
    {
        Client * client = *playeriter;
        if (client)
        {
            for (int a = 0; a < 25; ++a)
            {
                if (client->research[a].castleid > 0)
                {
                    PlayerCity * pcity = client->GetCity(client->research[a].castleid);
                    if (pcity != 0)
                    {
                        pcity->researching = true;

                        if (client->research[a].castleid != 0)
                        {
                            stResearchAction * ra = new stResearchAction;

                            stTimedEvent te;
                            ra->city = pcity;
                            ra->client = pcity->client;
                            ra->researchid = a;
                            te.data = ra;
                            te.type = DEF_TIMEDRESEARCH;

                            AddTimedEvent(te);
                        }
                    }
                    else
                    {
                        log->error("Castleid does not exist for research! research:{} castleid:{} accountid:{}", a, client->research[a].castleid, client->accountid);
                    }
                }
            }
        }
    }



    log->info("Loading alliance data.");




    uint64_t alliancecount = 0;
    {
        Poco::Data::Session ses2(serverpool->get());
        Statement select(ses2);
        select << "SELECT COUNT(*) AS a FROM `alliances`";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();
        alliancecount = rs.value("a").convert<uint64_t>();
    }

    count = 0;


    {
        Poco::Data::Session ses2(serverpool->get());
        Statement select(ses2);
        select << "SELECT * FROM `alliances`";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();



        Alliance * alliance;

        for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
        {
            count++;
            alliance = alliances->CreateAlliance(rs.value("name").convert<std::string>(), rs.value("founder").convert<std::string>(), rs.value("id").convert<int64_t>(), false);
            if (alliance == nullptr)
                throw("Unable to create alliance : " + rs.value("name").convert<std::string>() + " ID : " + rs.value("id").convert<std::string>());
            //alliance->allianceid = msql->GetInt(i, "id");
            alliance->ParseMembers(rs.value("members").convert<std::string>());
            for (Alliance::stMember & member : alliance->members)
            {
                if (member.rank == DEF_ALLIANCEHOST)
                {
                    alliance->ownerid = member.clientid;
                    alliance->owner = get_client(member.clientid)->playername;
                    break;
                }
            }
            alliance->ParseRelation(&alliance->enemies, rs.value("enemies").convert<std::string>());
            alliance->ParseRelation(&alliance->allies, rs.value("allies").convert<std::string>());
            alliance->ParseRelation(&alliance->neutral, rs.value("neutrals").convert<std::string>());
            alliance->name = rs.value("name").convert<std::string>();
            alliance->founder = rs.value("founder").convert<std::string>();
            alliance->note = rs.value("note").convert<std::string>();

            if (alliance->allianceid >= allianceid)
                allianceid = alliance->allianceid + 1;


            if (alliancecount > 101)
                if ((count) % ((alliancecount) / 100) == 0)
                {
                    log->info("%d%", int((double(double(count) / alliancecount + 1))*double(100)));
                }
        }
    }


    log->info("Loading Report data.");




    /*uint64_t alliancecount = 0;
    {
    Session ses2(serverpool->get());
    Statement select(ses2);
    select << "SELECT COUNT(*) AS a FROM `alliances`";
    select.execute();
    RecordSet rs(select);

    rs.moveFirst();
    alliancecount = rs.value("a").convert<uint64_t>();
    }

    count = 0;


    {
    Session ses2(serverpool->get());
    Statement select(ses2);
    select << "SELECT * FROM `alliances`";
    select.execute();
    RecordSet rs(select);

    rs.moveFirst();



    Alliance * alliance;

    for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
    {
    count++;
    alliance = alliances->CreateAlliance(rs.value("name").convert<string>(), rs.value("leader").convert<int64_t>(), rs.value("id").convert<int64_t>());
    //alliance->allianceid = msql->GetInt(i, "id");
    alliance->ParseMembers(rs.value("members").convert<string>());
    alliance->ParseRelation(&alliance->enemies, rs.value("enemies").convert<string>());
    alliance->ParseRelation(&alliance->allies, rs.value("allies").convert<string>());
    alliance->ParseRelation(&alliance->neutral, rs.value("neutrals").convert<string>());
    alliance->name = rs.value("name").convert<string>();
    alliance->founder = rs.value("founder").convert<string>();
    alliance->note = rs.value("note").convert<string>();

    if (alliance->allianceid >= allianceid)
    allianceid = alliance->allianceid + 1;


    if (alliancecount > 101)
    if ((count) % ((alliancecount) / 100) == 0)
    {
    consoleLogger->information(Poco::format("%d%", int((double(double(count) / alliancecount + 1))*double(100))));
    }
    }
    }*/

    log->info("Validating players.");


    for (Client * player : players)
    {
        if (player->GetAlliance() == (Alliance*)-1)
        {
            player->allianceid = -1;
            player->alliancerank = 0;
            player->alliancename = "";
        }
    }


//     for (Alliance * alliance : alliances->alliances)
//     {
//         if (alliance == nullptr)
//             continue;
//         std::list<Alliance::stMember> tempmembers = alliance->members;
//         for (Alliance::stMember & member : tempmembers)
//         {
//             Client * client = GetClient(member.clientid);
//             if (client->allianceid != alliance->allianceid)
//             {
//                 client->allianceid = -1;
//                 client->alliancerank = 0;
//                 client->alliancename = "";
//                 alliance->members.remove(member);
//                 alliance->currentmembers--;
//                 continue;
//             }
//             if (client->alliancerank != member.rank)
//             {
//                 client->alliancerank = member.rank;
//             }
//         }
//     }



    log->info("Incrementing valleys.");


    for (int i = 0; i < mapsize*mapsize; ++i)
    {
        if (game_map->tile[i].type < 11 && game_map->tile[i].ownerid < 0)
        {
            game_map->tile[i].level++;
            if (game_map->tile[i].level > 10)
                game_map->tile[i].level = 1;
        }
    }
#pragma endregion

    SortPlayers();
    SortHeroes();
    SortCastles();
    //alliances->SortAlliances();

    // Create a pool of threads to run all of the io_services.
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < thread_pool_size; ++i)
    {
        threads.emplace_back([this]() { io_service_.run(); });
    }

    // Wait for all threads in the pool to exit.
    for (auto & thread : threads)
        thread.join();
}

void spitfire::stop()
{
    io_service_.post([this]()
    {
        Shutdown();
    });
}

void spitfire::start(connection_ptr c)
{
    try
    {
        asio::ip::tcp::endpoint endp = c->socket().remote_endpoint();
        asio::ip::address address = endp.address();
        c->address = address.to_string();
        log->info("Client connected {}", c->address);

        connections_.insert(c);
        c->start();
    }
    catch (std::exception & e)
    {
        //Remote connection closed before connection is finished
        //std::cerr << "Server::start(connection_ptr c) exception: " << e.what() << std::endl;
    }
}

void spitfire::start_policy(connection_ptr c)
{
    try
    {
        asio::ip::tcp::endpoint endp = c->socket().remote_endpoint();
        asio::ip::address address = endp.address();
        c->address = address.to_string();
        log->info("Policy Client connected {}", c->address);

        c->startpolicy();
    }
    catch (std::exception & e)
    {
        //Remote connection closed before connection is finished
        //std::cerr << "Server::startpolicy(connection_ptr c) exception: " << e.what() << std::endl;
    }
}

void spitfire::stop(connection_ptr c, bool checklock)
{
    try
    {
        //        if (checklock)
        //            c->mtx.lock();
        connections_.erase(c);
        asio::error_code ignored_ec;
        c->socket().shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
        c->stop();
        //        if (checklock)
        //            c->mtx.unlock();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
        //        if (checklock)
        //            c->mtx.unlock();
    }
}

void spitfire::stop_all()
{
//    std::for_each(connections_.begin(), connections_.end(),
//                  std::bind(&connection::stop, _1));
    for (auto & conn : connections_)
        conn->stop();
    connections_.clear();
}

void spitfire::do_accept()
{
    acceptor_.async_accept(socket_,
                           [this](asio::error_code ec)
    {
        try
        {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!acceptor_.is_open())
            {
                return;
            }

            if (!ec)
            {
                start(std::make_shared<connection>(
                    std::move(socket_), request_handler_));
            }
        }
        catch (std::exception & e)
        {
            //std::cerr << "uncaught do_accept() exception: " << e.what() << std::endl;
        }
        do_accept();
    });
}

void spitfire::do_acceptpolicy()
{
    acceptorpolicy_.async_accept(socketpolicy_,
                                 [this](asio::error_code ec)
    {
        try
        {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!acceptor_.is_open())
            {
                return;
            }

            if (!ec)
            {
                start_policy(std::make_shared<connection>(
                    std::move(socketpolicy_), request_handlerpolicy_));
            }
        }
        catch (std::exception & e)
        {
            std::cerr << "uncaught do_acceptpolicy() exception: " << e.what() << std::endl;
        }

        do_acceptpolicy();
    });
}

Client * spitfire::new_client()
{
    static uint32_t clientnum;
    if (players_online < maxplayers)
    {
        Client * client = new Client();
        client->internalid = clientnum++;
        players.push_back(client);
        log->info("New client # {}", players.size());
        return client;
    }
    log->error("Client list full! {} players on server.", maxplayers);
    return nullptr;
}

//unused? potentially erroring
int32_t  spitfire::get_client_index(int64_t accountid)
{
    int32_t i = 0;
    for (Client * client : players)
    {
        if (client->accountid == accountid)
            return i;
        ++i;
    }
    return -1;
}
Client * spitfire::get_client_by_castle(int64_t castleid)
{
//     for (Client * client : players)
//     {
//         if (client->GetCity(castleid) != 0)
//             return client;
//     }
    return 0;
}
Client * spitfire::get_client(int64_t accountid)
{
    for (Client * client : players)
    {
        if (client->accountid == accountid)
            return client;
    }
    return 0;
}
Client * spitfire::GetClientByParent(int64_t accountid)
{
    for (Client * client : players)
    {
        if (client->masteraccountid == accountid)
            return client;
    }
    return 0;
}
Client * spitfire::get_client_by_name(std::string name)
{
    for (Client * client : players)
    {
        if (client->playername == name)
            return client;
    }
    return 0;
}

void spitfire::close_client(Client* client, int typecode, std::string message) const
{
    try
    {
        if (client && client->socket)
        {
            amf3object obj;
            obj["cmd"] = "server.ConnectionLost";//gameClient.kickout";
            obj["data"] = amf3object();
            amf3object & data = obj["data"];
            data["reasonCode"] = typecode;
            data["msg"] = message;

            SendObject(client, obj);
            //client->socket->socket().shutdown(asio::socket_base::shutdown_type::shutdown_both);
            client->socket->stop();
        }
    }
    catch (std::exception & e)
    {
        std::cerr << "uncaught CloseClient() exception: " << e.what();
    }
}

City* spitfire::AddPlayerCity(Client* client, int tileid, uint64_t castleid)
{
    PlayerCity * city = new PlayerCity();
    city->type = CASTLE;
    city->logurl = "";
    city->status = 0;
    city->tileid = tileid;
    city->castleid = castleid;
    city->accountid = client->accountid;

    city->SetBuilding(31, 1, -1, 0, 0, 0);
    city->SetResources(0, 0, 0, 0, 0);
    city->client = client;
    std::string temp = "";
    std::stringstream ss;
    ss << "50,10.000000,100.000000,100.000000,100.000000,100.000000,90,0," << (double)Utils::time();
    temp = ss.str();

    city->ParseMisc(temp);

    client->citylist.push_back(city);
    client->citycount++;

    game_map->tile[tileid].city = city;
    city.push_back(city);


    game_map->tile[tileid].npc = false;
    game_map->tile[tileid].ownerid = client->accountid;
    game_map->tile[tileid].city = city;
    game_map->tile[tileid].type = CASTLE;
    game_map->tile[tileid].castleid = castleid;

    return city;
}

City * spitfire::AddNpcCity(int tileid)
{
    NpcCity * city = new NpcCity();
    city->tileid = tileid;
    city->type = NPC;
    city->cityname = "Barbarian City";
    city->status = 0;
    city.push_back(city);
    game_map->tile[tileid].city = city;
    game_map->tile[tileid].npc = true;
    game_map->tile[tileid].type = NPC;
    return city;
}

void spitfire::MassMessage(std::string str, bool nosender /* = false*/, bool tv /* = false*/, bool all /* = false*/)
{
    for (Client * client : players)
    {
        amf3object obj;
        obj["cmd"] = "server.SystemInfoMsg";
        obj["data"] = amf3object();
        amf3object & data = obj["data"];
        data["alliance"] = all;
        data["tV"] = tv;
        data["noSenderSystemInfo"] = nosender;
        data["msg"] = str;

        SendObject(client, obj);
    }
}

void spitfire::SendMessage(Client * client, std::string str, bool nosender /* = false*/, bool tv /* = false*/, bool all /* = false*/) const
{
    if (client && client->socket)
    {
        amf3object obj;
        obj["cmd"] = "server.SystemInfoMsg";
        obj["data"] = amf3object();
        amf3object & data = obj["data"];
        data["alliance"] = all;
        data["tV"] = tv;
        data["noSenderSystemInfo"] = nosender;
        data["msg"] = str;

        SendObject(client, obj);
    }
}

void spitfire::Shutdown()
{
    stop();

    //stop timer thread so the save
    //thread gets an accurate snapshot
    serverstatus = SERVERSTATUS_SHUTDOWN;

    try
    {
        acceptor_.close();
    }
    catch (std::exception & e)
    {
        log->error("exception: {}", e.what());
    }

    //disconnect everyone
    MassDisconnect();

    stop_all();

    //shutdown complete
    //do anything else that might need to be done
    io_service_.stop();
}

int32_t spitfire::CalcTroopSpeed(PlayerCity * city, stTroops & troops, int32_t starttile, int32_t endtile) const
{
    int32_t fx, fy, tx, ty;
    GETXYFROMID4(fx, fy, starttile, mapsize);
    GETXYFROMID4(tx, ty, endtile, mapsize);

    double line = sqrt(pow(abs(fx - tx), abs(fx - tx)) + pow(abs(fy - ty), abs(fy - ty)));

    double slowest = 10000;//either
    double mslowest = 10000;//mech
    double fslowest = 10000;//foot

    if (troops.catapult) { mslowest = 80; }
    else if (troops.ballista) { mslowest = 100; }
    else if (troops.ram) { mslowest = 120; }
    else if (troops.transporter) { mslowest = 150; }
    else if (troops.cataphract) { mslowest = 750; }
    else if (troops.cavalry) { mslowest = 1000; }

    if (troops.worker) { fslowest = 180; }
    else if (troops.warrior) { fslowest = 200; }
    else if (troops.archer) { fslowest = 250; }
    else if (troops.sword) { fslowest = 275; }
    else if (troops.pike) { fslowest = 300; }
    else if (troops.scout) { fslowest = 3000; }

    double maxtechlevel = city->GetBuildingLevel(B_ACADEMY);
    double compass = city->client->GetResearchLevel(T_COMPASS);//1.5x speed max
    double hbr = city->client->GetResearchLevel(T_HORSEBACKRIDING);//2x speed max

    hbr = (maxtechlevel >= hbr) ? hbr : maxtechlevel;
    compass = (maxtechlevel >= compass) ? compass : maxtechlevel;

    mslowest *= (hbr * 0.05) + 1;
    fslowest *= (compass * 0.10) + 1;

    mslowest /= 1000;
    fslowest /= 1000;

    slowest = (mslowest < fslowest) ? mslowest : fslowest;

    slowest /= 60;

    return line / slowest;
}

void spitfire::SendObject(Client* c, const amf3object& object) const
{
    if ((!c) || (!c->connected) || (c->socket == nullptr))
        return;
    SendObject(c->socket, object);
}

bool spitfire::ParseChat(Client * client, std::string str)
{
    if (str.size() > 0)
    {
        char tempstr[200];
        memset(tempstr, 0, 200);
        memcpy(tempstr, str.c_str(), str.size() >= 200 ? 200 : str.size());
        if (!memcmp(tempstr, "\\", 1))
        {
            char * command, *ctx;
            command = strtok_s(tempstr, "\\ ", &ctx);

            amf3object obj;
            obj["cmd"] = "server.SystemInfoMsg";
            obj["data"] = amf3object();
            amf3object & data = obj["data"];
            data["alliance"] = false;
            data["tV"] = false;
            data["noSenderSystemInfo"] = true;

            if (command == 0)
            {
                return false;
            }

            if (!strcmp(command, "motd"))
            {
                if ((client->allianceid <= 0) || (client->alliancerank < 3))
                {
                    data["msg"] = "You do not have an alliance or are not the proper rank.";
                }
                else
                {
                }
            }
            else if (!strcmp(command, "restart"))
            {
                if (client->playername == "Daisy")
                {
                    std::list<Client*>::iterator playeriter;
                    for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
                        close_client(*playeriter, 1, "");
                    Shutdown();
                }
                //                 amf3object sobj;
                //                 sobj["cmd"] = "server.ConnectionLost";
                //                 sobj["data"] = amf3object();
                //                 amf3object & sdata = sobj["data"];
                //                 sdata["reasonCode"] = 1;

                //                    SendObject((*playeriter)->socket, sobj);
            }
            else if (!strcmp(command, "online"))
            {
                uint32_t tc = 0;
                for (Client * client : players)
                {
                    if (client->socket)
                    {
                        tc++;
                    }
                }
                SendMessage(client, fmt::format("<font color='#00A2FF'>There are currently <u>{}</u> players online.</font>", tc));
            }
            else if (!strcmp(command, "debug"))
            {
                client->debugmode = !client->debugmode;
            }
            else if (!strcmp(command, "massmsg"))
            {
                if (client->playername == "Daisy")
                {
                    char * substr = tempstr + strlen(command) + 2;
                    int substrlen = strlen(substr);
                    for (int i = 0; i < substrlen; ++i)
                    {
                        if (!memcmp(((char*)substr) + i, "&lt;", 4))
                        {
                            //match
                            *(char*)(substr + i) = '<';
                            memcpy(substr + i + 1, substr + i + 4, substrlen - i);
                        }
                    }
                    MassMessage(substr);
                }
            }
            else if (!strcmp(command, "cents"))
            {
                data["msg"] = "50000 Cents granted.";
                client->cents += 50000;
                client->PlayerUpdate();
            }
            /*            else if (!strcmp(command, "pres"))
            {
            int test = 12786431;
            stringstream ss;
            ss << test << "prestige granted.";
            data["msg"] = ss.str().c_str();
            client->prestige += test;
            client->PlayerUpdate();
            }*/
            else if (!strcmp(command, "resources"))
            {
                stResources res = { 10000000, 10000000, 10000000, 10000000, 10000000 };
                client->GetFocusCity()->resources += res;
                client->GetFocusCity()->ResourceUpdate();
            }
            else if (!strcmp(command, "tempvar"))
            {
                command = strtok_s(0, " ", &ctx);
                client->icon = atoi(command);
                std::string s;
                s = "Tempvar set to '<u>";
                s += command;
                s += "</u>'.";
                data["msg"] = s;
            }
            else if (!strcmp(command, "kick"))
            {
                command = strtok_s(0, " |", &ctx);
                if (!command)
                {
                    data["msg"] = "Invalid syntax";
                    SendObject(client, obj);
                    return false;
                }

                int typecode = atoi(command);
                command = strtok_s(0, "|", &ctx);
                if (!command)
                {
                    data["msg"] = "Invalid syntax";
                    SendObject(client, obj);
                    return false;
                }
                std::string msg = command;
                close_client(client, typecode, msg);
            }
            else if (!strcmp(command, "buff"))
            {
                command = strtok_s(0, " |", &ctx);
                if (!command)
                {
                    data["msg"] = "Invalid syntax";
                    SendObject(client, obj);
                    return false;
                }

                std::string buffid = command;
                command = strtok_s(0, "| ", &ctx);
                if (!command)
                {
                    data["msg"] = "Invalid syntax";
                    SendObject(client, obj);
                    return false;
                }
                std::string desc = command;
                command = strtok_s(0, "| ", &ctx);
                if (!command)
                {
                    data["msg"] = "Invalid syntax";
                    SendObject(client, obj);
                    return false;
                }
                int64_t var = atoi(command) + Utils::time();
                std::string s;
                s = "Buff set to '<u>";
                s += buffid;
                s += "</u>'.";
                data["msg"] = s;

                client->SetBuff(buffid, desc, var);
            }
            else if (!strcmp(command, "commands"))
            {

            }
            else if (!strcmp(command, "commands"))
            {
                data["msg"] = "Commands: motd cents resources";
            }
            else
            {
                std::string s;
                s = "Command '<u>";
                s += command;
                s += "</u>' does not exist.";
                data["msg"] = s;
            }
            SendObject(client, obj);
            return false;
        }
    }
    return true;
}

int16_t spitfire::GetRelation(int32_t client1, int32_t client2) const
{
//     if (client1 >= 0 && client2 >= 0)
//     {
//         return alliances->GetRelation(client1, client2);
//     }
    return 0;
}

void spitfire::TimerThread()
{
    uint64_t t1htimer;
    uint64_t t30mintimer;
    uint64_t t6mintimer;
    uint64_t t5mintimer;
    uint64_t t3mintimer;
    uint64_t t1mintimer;
    uint64_t t5sectimer;
    uint64_t t1sectimer;
    uint64_t t100msectimer;
    uint64_t ltime;

   std::list<stTimedEvent>::iterator iter;
   std::list<Client*>::iterator playeriter;

    t1htimer = t30mintimer = t6mintimer = t5mintimer = t3mintimer = t1mintimer = t5sectimer = t1sectimer = t100msectimer = Utils::time();

    while (serverstatus == SERVERSTATUS_ONLINE)
    {
        try
        {
            ltime = Utils::time();

            if (t1sectimer < ltime)
            {
                t1sectimer += 1000;
            }
            if (t100msectimer < ltime)
            {
                for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
                {
                    Client * client = *playeriter;
//                    client->lists.lock();
                    for (int j = 0; j < client->citylist.size(); ++j)
                    {
                        PlayerCity * city = client->citylist[j];
                        std::vector<stTroopQueue>::iterator tqiter;
                        for (tqiter = city->troopqueue.begin(); tqiter != city->troopqueue.end();)
                        {
                            std::list<stTroopTrain>::iterator iter;
                            iter = tqiter->queue.begin();
                            if (iter != tqiter->queue.end() && iter->endtime <= ltime)
                            {
                                //troops done training
                                double gain = iter->count * GetPrestigeOfAction(DEF_TRAIN, iter->troopid, 1, city->level);
                                client->Prestige(gain);
                                client->PlayerUpdate();
                                if (city->mayor)
                                {
                                    city->mayor->experience += gain;
                                    city->HeroUpdate(city->mayor, 2);
                                }

                                if (tqiter->positionid == -2)
                                {
                                    city->SetForts(iter->troopid, iter->count);
                                }
                                else
                                    city->SetTroops(iter->troopid, iter->count);
                                tqiter->queue.erase(iter++);
                                if (iter != tqiter->queue.end())
                                    iter->endtime = ltime + iter->costtime;
                            }
                            ++tqiter;
                        }
                    }
//                    client->lists.unlock();
                }

                /*
                if (armylist.size() > 0)
                {
                std::list<stTimedEvent> tarmylist = armylist;
                for (iter = tarmylist.begin(); iter != tarmylist.end();)
                {
                //armylist.erase(iter++);
                stArmyMovement * am = (stArmyMovement *)iter->data;
                PlayerCity * fcity = (PlayerCity *)am->city;
                Client * fclient = am->client;
                Hero * fhero = am->hero;
                Tile * tile = map->GetTileFromID(am->targetfieldid);
                if (am->reachtime + am->resttime < ltime)
                {
                if (am->direction == 1)
                {
                //start combat, deposit resources, reinforce city, etc
                combat fight(tile, *am);
                fight.run();
                stTimedEvent te;
                stArmyMovement * amreturn = new stArmyMovement;

                amreturn->city = am->city;
                amreturn->client = am->client;

                amreturn->hero = am->hero;
                amreturn->heroname = am->hero->name;
                amreturn->direction = 2;
                amreturn->resources += fight.returned;
                amreturn->startposname = am->city->cityname;
                amreturn->king = am->client->playername;
                amreturn->troops = am->troops;
                amreturn->starttime = Utils::time();
                amreturn->armyid = armycounter++;
                amreturn->reachtime = amreturn->starttime + (am->reachtime - am->starttime - am->resttime);
                amreturn->herolevel = am->hero->level;
                amreturn->resttime = 0;
                amreturn->missiontype = am->missiontype;
                amreturn->startfieldid = am->city->tileid;
                amreturn->targetfieldid = am->targetfieldid;
                amreturn->targetposname = map->GetTileFromID(am->targetfieldid)->GetName();
                amreturn->hero->status = DEF_HERORETURN;

                te.data = amreturn;
                te.type = DEF_TIMEDARMY;

                AddTimedEvent(te);
                am->client->armymovement.remove(am);
                am->client->armymovement.push_back(amreturn);
                am->client->SelfArmyUpdate();

                armylist.remove(*iter);
                am->client->HeroUpdate(am->hero->id, am->hero->castleid);
                delete am;
                }
                else
                {
                //returning to city from attack/reinforce/etc

                am->hero->status = DEF_HEROIDLE;

                am->city->resources += am->resources;
                ((PlayerCity*)am->city)->troops += am->troops;

                ((PlayerCity*)am->city)->HeroUpdate(am->hero, 2);
                am->client->SelfArmyUpdate();
                am->client->PlayerUpdate();
                ((PlayerCity*)am->city)->TroopUpdate();
                ((PlayerCity*)am->city)->ResourceUpdate();


                armylist.remove(*iter);
                delete am;
                }
                }
                ++iter;


                //
                //                         fclient->SelfArmyUpdate();
                //
                //                         stTimedEvent te;
                //                         te.data = am;
                //                         te.type = DEF_TIMEDARMY;
                //
                //                         gserver->AddTimedEvent(te);

                // recv: server.HeroUpdate
                // recv: server.InjuredTroopUpdate
                // recv: server.SelfArmysUpdate
                // recv: server.SelfArmysUpdate
                // recv: server.HeroUpdate
                // recv: server.SelfArmysUpdate
                // recv: server.HeroUpdate
                // recv: server.NewReport
                // recv: server.SelfArmysUpdate
                // recv: server.HeroUpdate
                // recv: server.TroopUpdate
                // recv: server.SelfArmysUpdate
                // recv: server.HeroUpdate
                // recv: server.SystemInfoMsg
                // recv: server.SelfArmysUpdate
                // recv: server.ResourceUpdate
                // recv: server.SelfArmysUpdate
                }
                }
                */

                //TODO: some buildings not being set to notupgrading properly. add a new check for all buildings under construction to see if they are due to finish? maybe?
                if (buildinglist.size() > 0)
                {
                    for (iter = buildinglist.begin(); iter != buildinglist.end();)
                    {
                        stBuildingAction * ba = (stBuildingAction *)iter->data;
                        Client * client = ba->client;
                        PlayerCity * city = ba->city;
                        stBuilding * bldg = ba->city->GetBuilding(ba->positionid);
//                        client->lists.lock();
                        if (bldg->endtime < ltime)
                        {
                            if (bldg->status == 1)
                            {
                                //build/upgrade
                                bldg->status = 0;
                                bldg->level++;
                                ba->city->SetBuilding(bldg->type, bldg->level, ba->positionid, 0, 0.0, 0.0);

                                if (bldg->type == B_INN)
                                {
                                    for (int i = 0; i < 10; ++i)
                                    {
                                        if (city->innheroes[i])
                                        {
                                            delete city->innheroes[i];
                                            city->innheroes[i] = 0;
                                        }
                                    }
                                }
                                if (bldg->type == B_TOWNHALL)
                                {
                                    if (bldg->level == 5)
                                    {
                                        //beginner protection removed
                                        client->Beginner(false);
                                        CreateMail("System", client->playername, "Beginner's Protection Expired", "Dear player,\n\
This letter is dedicated to inform you that the Beginner's Protection is expired (7 days protection period was due or Town Hall has been upgraded to level 5). Now you are formally joining the battlefields of Evony. This is a competitive world. You never know when your enemies will come to your city gate. Therefore countermeasures must be made to secure your realm. It's recommended that you upgrade the Walls and build more fortified units, making it a great cost to those who lay their eyes upon your territory.In initial phase, your troops are far from enough to edge out the rivals, therefore you are also advised to build more Warehouses to preserve the resources from plundering.\n\
Hang in there and stay alert.Good luck!", MAIL_SYSTEM);
                                        //title- Beginner's Protection Expired
                                        //content- Dear player,
                                        //This letter is dedicated to inform you that the Beginner's Protection is expired (7 days protection period was due or Town Hall has been upgraded to level 5). Now you are formally joining the battlefields of Evony. This is a competitive world. You never know when your enemies will come to your city gate. Therefore countermeasures must be made to secure your realm. It's recommended that you upgrade the Walls and build more fortified units, making it a great cost to those who lay their eyes upon your territory.In initial phase, your troops are far from enough to edge out the rivals, therefore you are also advised to build more Warehouses to preserve the resources from plundering.
                                        //Hang in there and stay alert.Good luck!
                                    }
                                }

                                amf3object obj = amf3object();
                                obj["cmd"] = "server.BuildComplate";
                                obj["data"] = amf3object();

                                amf3object & data = obj["data"];

                                data["buildingBean"] = bldg->ToObject();
                                data["castleId"] = city->castleid;

                                buildinglist.erase(iter++);

                                double gain = GetPrestigeOfAction(DEF_BUILDING, bldg->type, bldg->level, city->level);
                                client->Prestige(gain);

                                delete ba;

                                client->CalculateResources();
                                city->CalculateStats();
                                if (city->mayor)
                                {
                                    city->mayor->experience += gain;
                                    city->HeroUpdate(city->mayor, 2);
                                }
                                //city->CastleUpdate();
                                client->PlayerUpdate();
                                city->ResourceUpdate();

                                SendObject(client, obj);

//                                client->lists.unlock();
                                continue;
                            }
                            else if (bldg->status == 2)
                            {
                                //destruct
                                bldg->status = 0;
                                bldg->level--;

                                stResources res;
                                res.food = buildingconfig[bldg->type][bldg->level].food / 3;
                                res.wood = buildingconfig[bldg->type][bldg->level].wood / 3;
                                res.stone = buildingconfig[bldg->type][bldg->level].stone / 3;
                                res.iron = buildingconfig[bldg->type][bldg->level].iron / 3;
                                res.gold = buildingconfig[bldg->type][bldg->level].gold / 3;
                                ba->city->resources += res;

                                if (bldg->level == 0)
                                    ba->city->SetBuilding(0, 0, ba->positionid, 0, 0.0, 0.0);
                                else
                                    ba->city->SetBuilding(bldg->type, bldg->level, ba->positionid, 0, 0.0, 0.0);

                                delete ba;

                                client->CalculateResources();
                                city->CalculateStats();


                                amf3object obj = amf3object();
                                obj["cmd"] = "server.BuildComplate";
                                obj["data"] = amf3object();

                                amf3object & data = obj["data"];

                                data["buildingBean"] = bldg->ToObject();
                                data["castleId"] = city->castleid;

                                SendObject(client, obj);

                                buildinglist.erase(iter++);

                                client->CalculateResources();
                                city->CalculateStats();
                                city->ResourceUpdate();


//                                client->lists.unlock();
                                continue;
                            }
                        }
//                        client->lists.unlock();
                        ++iter;
                    }
                }

                if (researchlist.size() > 0)
                {
                    for (iter = researchlist.begin(); iter != researchlist.end();)
                    {
                        stResearchAction * ra = (stResearchAction *)iter->data;
                        Client * client = ra->client;
                        PlayerCity * city = ra->city;
//                        client->lists.lock();
                        if (ra->researchid != 0)
                        {
                            if (client->research[ra->researchid].endtime < ltime)
                            {
                                city->researching = false;
                                client->research[ra->researchid].level++;
                                client->research[ra->researchid].endtime = 0;
                                client->research[ra->researchid].starttime = 0;
                                client->research[ra->researchid].castleid = 0;

                                amf3object obj = amf3object();
                                obj["cmd"] = "server.ResearchCompleteUpdate";
                                obj["data"] = amf3object();

                                amf3object & data = obj["data"];

                                data["castleId"] = city->castleid;


                                researchlist.erase(iter++);

                                double gain = GetPrestigeOfAction(DEF_RESEARCH, ra->researchid, client->research[ra->researchid].level, city->level);
                                client->Prestige(gain);


                                client->CalculateResources();
                                city->CalculateStats();
                                if (city->mayor)
                                {
                                    city->mayor->experience += gain;
                                    city->HeroUpdate(city->mayor, 2);
                                }
                                //city->CastleUpdate();
                                client->PlayerUpdate();
                                city->ResourceUpdate();

                                SendObject(client, obj);

                                delete ra;

//                                client->lists.unlock();
                                continue;
                            }
                        }
                        else
                        {
                            researchlist.erase(iter++);
//                            client->lists.unlock();
                            continue;
                        }
//                        client->lists.unlock();
                        ++iter;
                    }
                }
                t100msectimer += 100;
            }
            if (t5sectimer < ltime)
            {
                //market.Process();
                SortPlayers();
                SortHeroes();
                SortCastles();
                alliances->SortAlliances();
                for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
                {
                    Client * client = *playeriter;
                    for (int j = 0; j < client->bufflist.size(); ++j)
                    {
                        if (client->bufflist[j].id.length() != 0)
                        {
                            if (ltime > client->bufflist[j].endtime)
                            {
                                if (client->bufflist[j].id == "PlayerPeaceBuff")
                                {
                                    client->SetBuff("PlayerPeaceCoolDownBuff", "Truce Agreement in cooldown.", ltime + (12 * 60 * 60 * 1000));
                                }
                                client->RemoveBuff(client->bufflist[j].id);
                            }
                        }
                    }
                }

                t5sectimer += 5000;
            }
            if (t1mintimer < ltime)
            {
                for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
                {
                    Client * client = *playeriter;
                    client->CalculateResources();

                    if (client->socknum > 0 && (client->currentcityindex != -1) && client->citylist[client->currentcityindex])
                    {
                        client->citylist[client->currentcityindex]->ResourceUpdate();
                    }
                }

                CheckRankSearchTimeouts(ltime);

                t1mintimer += 60000;
            }
            if (t3mintimer < ltime)
            {
                //                if (!savethreadrunning)
                //                    savethread = shared_ptr<thread>(new thread(std::bind(std::mefun(&Server::SaveData), this)));
                //                std::thread timerthread(std::bind(std::mefun(&Server::TimerThread), gserver));
                //                 //hSaveThread = (HANDLE)_beginthreadex(0, 0, SaveData, 0, 0, &uAddr);
                //t3mintimer += 180000;
                t3mintimer += 180000;
            }
            //             if (t5mintimer < ltime)
            //             {
            // #ifndef WIN32
            //             if (pthread_create(&hSaveThread, NULL, SaveData, 0))
            //             {
            //                 SFERROR("pthread_create");
            //             }
            // #else
            //                 //hSaveThread = (HANDLE)_beginthreadex(0, 0, SaveData, 0, 0, &uAddr);
            // #endif
            //                 t5mintimer += 300000;
            //             }
            if (t6mintimer < ltime)
            {
                try
                {
                    for (int i = 0; i < city.size(); ++i)
                    {
                        if (city.at(i)->type == NPC)
                        {
                            ((NpcCity*)city.at(i))->CalculateStats(true, true);
                        }
                        else if (city.at(i)->type == CASTLE)
                        {
                            ((PlayerCity*)city.at(i))->RecalculateCityStats();
                        }
                    }
                }
                catch (...)
                {
                    log->error("exception: {} : {}", (std::string)__FILE__, __LINE__);
                    //error?
                }
                t6mintimer += 360000;
            }
            //             if (t30mintimer < ltime)
            //             {
            //                 LOCK(RANKEDLIST);
            //                 gserver->SortPlayers();
            //                 gserver->SortHeroes();
            //                 gserver->SortCastles();
            //                 gserver->alliances->SortAlliances();
            //                 UNLOCK(RANKEDLIST);
            //                 t30mintimer += 1800000;
            //             }
            if (t1htimer < ltime)
            {

                t1htimer += 3600000;
            }

            uint64_t t1 = Utils::time();
            //packet queue - always process per cycle
            if (packetqueue.size() > 0)
            {
                std::list<request> packetqueue2;
                {
                    std::lock_guard<std::mutex> l(m);
                    packetqueue2 = packetqueue;
                    packetqueue.clear();
                }
                for (request & req_ : packetqueue2)
                {
                    try
                    {
                        request_handler_.handle_request(*this, req_);
                    }
                    SQLCATCH3(0, spitfire::GetSingleton());
                }
            }
            uint64_t t2 = Utils::time();
            if (t2 - t1 > 50)
            {
                //consoleLogger->information("Slow packet queue: " + Poco::NumberFormatter::format(t2-t1) + "ms");
                log->error("Slow packet queue: %Lums", t2 - t1);
            }
            std::this_thread::sleep_for(1ms);
        }
        catch (...)
        {
            log->error("uncaught TimeThread() exception");
            TimerThreadRunning = false;
        }
    }
    TimerThreadRunning = false;
    log->info("Timer thread exited.");
    return;
}

stItemConfig * spitfire::GetItem(std::string name)
{
    for (int i = 0; i < DEF_MAXITEMS; ++i)
    {
        if (items[i].name == name)
            return &items[i];
    }
    return 0;
}

double spitfire::GetPrestigeOfAction(int8_t action, int8_t id, int8_t level, int8_t thlevel) const
{
    double prestige = 0;
    switch (action)
    {
        case DEF_RESEARCH:
            switch (id)
            {
                case T_AGRICULTURE:
                    prestige = 26 * 2;
                    break;
                case T_LUMBERING:
                    prestige = 31 * 2;
                    break;
                case T_MASONRY:
                    prestige = 41 * 2;
                    break;
                case T_MINING:
                    prestige = 55 * 2;
                    break;
                case T_METALCASTING:
                    prestige = 57 * 2;//not final
                    break;
                case T_INFORMATICS:
                    prestige = 59 * 2;//not final
                    break;
                case T_MILITARYSCIENCE:
                    prestige = 61 * 2;
                    break;
                case T_MILITARYTRADITION:
                    prestige = 78 * 2;
                    break;
                case T_IRONWORKING:
                    prestige = 26 * 2;//not final
                    break;
                case T_LOGISTICS:
                    prestige = 26 * 2;//not final
                    break;
                case T_COMPASS:
                    prestige = 26 * 2;//not final
                    break;
                case T_HORSEBACKRIDING:
                    prestige = 26 * 2;//not final
                    break;
                case T_ARCHERY:
                    prestige = 26 * 2;//not final
                    break;
                case T_STOCKPILE:
                    prestige = 26 * 2;//not final
                    break;
                case T_MEDICINE:
                    prestige = 26 * 2;//not final
                    break;
                case T_CONSTRUCTION:
                    prestige = 26 * 2;//not final
                    break;
                case T_ENGINEERING:
                    prestige = 26 * 2;//not final
                    break;
                case T_MACHINERY:
                    prestige = 26 * 2;//not final
                    break;
                case T_PRIVATEERING:
                    prestige = 26 * 2;//not final
                    break;
            }
            break;
        case DEF_BUILDING:
            switch (id)
            {
                case B_COTTAGE:
                    prestige = 4;
                    break;
                case B_BARRACKS:
                    prestige = 28;
                    break;
                case B_WAREHOUSE:
                    prestige = 21;
                    break;
                case B_SAWMILL:
                    prestige = 7;
                    break;
                case B_STONEMINE:
                    prestige = 9;
                    break;
                case B_IRONMINE:
                    prestige = 11;
                    break;
                case B_FARM:
                    prestige = 5;
                    break;
                case B_STABLE:
                    prestige = 36;
                    break;
                case B_INN:
                    prestige = 26;
                    break;
                case B_FORGE:
                    prestige = 26;
                    break;
                case B_MARKETPLACE:
                    prestige = 32;
                    break;
                case B_RELIEFSTATION:
                    prestige = 26;//not final
                    break;
                case B_ACADEMY:
                    prestige = 30;
                    break;
                case B_WORKSHOP:
                    prestige = 38;
                    break;
                case B_FEASTINGHALL:
                    prestige = 35;
                    break;
                case B_EMBASSY:
                    prestige = 18;
                    break;
                case B_RALLYSPOT:
                    prestige = 25;
                    break;
                case B_BEACONTOWER:
                    prestige = 39;
                    break;
                case B_TOWNHALL:
                    prestige = 112;
                    break;
                case B_WALLS:
                    prestige = 128;
                    break;
            }
            break;
        case DEF_TRAIN:
            switch (id)
            {
                case TR_WORKER:
                    prestige = 0.5;
                    break;
                case TR_WARRIOR:
                    prestige = 0.5;
                    break;
                case TR_SCOUT:
                    prestige = 1.0;//not final
                    break;
                case TR_PIKE:
                    prestige = 2.0;
                    break;
                case TR_SWORDS:
                    prestige = 3;//not final
                    break;
                case TR_ARCHER:
                    prestige = 4;//not final
                    break;
                case TR_TRANSPORTER:
                    prestige = 5;//not final
                    break;
                case TR_CAVALRY:
                    prestige = 6;//not final
                    break;
                case TR_CATAPHRACT:
                    prestige = 7;//not final
                    break;
                case TR_BALLISTA:
                    prestige = 8;//not final
                    break;
                case TR_RAM:
                    prestige = 9;//not final
                    break;
                case TR_CATAPULT:
                    prestige = 10;//not final
                    break;
                case TR_TRAP:
                    prestige = 1;//not final
                    break;
                case TR_ABATIS:
                    prestige = 2;//not final
                    break;
                case TR_ARCHERTOWER:
                    prestige = 3;//not final
                    break;
                case TR_ROLLINGLOG:
                    prestige = 4;//not final
                    break;
                case TR_TREBUCHET:
                    prestige = 5;//not final
                    break;
            }
            break;
    }
    for (int i = 0; i < level; ++i)
        prestige *= 2;
    for (int i = 0; i < thlevel - 1; ++i)
        prestige /= 2;
    return prestige;
}

void spitfire::AddTimedEvent(stTimedEvent & te)
{
    te.id = tecounter++;
    switch (te.type)
    {
        case DEF_TIMEDARMY:
            armylist.push_back(te);
            break;
        case DEF_TIMEDBUILDING:
            buildinglist.push_back(te);
            break;
        case DEF_TIMEDRESEARCH:
            researchlist.push_back(te);
            break;
    }
}

void spitfire::MassDisconnect()
{
    std::list<Client*>::iterator playeriter;

    for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
    {
        Client * client = *playeriter;
        if (client->socket)
        {
            amf3object obj;
            obj["cmd"] = "server.SystemInfoMsg";
            obj["data"] = amf3object();
            amf3object & data = obj["data"];
            data["alliance"] = false;
            data["tV"] = false;
            data["noSenderSystemInfo"] = true;
            data["msg"] = "Server shutting down.";

            SendObject(client->socket, obj);
            SendObject(client->socket, CreateError3("server.ConnectionLost", 4));

            try
            {
                client->socket->stop();
            }
            catch (std::exception & e)
            {
                log->error("ConnectionException: {}", e.what());
            }
            //shutdown(sockets->fdsockets[i].fdsock, 0);
            //closesocket(sockets->fdsockets[i].fdsock);
        }
    }
}

int spitfire::RandomStat() const
{
    int rnd = rand() % 10000;
    if ((rnd >= 0) && (rnd < 6500))
    {
        return rand() % 21 + 15; // 15-35
    }
    else if ((rnd >= 6500) && (rnd < 8500))
    {
        return rand() % 21 + 30; // 30-50
    }
    else if ((rnd >= 8500) && (rnd < 9500))
    {
        return rand() % 16 + 45; // 45-60
    }
    else if ((rnd >= 9500) && (rnd < 9900))
    {
        return rand() % 11 + 60; // 60-70
    }
    else if ((rnd >= 9900) && (rnd < 9950))
    {
        return rand() % 6 + 70; // 70-75
    }
    else if ((rnd >= 9950) && (rnd < 9975))
    {
        return rand() % 6 + 75; // 75-80
    }
    else if ((rnd >= 9975) && (rnd < 10000))
    {
        return rand() % 6 + 80; // 80-85
    }
    return 10;
}

Hero * spitfire::CreateRandomHero(int innlevel) const
{
    Hero * hero = new Hero();

    int maxherolevel = innlevel * 5;

    hero->level = (rand() % maxherolevel) + 1;
    hero->basemanagement = RandomStat();
    hero->basestratagem = RandomStat();
    hero->basepower = RandomStat();

    int remainpoints = hero->level;

    hero->power = rand() % remainpoints;
    remainpoints -= hero->power;
    hero->power += hero->basepower;
    if (remainpoints > 0)
    {
        hero->management = rand() % remainpoints;
        remainpoints -= hero->management;
        hero->management += hero->basemanagement;
    }
    if (remainpoints > 0)
    {
        hero->stratagem = remainpoints;
        remainpoints -= hero->stratagem;
        hero->stratagem += hero->basestratagem;
    }


    hero->loyalty = 70;
    hero->experience = 0;
    hero->upgradeexp = hero->level * hero->level * 100;
    hero->id = 0;
    char tempstr[30];
    sprintf(tempstr, "Test Name%d%d%d", hero->power, hero->management, hero->stratagem);
    hero->name = tempstr;
    hero->logourl = "images/icon/player/faceA20.jpg";

    return hero;
}

bool spitfire::compareprestige(stClientRank first, stClientRank second)
{
    if (first.client->prestige > second.client->prestige)
        return true;
    else
        return false;
}
bool spitfire::comparehonor(stClientRank first, stClientRank second)
{
    if (first.client->honor > second.client->honor)
        return true;
    else
        return false;
}
bool spitfire::comparetitle(stClientRank first, stClientRank second)
{
    if (first.client->title > second.client->title)
        return true;
    else
        return false;
}
bool spitfire::comparepop(stClientRank first, stClientRank second)
{
    if (first.client->population > second.client->population)
        return true;
    else
        return false;
}
bool spitfire::comparecities(stClientRank first, stClientRank second)
{
    if (first.client->citycount > second.client->citycount)
        return true;
    else
        return false;
}

void spitfire::SortPlayers()
{
    prestigerank.clear();
    honorrank.clear();
    titlerank.clear();
    populationrank.clear();
    citiesrank.clear();


    std::list<Client*>::iterator playeriter;
    for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
    {
        Client * client = *playeriter;

        stClientRank rank = stClientRank();
        rank.client = client;
        rank.rank = 0;

        prestigerank.push_back(rank);
        honorrank.push_back(rank);
        titlerank.push_back(rank);
        populationrank.push_back(rank);
        citiesrank.push_back(rank);
    }

    prestigerank.sort(compareprestige);
    honorrank.sort(comparehonor);
    titlerank.sort(comparetitle);
    populationrank.sort(comparepop);
    citiesrank.sort(comparecities);


    int num = 1;
    for (stClientRank & ranklist : prestigerank)
    {
        ranklist.rank = num;
        ranklist.client->prestigerank = num++;
        if (ranklist.client->connected)
            ranklist.client->PlayerUpdate();
    }
    num = 1;
    for (stClientRank & ranklist : honorrank)
    {
        ranklist.rank = num++;
    }
    num = 1;
    for (stClientRank & ranklist : titlerank)
    {
        ranklist.rank = num++;
    }
    num = 1;
    for (stClientRank & ranklist : populationrank)
    {
        ranklist.rank = num++;
    }
    num = 1;
    for (stClientRank & ranklist : citiesrank)
    {
        ranklist.rank = num++;
    }
}

void spitfire::SortHeroes()
{
    herorankstratagem.clear();
    herorankpower.clear();
    herorankmanagement.clear();
    herorankgrade.clear();


    std::list<Client*>::iterator playeriter;
    for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
    {
        Client * client = *playeriter;
        for (uint32_t j = 0; j < client->citylist.size(); ++j)
        {
            if (client->citylist.at(j))
            {
                for (uint32_t k = 0; k < 10; ++k)
                {
                    if (client->citylist[j]->heroes[k])
                    {
                        Hero * hero = client->citylist[j]->heroes[k];
                        stHeroRank rank = stHeroRank();
                        assert(hero->level > 0);
                        assert(hero->stratagem > 0);
                        assert(hero->management > 0);
                        assert(hero->power > 0);
                        rank.grade = hero->level;
                        rank.stratagem = hero->stratagem;
                        rank.management = hero->management;
                        rank.power = hero->power;
                        rank.name = hero->name;
                        rank.kind = client->playername;
                        rank.rank = 0;
                        herorankstratagem.push_back(rank);
                        herorankpower.push_back(rank);
                        herorankmanagement.push_back(rank);
                        herorankgrade.push_back(rank);
                    }
                }
            }
        }
    }

    herorankstratagem.sort(comparestratagem);
    herorankpower.sort(comparepower);
    herorankmanagement.sort(comparemanagement);
    herorankgrade.sort(comparegrade);

    std::list<stHeroRank>::iterator iter;
    int num = 1;
    for (iter = herorankstratagem.begin(); iter != herorankstratagem.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = herorankpower.begin(); iter != herorankpower.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = herorankmanagement.begin(); iter != herorankmanagement.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = herorankgrade.begin(); iter != herorankgrade.end(); ++iter)
    {
        iter->rank = num++;
    }
}

bool spitfire::comparestratagem(stHeroRank first, stHeroRank second)
{
    if (first.stratagem > second.stratagem)
        return true;
    else
        return false;
}
bool spitfire::comparepower(stHeroRank first, stHeroRank second)
{
    if (first.power > second.power)
        return true;
    else
        return false;
}
bool spitfire::comparemanagement(stHeroRank first, stHeroRank second)
{
    if (first.management > second.management)
        return true;
    else
        return false;
}
bool spitfire::comparegrade(stHeroRank first, stHeroRank second)
{
    if (first.grade > second.grade)
        return true;
    else
        return false;
}

void spitfire::SortCastles()
{
    castleranklevel.clear();
    castlerankpopulation.clear();

    std::list<Client*>::iterator playeriter;
    for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
    {
        Client * client = *playeriter;
        for (int j = 0; j < client->citylist.size(); ++j)
        {
            if (client->citylist.at(j))
            {
                PlayerCity * city = client->citylist[j];
                stCastleRank rank = stCastleRank();
                std::stringstream ss;
                std::string grade;
                int16_t level = city->GetBuildingLevel(B_TOWNHALL);
                ss << "Level " << level;
                grade = ss.str();
                if (client->HasAlliance())
                    rank.alliance = client->GetAlliance()->name;
                else
                    rank.alliance = "";
                rank.level = level;
                rank.population = city->population;
                rank.name = city->cityname;
                rank.grade = grade;
                rank.kind = client->playername;
                rank.rank = 0;
                castleranklevel.push_back(rank);
                castlerankpopulation.push_back(rank);
            }
        }
    }

    castleranklevel.sort(comparelevel);
    castlerankpopulation.sort(comparepopulation);

    std::list<stCastleRank>::iterator iter;
    int num = 1;
    for (iter = castleranklevel.begin(); iter != castleranklevel.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = castlerankpopulation.begin(); iter != castlerankpopulation.end(); ++iter)
    {
        iter->rank = num++;
    }
}

bool spitfire::comparepopulation(stCastleRank first, stCastleRank second)
{
    if (first.population > second.population)
        return true;
    else
        return false;
}
bool spitfire::comparelevel(stCastleRank first, stCastleRank second)
{
    if (first.level > second.level)
        return true;
    else
        return false;
}

void * spitfire::DoRankSearch(std::string key, int8_t type, void * subtype, int16_t page, int16_t pagesize)
{
    if (type == 1)//client lists
    {
        std::list<stSearchClientRank>::iterator iter;
        for (iter = searchclientranklist.begin(); iter != searchclientranklist.end();)
        {
            if (iter->rlist == subtype && iter->key == key)
                return &iter->ranklist;
            ++iter;
        }

        stSearchClientRank searchrank = stSearchClientRank();
        searchrank.key = key;
        searchrank.lastaccess = Utils::time();
        searchrank.rlist = (std::list<stClientRank>*)subtype;

        std::list<stClientRank>::iterator iterclient;

        for (iterclient = ((std::list<stClientRank>*)subtype)->begin(); iterclient != ((std::list<stClientRank>*)subtype)->end();)
        {
            if (Utils::ci_find(iterclient->client->playername, key) != std::string::npos)
            {
                stClientRank clientrank;
                clientrank.client = iterclient->client;
                clientrank.rank = iterclient->rank;
                searchrank.ranklist.push_back(clientrank);
            }
            ++iterclient;
        }
        searchclientranklist.push_back(searchrank);
        return &searchclientranklist.back();
    }
    else if (type == 2)//hero lists
    {
        std::list<stSearchHeroRank>::iterator iter;
        for (iter = searchheroranklist.begin(); iter != searchheroranklist.end();)
        {
            if (iter->rlist == subtype && iter->key == key)
                return &iter->ranklist;
            ++iter;
        }

        stSearchHeroRank searchrank = stSearchHeroRank();
        searchrank.key = key;
        searchrank.lastaccess = Utils::time();
        searchrank.rlist = (std::list<stHeroRank>*)subtype;

        std::list<stHeroRank>::iterator iterhero;

        for (iterhero = ((std::list<stHeroRank>*)subtype)->begin(); iterhero != ((std::list<stHeroRank>*)subtype)->end();)
        {
            if (Utils::ci_find(iterhero->name, key) != std::string::npos)
            {
                stHeroRank herorank;
                herorank.grade = iterhero->grade;
                herorank.kind = iterhero->kind;
                herorank.management = iterhero->management;
                herorank.power = iterhero->power;
                herorank.name = iterhero->name;
                herorank.stratagem = iterhero->stratagem;
                herorank.rank = iterhero->rank;
                searchrank.ranklist.push_back(herorank);
            }
            ++iterhero;
        }
        searchheroranklist.push_back(searchrank);
        return &searchheroranklist.back();
    }
    else if (type == 3)//castle lists
    {
        std::list<stSearchCastleRank>::iterator iter;
        for (iter = searchcastleranklist.begin(); iter != searchcastleranklist.end();)
        {
            if (iter->rlist == subtype && iter->key == key)
                return &iter->ranklist;
            ++iter;
        }

        stSearchCastleRank searchrank;
        searchrank.key = key;
        searchrank.lastaccess = Utils::time();
        searchrank.rlist = (std::list<stCastleRank>*)subtype;

        std::list<stCastleRank>::iterator itercastle;

        for (itercastle = ((std::list<stCastleRank>*)subtype)->begin(); itercastle != ((std::list<stCastleRank>*)subtype)->end();)
        {
            if (Utils::ci_find(itercastle->name, key) != std::string::npos)
            {
                stCastleRank castlerank;
                castlerank.grade = itercastle->grade;
                castlerank.alliance = itercastle->alliance;
                castlerank.kind = itercastle->kind;
                castlerank.level = itercastle->level;
                castlerank.name = itercastle->name;
                castlerank.population = itercastle->population;
                castlerank.rank = itercastle->rank;
                searchrank.ranklist.push_back(castlerank);
            }
            ++itercastle;
        }
        searchcastleranklist.push_back(searchrank);
        return &searchcastleranklist.back();
    }
    else// if (type == 4)//alliance lists
    {
        std::list<stSearchAllianceRank>::iterator iter;
        for (iter = searchallianceranklist.begin(); iter != searchallianceranklist.end();)
        {
            if (iter->rlist == subtype && iter->key == key)
                return &iter->ranklist;
            ++iter;
        }

        stSearchAllianceRank searchrank = stSearchAllianceRank();
        searchrank.key = key;
        searchrank.lastaccess = Utils::time();
        searchrank.rlist = (std::list<stAlliance>*)subtype;

        std::list<stAlliance>::iterator iteralliance;

        for (iteralliance = ((std::list<stAlliance>*)subtype)->begin(); iteralliance != ((std::list<stAlliance>*)subtype)->end();)
        {
            if (Utils::ci_find(iteralliance->ref->name, key) != std::string::npos)
            {
                stAlliance alliancerank;
                alliancerank.ref = iteralliance->ref;
                alliancerank.rank = iteralliance->rank;
                searchrank.ranklist.push_back(alliancerank);
            }
            ++iteralliance;
        }
        searchallianceranklist.push_back(searchrank);
        return &searchallianceranklist.back();
    }
    return 0;
}

void spitfire::CheckRankSearchTimeouts(uint64_t time)
{
    std::list<stSearchClientRank>::iterator iterclient;
    std::list<stSearchHeroRank>::iterator iterhero;
    std::list<stSearchCastleRank>::iterator itercastle;

    for (iterclient = searchclientranklist.begin(); iterclient != searchclientranklist.end();)
    {
        if (iterclient->lastaccess + 30000 < time)
        {
            searchclientranklist.erase(iterclient++);
            continue;
        }
        ++iterclient;
    }

    for (iterhero = searchheroranklist.begin(); iterhero != searchheroranklist.end();)
    {
        if (iterhero->lastaccess + 30000 < time)
        {
            searchheroranklist.erase(iterhero++);
            continue;
        }
        ++iterhero;
    }

    for (itercastle = searchcastleranklist.begin(); itercastle != searchcastleranklist.end();)
    {
        if (itercastle->lastaccess + 30000 < time)
        {
            searchcastleranklist.erase(itercastle++);
            continue;
        }
        ++itercastle;
    }
}

bool spitfire::CreateMail(std::string sender, std::string receiver, std::string subject, std::string content, int8_t type)
{
    Client * snd = this->get_client_by_name(sender);
    Client * rcv = this->get_client_by_name(receiver);

    if (((snd == nullptr) && (sender != "System")) || (rcv == nullptr))
    {
        return false;
    }

    //player id of 0 is SYSTEM
    int64_t playerid = (sender == "System") ? 0 : snd->accountid;

    int64_t time = Utils::time();

    try
    {
        stMail mail;
        mail.content = content;
        mail.title = subject;
        mail.type_id = type;
        mail.senttime = time;
        mail.mailid = rcv->mailpid;
        mail.playerid = playerid;

        rcv->maillist.push_back(mail);

        Poco::Data::Session ses(serverpool->get());
        Statement stmt = (ses << "INSERT INTO `mail` (`senderid`, `receiverid`, `title`, `content`, `senttime`, `type`, `pid`) VALUES (?, ?, ?, ?, ?, ?, ?);",
                          use(playerid), use(rcv->accountid), use(subject), use(content), use(time), use(type), use(rcv->mailpid));
        stmt.execute();
        rcv->mailpid++;
        if (!stmt.done())
        {
            log->error("Unable to send mail.");
            return false;
        }
    }
    SQLCATCH(return false;);

    if (sender == "System")
    {
        return true;
    }

    try
    {
        stMail mail;
        mail.content = content;
        mail.title = subject;
        mail.type_id = MAIL_SENT;
        mail.senttime = time;
        mail.mailid = snd->mailpid;
        mail.playerid = playerid;
        mail.readtime = time;

        rcv->maillist.push_back(mail);

        Poco::Data::Session ses(serverpool->get());
        Statement stmt = (ses << "INSERT INTO `mail` (`senderid`, `receiverid`, `title`, `content`, `senttime`, `readtime`, `type`, `pid`) VALUES (?, ?, ?, ?, ?, ?, ?, ?);",
                          use(playerid), use(rcv->accountid), use(subject), use(content), use(time), use(time), use(type), use(snd->mailpid));
        stmt.execute();
        snd->mailpid++;
        if (!stmt.done())
        {
            log->error("Unable to send mail.");
            return false;
        }
    }
    SQLCATCH(return false;);



    //rcv

    return true;
}

bool spitfire::initialize()
{
    try
    {
        std::ifstream config("config.json");

        json obj;

        config >> obj;


        log->info("Loading config.json");

        bindaddress = obj["bindaddress"];
        log->info(fmt::format("bindaddress: {}", bindaddress));

        bindport = obj["bindport"];
        log->info(fmt::format("bindport: {}", bindport));

        maxplayers = obj["maxplayers"];
        log->info(fmt::format("maxplayers: {}", maxplayers));

        mapsize = obj["mapsize"];
        log->info(fmt::format("mapsize: {}", mapsize));

        sqlhost = obj["sqlhost"];
        log->info(fmt::format("sqlhost: {}", sqlhost));

        sqluser = obj["sqluser"];
        log->info(fmt::format("sqluser: {}", sqluser));

        sqlpass = obj["sqlpass"];
        log->info("sqlpass set");

        dbmaintable = obj["dbmaintable"];
        log->info(fmt::format("dbmaintable: {}", dbmaintable));

        dbservertable = obj["dbservertable"];
        log->info(fmt::format("dbservertable: {}", dbservertable));

        servername = obj["servername"];
        log->info(fmt::format("servername: {}", servername));
    }
    catch (std::exception& e)
    {
        log->error("Init() Exception: {}", (std::string)e.what());
        return false;
    }
    catch (...)
    {
        log->error("Unspecified Init() Exception.");
        return false;
    }

    try
    {
        accountpool = new SessionPool("MySQL", "host=" + sqlhost + ";port=3306;db=" + dbmaintable + ";user=" + sqluser + ";password=" + sqlpass + ";compress=true;auto-reconnect=true");
        serverpool = new SessionPool("MySQL", "host=" + sqlhost + ";port=3306;db=" + dbservertable + ";user=" + sqluser + ";password=" + sqlpass + ";compress=true;auto-reconnect=true");
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << exc.displayText() << std::endl;
        return false;
    }


//#ifdef WIN32
//     {
//         asio::ip::tcp::resolver resolver(io_service_);
//         asio::ip::tcp::endpoint endpoint = *resolver.resolve({ bindaddress, std::string("843") });
//         acceptorpolicy_.open(endpoint.protocol());
//         acceptorpolicy_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
//         bool test = true;
//         try
//         {
//             acceptorpolicy_.bind(endpoint);
//         }
//         catch (std::exception& e)
//         {
//             test = false;
//         }
//         if (test == false)
//         {
//             printf("Invalid bind address or port 843 already in use!\n");
//         }
//         else
//         {
//             // Finally listen on the socket and start accepting connections
//             acceptorpolicy_.listen();
//             do_acceptpolicy();
//         }
//     }
    //#endif

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    asio::ip::tcp::resolver resolver(io_service_);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve({ bindaddress, bindport });
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    bool test = true;
    try
    {
        acceptor_.bind(endpoint);
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
    if (test == false)
    {
        throw std::runtime_error("Invalid bind address or port 443 already in use! Exiting.");
    }

    // Finally listen on the socket and start accepting connections
    acceptor_.listen();
    do_accept();

    return true;
}

void spitfire::start_logging()
{
    spdlog::set_async_mode(32);
    log = spdlog::stdout_color_mt("spitfire");
    log->set_level(spdlog::level::level_enum::trace);
}
