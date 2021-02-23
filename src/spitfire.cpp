/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/


#include "spitfire.h"

#include <fstream>
#include <thread>

// #include <boost/smart_ptr/shared_ptr.hpp>
// #include <boost/smart_ptr/make_shared_object.hpp>
// #include <boost/log/attributes.hpp>
// #include <boost/log/attributes/scoped_attribute.hpp>
// #include <boost/log/attributes/named_scope.hpp>
// #include <boost/log/sinks/unbounded_ordering_queue.hpp>
// #include <boost/log/sinks/text_file_backend.hpp>
// #include <boost/log/utility/setup/common_attributes.hpp>
// #include <boost/log/utility/record_ordering.hpp>
// #include <boost/log/expressions.hpp>
// #include <boost/log/sinks/sync_frontend.hpp>
// #include <boost/log/sinks/async_frontend.hpp>
// #include <boost/log/sinks/text_ostream_backend.hpp>
// #include <boost/log/sources/record_ostream.hpp>
// #include <boost/log/keywords/file_name.hpp>
// #include <boost/log/keywords/rotation_size.hpp>
// #include <boost/log/keywords/time_based_rotation.hpp>
// #include <boost/log/keywords/target.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
// #include <boost/date_time/posix_time/posix_time_types.hpp>
// #include <boost/log/support/date_time.hpp>
// #include <boost/core/null_deleter.hpp>
// 
// #include <boost/iostreams/filtering_streambuf.hpp>
// #include <boost/iostreams/copy.hpp>
// #include <boost/iostreams/filter/zlib.hpp>
// #include <boost/iostreams/device/back_inserter.hpp>

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
#include "combatsimulator.h"
#include "Valley.h"
#include "xml_writer.hpp"

#define DEF_NOMAPDATA

using namespace std::chrono_literals;

spitfire * spitfire::_instance = nullptr;

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
    , currentplayersonline(0)
    , map(nullptr)
{
    setupLogging();
    log->info("EPS Server starting.");
 
    MySQL::Connector::registerConnector();

    accountpool = nullptr;
    serverpool = nullptr;

    SaveThreadRunning = false;
    serverstatus = SERVERSTATUS_STOPPED;//offline
    servername = "";

    mapsize = 800;
    m_heroid = 1000;
    m_cityid = 100000;
    m_allianceid = 100;
    m_itemcount = 0;

    armycounter = 0;
    tecounter = 0;
    currentplayersonline = 0;
    ltime = 0;
//     m_alliances = nullptr;

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
    if (map)
        delete map;
    delete accountpool;
    delete serverpool;
    MySQL::Connector::unregisterConnector();
}

int64_t spitfire::testMemory[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

void spitfire::io_thread()
{
    srand(static_cast<int32_t>(Utils::time()));
    io_service_.run();
}

void spitfire::run()
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
    //     this->m_itemxml = itemxmlbuff;
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







    map = new Map(mapsize);
    m_alliances = new AllianceMgr();

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
                m_buildingconfig[cfgid][level].time = rs.value("buildtime").convert<double>();
                m_buildingconfig[cfgid][level].destructtime = rs.value("buildtime").convert<double>() / 2;
                m_buildingconfig[cfgid][level].food = rs.value("food").convert<int32_t>();
                m_buildingconfig[cfgid][level].wood = rs.value("wood").convert<int32_t>();
                m_buildingconfig[cfgid][level].stone = rs.value("stone").convert<int32_t>();
                m_buildingconfig[cfgid][level].iron = rs.value("iron").convert<int32_t>();
                m_buildingconfig[cfgid][level].gold = rs.value("gold").convert<int32_t>();
                m_buildingconfig[cfgid][level].prestige = rs.value("gold").convert<int32_t>();

                std::string strt = rs.value("prereqbuilding").convert<std::string>();
                char * str = (char*)strt.c_str();

                int x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    m_buildingconfig[cfgid][level].buildings.push_back(prereq);
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
                    m_buildingconfig[cfgid][level].techs.push_back(prereq);
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
                    m_buildingconfig[cfgid][level].items.push_back(prereq);
                    x++;
                }

                m_buildingconfig[cfgid][level].limit = rs.value("blimit").convert<int32_t>();
                m_buildingconfig[cfgid][level].inside = rs.value("inside").convert<int32_t>();
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
                m_troopconfig[cfgid].time = rs.value("buildtime").convert<double>();
                m_troopconfig[cfgid].destructtime = 0.0;
                m_troopconfig[cfgid].food = rs.value("food").convert<int32_t>();
                m_troopconfig[cfgid].wood = rs.value("wood").convert<int32_t>();
                m_troopconfig[cfgid].stone = rs.value("stone").convert<int32_t>();
                m_troopconfig[cfgid].iron = rs.value("iron").convert<int32_t>();
                m_troopconfig[cfgid].gold = rs.value("gold").convert<int32_t>();
                m_troopconfig[cfgid].inside = rs.value("inside").convert<int32_t>();
                m_troopconfig[cfgid].population = rs.value("population").convert<int32_t>();

                std::string strt = rs.value("prereqbuilding").convert<std::string>();
                char * str = (char*)strt.c_str();
                int x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    m_troopconfig[cfgid].buildings.push_back(prereq);
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
                    m_troopconfig[cfgid].techs.push_back(prereq);
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
                    m_troopconfig[cfgid].items.push_back(prereq);
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
                m_researchconfig[cfgid][level].time = rs.value("buildtime").convert<int32_t>();
                m_researchconfig[cfgid][level].destructtime = rs.value("buildtime").convert<int32_t>() / 2;
                m_researchconfig[cfgid][level].food = rs.value("food").convert<int32_t>();
                m_researchconfig[cfgid][level].wood = rs.value("wood").convert<int32_t>();
                m_researchconfig[cfgid][level].stone = rs.value("stone").convert<int32_t>();
                m_researchconfig[cfgid][level].iron = rs.value("iron").convert<int32_t>();
                m_researchconfig[cfgid][level].gold = rs.value("gold").convert<int32_t>();

                std::string strt = rs.value("prereqbuilding").convert<std::string>();
                char * str = (char*)strt.c_str();

                int x = 0;

                for (tok = strtok_s(str, "|", &ch); tok != 0; tok = strtok_s(0, "|", &ch))
                {
                    stPrereq prereq;
                    tok = strtok_s(tok, ",", &cr); assert(tok != 0); prereq.id = atoi(tok);
                    tok = strtok_s(0, ",", &cr); assert(tok != 0); prereq.level = atoi(tok);
                    m_researchconfig[cfgid][level].buildings.push_back(prereq);
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
                    m_researchconfig[cfgid][level].techs.push_back(prereq);
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
                    m_researchconfig[cfgid][level].items.push_back(prereq);
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
                m_items[id].name = rs.value("name").convert<std::string>();
                m_items[id].desc = rs.value("description").convert<std::string>();
                m_items[id].cost = rs.value("cost").convert<int32_t>();
                m_items[id].saleprice = rs.value("cost").convert<int32_t>();
                m_items[id].buyable = rs.value("buyable").convert<bool>();
                m_items[id].cangamble = rs.value("cangamble").convert<bool>();
                m_items[id].daylimit = rs.value("daylimit").convert<int32_t>();
                m_items[id].type = rs.value("itemtype").convert<int32_t>();
                m_items[id].rarity = rs.value("rarity").convert<int32_t>();
                ++m_itemcount;

                if (m_items[id].cangamble)
                {
                    switch (m_items[id].rarity)
                    {
                        default:
                        case 5:
                            m_gambleitems.common.push_back(&m_items[id]);
                            break;
                        case 4:
                            m_gambleitems.special.push_back(&m_items[id]);
                            break;
                        case 3:
                            m_gambleitems.rare.push_back(&m_items[id]);
                            break;
                        case 2:
                            m_gambleitems.superrare.push_back(&m_items[id]);
                            break;
                        case 1:
                            m_gambleitems.ultrarare.push_back(&m_items[id]);
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

            map->m_tile[id].m_id = id;
            map->m_tile[id].m_ownerid = ownerid;
            map->m_tile[id].m_type = type;
            map->m_tile[id].m_level = level;

            if (type == NPC)
            {
                NpcCity * city = (NpcCity *)AddNpcCity(id);
                city->Initialize(true, true);
                city->m_level = level;
                city->m_ownerid = ownerid;
                map->m_tile[id].m_zoneid = map->GetStateFromID(id);
            }

            if ((id + 1) % ((mapsize*mapsize) / 100) == 0)
            {
                log->info(fmt::format("{}%", int((double(double(id + 1) / (mapsize*mapsize)))*double(100))));
            }
        } while (rs.moveNext());
    }
#else
    //this fakes map data
    int32_t maparea=mapsize*mapsize;
    for (int x = 0; x < maparea; x += 1/*(mapsize*mapsize)/10*/)
    {
        map->m_tile[x].m_id = x;
        map->m_tile[x].m_ownerid = 0;
        //make every tile an npc
        //m_map->m_tile[x].m_type = NPC;
        map->m_tile[x].m_type = rand() % 8 + 1;
        map->m_tile[x].m_level = (rand() % 10) + 1;

        if (map->m_tile[x].m_type == 7)
            map->m_tile[x].m_type = 10;

        if (map->m_tile[x].m_type == 8)
        {
            map->m_tile[x].m_type = NPC;
            NpcCity * city = (NpcCity *)AddNpcCity(map->m_tile[x].m_id);
            city->m_level = map->m_tile[x].m_level;
            city->Initialize(true, true);
            city->m_ownerid = map->m_tile[x].m_ownerid;
            map->m_tile[map->m_tile[x].m_id].m_zoneid = map->GetStateFromID(map->m_tile[x].m_id);
        }

        if ((x + 1) % (maparea / 100) == 0)
        {
            log->info("{}%", int((double(double(x + 1) / maparea))*double(100)));
        }
    }
#endif

    map->CalculateOpenTiles();

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

            Client * client = NewClient();
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
            auto * client = GetClient(accountid);
            GETXYFROMID(fieldid);
            if (client == nullptr)
            {
                log->error("City exists with no account attached. - accountid:{} cityid:{} coord:({},{})", accountid, cityid, xfromid, yfromid);
                continue;
            }
            auto * city = (PlayerCity *)AddPlayerCity(client, fieldid, cityid);
            city->m_client = client;
            city->m_resources.food = rs.value("food").convert<double>();
            city->m_resources.wood = rs.value("wood").convert<double>();
            city->m_resources.iron = rs.value("iron").convert<double>();
            city->m_resources.stone = rs.value("stone").convert<double>();
            city->m_resources.gold = rs.value("gold").convert<double>();
            city->m_cityname = rs.value("name").convert<std::string>();
            city->m_logurl = rs.value("logurl").convert<std::string>();
            city->m_tileid = fieldid;
            //        city->m_accountid = accountid;
            //        city->m_client = client;
            city->m_creation = rs.value("creation").convert<double>();

            //        client->m_citycount++;

            //         server->m_map->m_tile[fieldid].m_city = city;
            //         server->m_map->m_tile[fieldid].m_npc = false;
            //         server->m_map->m_tile[fieldid].m_ownerid = accountid;
            //         server->m_map->m_tile[fieldid].m_type = CASTLE;
            // 
            //         //server->m_map->m_tile[fieldid].m_zoneid = server->m_map->GetStateFromID(fieldid);
            // 
            //         server->m_map->m_tile[fieldid].m_castleid = cityid;


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
                select2 << "SELECT * FROM `heroes` WHERE `castleid`=?", use(city->m_castleid);
                select2.execute();
                RecordSet rs2(select2);

                rs2.moveFirst();

                for (int a = 0; a < rs2.rowCount(); ++a, rs2.moveNext())
                {
                    Hero * temphero;
                    temphero = new Hero();
                    temphero->m_id = rs2.value("id").convert<uint64_t>();
                    temphero->m_status = rs2.value("status").convert<int8_t>();
                    temphero->m_itemid = rs2.value("itemid").convert<int32_t>();
                    temphero->m_itemamount = rs2.value("itemamount").convert<int32_t>();
                    temphero->m_castleid = cityid;
                    temphero->m_ownerid = accountid;

                    temphero->m_basestratagem = rs2.value("basestratagem").convert<uint32_t>();
                    temphero->m_stratagem = rs2.value("stratagem").convert<uint32_t>();
                    temphero->m_stratagemadded = rs2.value("stratagemadded").convert<uint32_t>();
                    temphero->m_stratagembuffadded = rs2.value("stratagembuffadded").convert<uint32_t>();
                    temphero->m_basepower = rs2.value("basepower").convert<uint32_t>();
                    temphero->m_power = rs2.value("power").convert<uint32_t>();
                    temphero->m_poweradded = rs2.value("poweradded").convert<uint32_t>();
                    temphero->m_powerbuffadded = rs2.value("powerbuffadded").convert<uint32_t>();
                    temphero->m_basemanagement = rs2.value("basemanagement").convert<uint32_t>();
                    temphero->m_management = rs2.value("management").convert<uint32_t>();
                    temphero->m_managementadded = rs2.value("managementadded").convert<uint32_t>();
                    temphero->m_managementbuffadded = rs2.value("managementbuffadded").convert<uint32_t>();
                    temphero->m_logourl = rs2.value("logurl").convert<std::string>();
                    temphero->m_name = rs2.value("name").convert<std::string>();
                    temphero->m_remainpoint = rs2.value("remainpoint").convert<uint32_t>();
                    temphero->m_level = rs2.value("level").convert<uint32_t>();
                    temphero->m_upgradeexp = rs2.value("upgradeexp").convert<double>();
                    temphero->m_experience = rs2.value("experience").convert<double>();
                    temphero->m_loyalty = rs2.value("loyalty").convert<int8_t>();
                    if (temphero->m_loyalty>100) temphero->m_loyalty=100;
                    city->m_heroes[a] = temphero;
                    city->m_heroes[a]->m_client = client;
                    if (temphero->m_status == DEF_HEROMAYOR && city->m_mayor)
                    {
                        temphero->m_status = DEF_HEROIDLE;
                    }
                    if (!city->m_mayor && temphero->m_status == DEF_HEROMAYOR)
                    {
                        city->m_mayor = temphero;
                    }
                    if (temphero->m_id >= m_heroid)
                        m_heroid = temphero->m_id + 1;
                }
            }

            if (cityid >= m_cityid)
                m_cityid = cityid + 1;


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
                        pcity->m_researching = true;

                        if (client->research[a].castleid != 0)
                        {
                            stResearchAction * ra = new stResearchAction;

                            stTimedEvent te;
                            ra->city = pcity;
                            ra->client = pcity->m_client;
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
            alliance = m_alliances->CreateAlliance(rs.value("name").convert<std::string>(), rs.value("founder").convert<std::string>(), rs.value("id").convert<int64_t>(), false);
            if (alliance == nullptr)
                throw("Unable to create alliance : " + rs.value("name").convert<std::string>() + " ID : " + rs.value("id").convert<std::string>());
            //alliance->m_allianceid = msql->GetInt(i, "id");
            alliance->ParseMembers(rs.value("members").convert<std::string>());
            for (Alliance::stMember & member : alliance->m_members)
            {
                if (member.rank == DEF_ALLIANCEHOST)
                {
                    alliance->m_ownerid = member.clientid;
                    alliance->m_owner = GetClient(member.clientid)->playername;
                    break;
                }
            }
            alliance->ParseRelation(&alliance->m_enemies, rs.value("enemies").convert<std::string>());
            alliance->ParseRelation(&alliance->m_allies, rs.value("allies").convert<std::string>());
            alliance->ParseRelation(&alliance->m_neutral, rs.value("neutrals").convert<std::string>());
            alliance->m_name = rs.value("name").convert<std::string>();
            alliance->m_founder = rs.value("founder").convert<std::string>();
            alliance->m_note = rs.value("note").convert<std::string>();

            if (alliance->m_allianceid >= m_allianceid)
                m_allianceid = alliance->m_allianceid + 1;


            if (alliancecount > 101)
                if ((count) % ((alliancecount) / 100) == 0)
                {
                    log->info("%d%", int((double(double(count) / alliancecount + 1))*double(100)));
                }
        }
    }

    log->info("Loading army data.");
    {
        Poco::Data::Session ses2(serverpool->get());
        Statement select(ses2);
        select << "SELECT * FROM `armies`";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();
        std::vector<std::string> vec;
        for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext())
        {
            stArmyMovement* x=new stArmyMovement;
            Client* l=GetClient(rs.value("clientid").convert<std::int32_t>());
            if (l==0) {
                delete x;
                continue;
            }
            x->client=l;
            PlayerCity* city=l->GetCity(rs.value("cityid").convert<int64_t>());
            if (city==0) {
                delete x;
                continue;
            }
            int64_t heroid=rs.value("heroid").convert<int64_t>();
            Hero* hero = nullptr;
            if (heroid>0) {
                hero=city->GetHero(heroid);
                if (hero==0) {
                    delete x;
                    continue;
                }
            }
            x->hero=hero;
            x->city=city;
            x->targetfieldid=rs.value("targetfieldid").convert<int32_t>();
            x->direction=rs.value("direction").convert<int16_t>();
            std::string resourcestring = rs.value("resource").convert<std::string>();
            if (resourcestring.length() > 0) {
                my_split(vec, resourcestring, ",");
                x->resources.food = atol(vec[0].c_str());
                x->resources.wood = atol(vec[1].c_str());
                x->resources.stone = atol(vec[2].c_str());
                x->resources.iron = atol(vec[3].c_str());
                x->resources.gold = atol(vec[4].c_str());
            }
            std::string troopstring = rs.value("troops").convert<std::string>();
            if (troopstring.length() > 0) {
                my_split(vec, troopstring, ",");
                x->troops.worker = atol(vec[0].c_str());
                x->troops.warrior = atol(vec[1].c_str());
                x->troops.scout = atol(vec[2].c_str());
                x->troops.pike = atol(vec[3].c_str());
                x->troops.sword = atol(vec[4].c_str());
                x->troops.archer = atol(vec[5].c_str());
                x->troops.transporter = atol(vec[6].c_str());
                x->troops.cavalry = atol(vec[7].c_str());
                x->troops.cataphract = atol(vec[8].c_str());
                x->troops.ballista = atol(vec[9].c_str());
                x->troops.ram = atol(vec[10].c_str());
                x->troops.catapult = atol(vec[11].c_str());
            }
            x->starttime = rs.value("starttime").convert<int64_t>();
            x->reachtime = rs.value("reachtime").convert<int64_t>();
            x->resttime = rs.value("resttime").convert<int64_t>();
            x->missiontype = rs.value("missiontype").convert<int32_t>();
            x->startfieldid = rs.value("startfieldid").convert<int32_t>();
            if (x->hero != 0) {
                x->herolevel = x->hero->m_level;
                x->heroname = x->hero->m_name;
            }
            x->startposname = city->m_cityname;
            x->targetposname= map->GetTileFromID(x->targetfieldid)->GetName();
            x->armyid = armycounter++;
            stTimedEvent tl;
            tl.type = DEF_TIMEDARMY;
            tl.data = x;
            armylist.push_back(tl);
            x->client->armymovement.push_back(x);
            Tile* til = map->GetTileFromID(x->targetfieldid);
            if (til->m_ownerid > 0) {
                Client* ml = GetClient(til->m_ownerid);
                if (ml != x->client && ml!=0) {
                    int16_t relation = m_alliances->GetRelation(ml->accountid, x->client->accountid);
                    if (relation == DEF_ALLIANCE || relation == DEF_ALLY) {
                        ml->friendarmymovement.push_back(x);
                    }
                }
            }
        }
        armylist.sort(comparearmies);
    }

    log->info("Loading Report data.");

    {
        Poco::Data::Session ses(serverpool->get());
        Statement select(ses);
        select << "SELECT * FROM `reports`;";
        select.execute();
        RecordSet rs(select);

        rs.moveFirst();
        for (int i = 0; i < rs.rowCount(); ++i, rs.moveNext()) {
            int64_t clientid = rs.value("accountid").convert<int64_t>();
            Client * client = GetClient(clientid);
            if (client == 0) continue;
            stReport r;
            r.armytype = rs.value("armytype").convert<int8_t>();
            r.back = rs.value("back").convert<bool>();
            r.attack = rs.value("attack").convert<bool>();
            r.type_id = rs.value("typeid").convert<int8_t>();
            r.startpos = rs.value("startpos").convert<std::string>();
            r.targetpos = rs.value("targetpos").convert<std::string>();
            r.title = rs.value("title").convert<std::string>();
            r.guid = rs.value("guid").convert<std::string>();
            r.eventtime = rs.value("eventtime").convert<uint64_t>();
            r.isread = rs.value("isread").convert<bool>();
            r.reportid = client->currentreportid++;
            client->reportlist.push_back(r);
        }
    }
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
    alliance = m_alliances->CreateAlliance(rs.value("name").convert<string>(), rs.value("leader").convert<int64_t>(), rs.value("id").convert<int64_t>());
    //alliance->m_allianceid = msql->GetInt(i, "id");
    alliance->ParseMembers(rs.value("members").convert<string>());
    alliance->ParseRelation(&alliance->m_enemies, rs.value("enemies").convert<string>());
    alliance->ParseRelation(&alliance->m_allies, rs.value("allies").convert<string>());
    alliance->ParseRelation(&alliance->m_neutral, rs.value("neutrals").convert<string>());
    alliance->m_name = rs.value("name").convert<string>();
    alliance->m_founder = rs.value("founder").convert<string>();
    alliance->m_note = rs.value("note").convert<string>();

    if (alliance->m_allianceid >= m_allianceid)
    m_allianceid = alliance->m_allianceid + 1;


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


//     for (Alliance * alliance : m_alliances->m_alliances)
//     {
//         if (alliance == nullptr)
//             continue;
//         std::list<Alliance::stMember> tempmembers = alliance->m_members;
//         for (Alliance::stMember & member : tempmembers)
//         {
//             Client * client = GetClient(member.clientid);
//             if (client->allianceid != alliance->m_allianceid)
//             {
//                 client->allianceid = -1;
//                 client->alliancerank = 0;
//                 client->alliancename = "";
//                 alliance->m_members.remove(member);
//                 alliance->m_currentmembers--;
//                 continue;
//             }
//             if (client->alliancerank != member.rank)
//             {
//                 client->alliancerank = member.rank;
//             }
//         }
//     }


#ifndef DEF_NOMAPDATA

    log->info("Incrementing valleys.");


    for (int i = 0; i < mapsize*mapsize; ++i)
    {
        if (map->m_tile[i].m_type < 11 && map->m_tile[i].m_ownerid == 0)
        {
            map->m_tile[i].m_level++;
            if (map->m_tile[i].m_level > 10)
                map->m_tile[i].m_level = 1;
        }
    }

#endif
#pragma endregion

    SortPlayers();
    SortHeroes();
    SortCastles();
    //m_alliances->SortAlliances();

    TimerThreadRunning = true;
    std::thread timerthread(std::bind(std::mem_fun(&spitfire::TimerThread), this));


    //SOCKET THREADS

    // Create a pool of threads to run all of the io_services.
    std::vector<std::shared_ptr<std::thread> > threads;
    for (std::size_t i = 0; i < /*thread_pool_size_*/1; ++i)
    {
        std::shared_ptr<std::thread> thread(new std::thread(
            std::bind(&spitfire::io_thread, this)));
        threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();
    //io_service_.run();

    timerthread.join();

    // update DB on exit
    log->info("Updating database before exiting.");

    log->info("Updating players database.");
    for (Client* xl:players) {
        if (xl) xl->SaveToDB();
    }

    log->info("Updating cities database.");
    for (Client* xl : players) {
        if (xl) {
            for (PlayerCity* pl : xl->citylist) {
                if (pl) pl->SaveToDB();
            }
        }
    }

    log->info("Updating heroes database.");
    for (Client* xl : players) {
        if (xl) {
            for (PlayerCity* pl : xl->citylist) {
                if (pl) {
                    for (Hero* hl : pl->m_heroes) {
                        if (hl) hl->SaveToDB();
                    }
                }
            }
        }
    }

    log->info("Updating armies database.");
    {
        Poco::Data::Session ses2(serverpool->get());
        ses2 << "TRUNCATE TABLE `armies`", now;        
        
        using ArmyData = Poco::Tuple<int64_t, int8_t, std::string, std::string, int64_t, int64_t, int64_t, int8_t, int32_t, int32_t, int64_t, int32_t>;
        std::vector<Poco::Any> vec;
        for (stTimedEvent& evt : armylist)
        {
            stArmyMovement* x=(stArmyMovement*)evt.data;
            vec.clear();
            vec.emplace_back((int64_t)x->resources.food);
            vec.emplace_back((int64_t)x->resources.wood);
            vec.emplace_back((int64_t)x->resources.stone);
            vec.emplace_back((int64_t)x->resources.iron);
            vec.emplace_back((int64_t)x->resources.gold);
            std::string resourcestring;
            Poco::format(resourcestring,"%?d,%?d,%?d,%?d,%?d", vec);
            vec.clear();
            vec.emplace_back(x->troops.worker);
            vec.emplace_back(x->troops.warrior);
            vec.emplace_back(x->troops.scout);
            vec.emplace_back(x->troops.pike);
            vec.emplace_back(x->troops.sword);
            vec.emplace_back(x->troops.archer);
            vec.emplace_back(x->troops.transporter);
            vec.emplace_back(x->troops.cavalry);
            vec.emplace_back(x->troops.cataphract);
            vec.emplace_back(x->troops.ballista);
            vec.emplace_back(x->troops.ram);
            vec.emplace_back(x->troops.catapult);
            std::string troopstring;
            Poco::format(troopstring,"%?d,%?d,%?d,%?d,%?d,%?d,%?d,%?d,%?d,%?d,%?d,%?d", vec);
            int64_t heroid = -1;
            if (x->hero != 0) heroid = x->hero->m_id;
            ArmyData save(heroid, x->direction, resourcestring, troopstring, x->starttime, x->reachtime, x->resttime, x->missiontype, x->startfieldid, x->targetfieldid, ((PlayerCity*)x->city)->m_castleid, x->client->accountid);
            ses2 << "INSERT INTO `armies` (`heroid`, `direction`, `resource`, `troops`, `starttime`, `reachtime`, `resttime`, `missiontype`, `startfieldid`, `targetfieldid`, `cityid`, `clientid`) VALUES( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", use(save), now;
        }
    }

    log->info("Updating reports database.");
    {
        Poco::Data::Session ses(serverpool->get());
        ses << "TRUNCATE TABLE `reports`;", now;
        using ReportData = Poco::Tuple<int64_t, int8_t, bool, bool, int8_t, std::string, std::string, std::string, std::string, uint64_t, bool>;
        for (Client* client : players) {
            if (client == 0) continue;
            for (stReport& r : client->reportlist) {
                ReportData report(client->accountid, r.armytype, r.back, r.attack, r.type_id, r.startpos, r.targetpos, r.title, r.guid, r.eventtime, r.isread);
                ses << "INSERT INTO `reports` (`accountid`, `armytype`, `back`, `attack`, `typeid`, `startpos`, `targetpos`, `title`, `guid`, `eventtime`, `isread`) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", use(report), now;
            }
        }
    }
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

void spitfire::startpolicy(connection_ptr c)
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
                startpolicy(std::make_shared<connection>(
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

bool spitfire::ConnectSQL()
{
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
    return true;
}

bool spitfire::InitSockets()
{
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

Client * spitfire::NewClient()
{
    static uint32_t clientnum;
    if (currentplayersonline < maxplayers)
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
int32_t  spitfire::GetClientIndex(int64_t accountid)
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
Client * spitfire::GetClientByCastle(int64_t castleid)
{
//     for (Client * client : players)
//     {
//         if (client->GetCity(castleid) != 0)
//             return client;
//     }
    return 0;
}
Client * spitfire::GetClient(int64_t accountid)
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
Client * spitfire::GetClientByName(std::string name)
{
    for (Client * client : players)
    {
        if (client->playername == name)
            return client;
    }
    return 0;
}

void spitfire::CloseClient(Client* client, int typecode, std::string message) const
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
    city->m_type = CASTLE;
    city->m_logurl = "";
    city->m_status = 0;
    city->m_tileid = tileid;
    city->m_castleid = castleid;
    city->m_accountid = client->accountid;

    city->SetBuilding(31, 1, -1, 0, 0, 0);
    city->SetResources(0, 0, 0, 0, 0);
    city->m_client = client;
    std::string temp = "";
    std::stringstream ss;
    ss << "50,10.000000,100.000000,100.000000,100.000000,100.000000,90,0," << (double)Utils::time();
    temp = ss.str();

    city->ParseMisc(temp);

    client->citylist.push_back(city);
    client->citycount++;

    map->m_tile[tileid].m_city = city;
    m_city.push_back(city);


    map->m_tile[tileid].m_npc = false;
    map->m_tile[tileid].m_ownerid = client->accountid;
    map->m_tile[tileid].m_city = city;
    map->m_tile[tileid].m_type = CASTLE;
    map->m_tile[tileid].m_castleid = castleid;

    return city;
}

City * spitfire::AddNpcCity(int tileid)
{
    NpcCity * city = new NpcCity();
    city->m_tileid = tileid;
    city->m_type = NPC;
    city->m_cityname = "Barbarian City";
    city->m_status = 0;
    m_city.push_back(city);
    map->m_tile[tileid].m_city = city;
    map->m_tile[tileid].m_npc = true;
    map->m_tile[tileid].m_type = NPC;
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

    double line = sqrt(pow(abs(fx - tx), 2) + pow(abs(fy - ty), 2));

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
    double compass = city->m_client->GetResearchLevel(T_COMPASS);//1.5x speed max
    double hbr = city->m_client->GetResearchLevel(T_HORSEBACKRIDING);//2x speed max

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
                        CloseClient(*playeriter, 1, "");
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
                client->PlayerInfoUpdate();
            }
            /*            else if (!strcmp(command, "pres"))
            {
            int test = 12786431;
            stringstream ss;
            ss << test << "prestige granted.";
            data["msg"] = ss.str().c_str();
            client->m_prestige += test;
            client->PlayerInfoUpdate();
            }*/
            else if (!strcmp(command, "resources"))
            {
                stResources res = { 10000000, 10000000, 10000000, 10000000, 10000000 };
                client->GetFocusCity()->m_resources += res;
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
                CloseClient(client, typecode, msg);
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
//         return m_alliances->GetRelation(client1, client2);
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
                        for (tqiter = city->m_troopqueue.begin(); tqiter != city->m_troopqueue.end();)
                        {
                            std::list<stTroopTrain>::iterator iter;
                            iter = tqiter->queue.begin();
                            if (iter != tqiter->queue.end() && iter->endtime <= ltime)
                            {
                                //troops done training
                                double gain = iter->count * GetPrestigeOfAction(DEF_TRAIN, iter->troopid, 1, city->m_level);
                                client->Prestige(gain);
                                client->PlayerInfoUpdate();
                                if (city->m_mayor)
                                {
                                    city->m_mayor->m_experience += gain;
                                    city->HeroUpdate(city->m_mayor, 2);
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

                if (armylist.size() > 0)
                {
                    std::list<stTimedEvent> tarmylist = armylist;
                    Client* oclient=nullptr;
                    uint32_t fieldid;
                    std::vector<stArmyMovement*> campers;
                    for (iter = tarmylist.begin(); iter != tarmylist.end(); iter++)
                    {
                        //armylist.erase(iter++);
                        stArmyMovement * am = (stArmyMovement *)iter->data;
                        //if the army is invalid, remove it
                        if (am->client == 0) {
                            delete am;
                            armylist.erase(iter);
                            continue;
                        }
                        if (am->direction == DIRECTION_STAY) continue;
                        PlayerCity * fcity = (PlayerCity *)am->city;
                        Client * fclient = am->client;
                        Hero * fhero = am->hero;
                        fieldid=am->targetfieldid;
                        Tile * tile = map->GetTileFromID(fieldid);
                        if (am->reachtime < ltime)
                        {
                            if (am->direction == DIRECTION_FORWARD)
                            {
                                //check if its still a valid target
                                bool validTarget=true;
                                if (fclient->Beginner() && tile->m_type > 10) validTarget=false;
                                if (tile->m_ownerid > 0) {
                                    oclient=GetClient(tile->m_ownerid);
                                    if (oclient == 0) validTarget=false;
                                    int16_t relation = m_alliances->GetRelation(fclient->accountid, oclient->accountid);
                                    if (relation == DEF_SELFRELATION || relation == DEF_ALLIANCE || relation == DEF_ALLY) validTarget=false;
                                    if (oclient->Beginner() && tile->m_type==CASTLE) validTarget=false;
                                }
                                if (validTarget) {
                                    // scouting mission
                                    if (am->missiontype==MISSION_SCOUT) {
                                        // in case of scouting valleys no scouting battle
                                        if (tile->m_type < CASTLE) {
                                            // this is for saving memory only for now
                                            if (tile->m_valley == nullptr) {
                                                tile->m_valley = new ValleyData;
                                            }
                                            tile->m_valley->Reset(false);
                                            stReport r;
                                            r.guid = Utils::generaterandomstring(28);
                                            r.attack = true;
                                            r.back = false;
                                            r.armytype = MISSION_SCOUT;
                                            r.isread = false;
                                            r.reportid = fclient->currentreportid++;
                                            int xid, yid;
                                            GETXYFROMID4(xid,yid,am->startfieldid,mapsize);
                                            std::stringstream ss;
                                            ss << am->startposname << " (" << xid << "," << yid << ")";
                                            r.startpos = ss.str();
                                            std::stringstream ss1;
                                            GETXYFROMID4(xid,yid,am->targetfieldid,mapsize);
                                            ss1 << am->targetposname << " (" << xid << "," << yid << ")";
                                            r.targetpos = ss1.str();
                                            r.title = "Scout Report";
                                            r.type_id = 1;
                                            r.eventtime = Utils::time();
                                            std::string path = reportbasepath + r.guid+".xml";
                                            std::ofstream file;
                                            file.open(path, std::ios::out);
                                            ValleyData* valley = ((ValleyData*)tile->m_valley);
                                            Writer writer(file);

                                            writer.openElt("reportData").attr("reportUrl", reportbaseurl + r.guid + ".xml");
                                            writer.openElt("scoutReport").attr("isFound", "false").attr("isSuccess", "true").attr("isAttack", "true");
                                            writer.openElt("scoutInfo").attr("heroLevel",std::to_string(valley->m_temphero->m_level)).attr("heroName",valley->m_temphero->m_name).attr("heroUrl","");
                                            writer.openElt("troops");
                                            stTroops st = valley->m_troops;
                                            int64_t* trc = (int64_t*)&st;
                                            for (int ij = 0; ij < 12; ij++) {
                                                if ((*(trc + ij)) > 0) writer.openElt("troopStrType").attr("typeId", std::to_string(ij + 2)).attr("count", std::to_string(*(trc + ij))).closeElt();
                                            }
                                            writer.closeElt();
                                            writer.closeElt();
                                            writer.openElt("battleInfo").attr("isAttack", "true").closeElt();
                                            writer.closeAll();
                                            file.close();
                                            fclient->reportlist.push_front(r);
                                            am->direction=DIRECTION_BACKWARD;
                                            am->reachtime=Utils::time()+am->reachtime-am->starttime-am->resttime;
                                            am->resttime=0;
                                            am->starttime=Utils::time();
                                            if (am->hero!=nullptr) am->hero->m_status=DEF_HERORETURN;
                                            fclient->SelfArmyUpdate();
                                            fclient->ReportUpdate();
                                            continue;
                                        }
                                        // otherwise there will be a scouting battle
                                        else if (tile->m_ownerid > 0) {
                                            oclient=GetClient(tile->m_ownerid);
                                            if (oclient!=0) {
                                                campers.clear();
                                                for (stArmyMovement* pq : oclient->armymovement) {
                                                    if (pq->direction==DIRECTION_STAY && pq->targetfieldid==fieldid) campers.push_back(pq);
                                                }
                                                for (stArmyMovement* pq : oclient->friendarmymovement) {
                                                    if (pq->direction==DIRECTION_STAY && pq->targetfieldid==fieldid) campers.push_back(pq);
                                                }
                                                attacker atk;
                                                defender def;
                                                if (am->hero!=nullptr) {
                                                    atk.hero.attack=am->hero->GetPower();
                                                    atk.hero.intel=am->hero->GetStratagem();
                                                }
                                                Hero* besthero=nullptr;
                                                int16_t bestPower=-1;
                                                if (campers.size()>0) {
                                                    for (stArmyMovement* pu : campers) {
                                                        def.troops[2]+=pu->troops.scout;
                                                        if (pu->hero!=nullptr && pu->hero->GetPower()>bestPower) {
                                                            besthero=pu->hero;
                                                            bestPower=pu->hero->GetPower();
                                                        }
                                                    }
                                                }
                                                atk.troops[2]=am->troops.scout;
                                                atk.research.military_tradition=fclient->research[T_MILITARYTRADITION].level;
                                                atk.research.iron_working=fclient->research[T_IRONWORKING].level;
                                                atk.research.medicine=fclient->research[T_MEDICINE].level;
                                                atk.research.compass=fclient->research[T_COMPASS].level;
                                                atk.research.horseback_riding=fclient->research[T_HORSEBACKRIDING].level;
                                                atk.research.archery=fclient->research[T_ARCHERY].level;
                                                atk.research.machinery=fclient->research[T_MACHINERY].level;
                                                def.research.military_tradition=oclient->research[T_MILITARYTRADITION].level;
                                                def.research.iron_working=oclient->research[T_IRONWORKING].level;
                                                def.research.medicine=oclient->research[T_MEDICINE].level;
                                                def.research.compass=oclient->research[T_COMPASS].level;
                                                def.research.horseback_riding=oclient->research[T_HORSEBACKRIDING].level;
                                                def.research.archery=oclient->research[T_ARCHERY].level;
                                                def.research.machinery=oclient->research[T_MACHINERY].level;
                                                PlayerCity* defenderCity=(PlayerCity*)tile->m_city;
                                                //also update the hero for a castle
                                                for (Hero* hh : defenderCity->m_heroes) {
                                                    if (hh==0) continue;
                                                    if (hh->GetPower()>bestPower) {
                                                        besthero=hh;
                                                        bestPower=hh->GetPower();
                                                    }
                                                }
                                                int64_t mainScouts=0;
                                                if (defenderCity->m_gooutforbattle) {
                                                    mainScouts=defenderCity->m_troops.scout;
                                                    def.troops[2]+=mainScouts;
                                                }
                                                if (besthero!=nullptr) {
                                                    def.hero.attack=besthero->GetPower();
                                                    def.hero.intel=besthero->GetStratagem();
                                                }
                                                battleResult result;
                                                CombatSimulator::fight(atk,def,&result);
                                                // send info for scout report generation :: TODO

                                                // distribute the damage between all campers
                                                int64_t damage=def.troops[2]-result.defenderTroops[2];
                                                if (campers.size() > 0 &&  damage > 0) {
                                                    for (stArmyMovement* il : campers) {
                                                        int64_t dmg=ceil(il->troops.scout*damage/def.troops[2]);
                                                        if (il->troops.scout > dmg) il->troops.scout-=dmg;
                                                        else il->troops.scout=0;
                                                        // if all the troops are dead, send the hero back
                                                        if (!memcmp(&testMemory,&(il->troops.worker),96)) {
                                                            if (il->hero!=nullptr) {
                                                                il->hero->m_status=DEF_HEROIDLE;
                                                                if (il->city!=0) il->client->HeroUpdate(il->hero->m_id,((PlayerCity*)il->city)->m_castleid);
                                                            }
                                                            il->client->armymovement.remove(il);
                                                            if (am->client!=il->client) {
                                                                am->client->friendarmymovement.remove(il);
                                                                am->client->FriendArmyUpdate();
                                                            }
                                                            else am->client->armymovement.remove(il);
                                                            il->client = 0;
                                                            il->client->SelfArmyUpdate();
                                                        }
                                                    }
                                                }
                                                if (damage > 0) {
                                                    mainScouts=ceil(mainScouts*damage/def.troops[2]);
                                                    int64_t scoutsleft=defenderCity->m_troops.scout-mainScouts;
                                                    if (scoutsleft>0) defenderCity->m_troops.scout=scoutsleft;
                                                    else defenderCity->m_troops.scout=0;
                                                    defenderCity->TroopUpdate();
                                                }
                                                // if defender wins
                                                if (result.result) {
                                                    // means all attacking scouts are dead
                                                    if (result.attackerTroops[2]==0) {
                                                        if (am->hero!=nullptr) {
                                                            if (am->hero->m_loyalty < 5) am->hero->m_loyalty=0;
                                                            else am->hero->m_loyalty-=5;
                                                            if (defenderCity!=0 && defenderCity->GetBuildingLevel(B_FEASTINGHALL)>defenderCity->HeroCount()) {
                                                                int8_t chance=rand()%100;
                                                                // in this case the hero will get captured
                                                                if (chance > am->hero->m_loyalty) {
                                                                    for (int x=0;x<10;++x) {
                                                                        if (fcity->m_heroes[x]==fhero) {
                                                                            fcity->m_heroes[x]=0;
                                                                            break;
                                                                        }
                                                                    }
                                                                    for (int x=0;x<10;++x) {
                                                                        if (!defenderCity->m_heroes[x]) {
                                                                            defenderCity->m_heroes[x]=fhero;
                                                                        }
                                                                    }
                                                                    fhero->m_client=oclient;
                                                                    fhero->m_ownerid=oclient->accountid;
                                                                    fhero->m_loyalty=0;
                                                                    fhero->m_powerbuffadded=0;
                                                                    fhero->m_stratagembuffadded=0;
                                                                    fhero->m_managementbuffadded=0;
                                                                    fhero->m_status=DEF_HEROSEIZED;
                                                                    defenderCity->HeroUpdate(fhero,0);
                                                                    fcity->HeroUpdate(fhero,1);
                                                                    am->client->armymovement.remove(am);
                                                                    am->client->SelfArmyUpdate();
                                                                    armylist.remove(*iter);
                                                                    delete am;
                                                                    continue;
                                                                }
                                                            }
                                                            // otherwise return the hero to the city immediately
                                                            fhero->m_status=DEF_HEROSEIZED;
                                                            fcity->HeroUpdate(fhero,2);
                                                            am->client->armymovement.remove(am);
                                                            am->client->SelfArmyUpdate();
                                                            armylist.remove(*iter);
                                                            delete am;
                                                            continue;
                                                        }
                                                        am->client->armymovement.remove(am);
                                                        am->client->SelfArmyUpdate();
                                                        armylist.remove(*iter);
                                                        delete am;
                                                        continue;
                                                    }
                                                }
                                                // if all attacking scouts aren't dead(doesn't matter if they lost or won), send the army back
                                                am->troops.scout=result.attackerTroops[2];
                                                am->direction=DIRECTION_BACKWARD;
                                                am->reachtime=Utils::time()+am->reachtime-am->starttime-am->resttime;
                                                am->resttime=0;
                                                am->starttime=Utils::time();
                                                am->hero->m_status=DEF_HERORETURN;
                                                am->client->SelfArmyUpdate();
                                                continue;
                                            }
                                        }
                                        // when scouting a NPC
                                        else {
                                            // generate scouting report here
                                            stReport r;
                                            r.guid = Utils::generaterandomstring(28);
                                            r.attack = true;
                                            r.back = false;
                                            r.armytype = MISSION_SCOUT;
                                            r.isread = false;
                                            r.reportid = fclient->currentreportid++;
                                            int xid, yid;
                                            GETXYFROMID4(xid, yid, am->startfieldid, mapsize);
                                            std::stringstream ss;
                                            ss << am->startposname << " (" << xid << "," << yid << ")";
                                            r.startpos = ss.str();
                                            std::stringstream ss1;
                                            GETXYFROMID4(xid, yid, am->targetfieldid, mapsize);
                                            ss1 << am->targetposname << " (" << xid << "," << yid << ")";
                                            r.targetpos = ss1.str();
                                            r.title = "Scout Report";
                                            r.type_id = 1;
                                            r.eventtime = Utils::time();
                                            std::string path = reportbasepath + r.guid + ".xml";
                                            std::ofstream file;
                                            file.open(path, std::ios::out);
                                            NpcCity* npc = ((NpcCity*)tile->m_city);
                                            npc->ResetHero();
                                            Writer writer(file);

                                            writer.openElt("reportData").attr("reportUrl", reportbaseurl + r.guid + ".xml");
                                            writer.openElt("scoutReport").attr("isFound", "true").attr("isSuccess", "true").attr("isAttack", "true");
                                            writer.openElt("scoutInfo").attr("support", std::to_string(npc->m_loyalty)).attr("population", std::to_string(npc->m_population)).attr("gold", std::to_string((int64_t)npc->m_resources.gold)).attr("heroLevel", std::to_string(npc->m_temphero->m_level)).attr("heroName", npc->m_temphero->m_name).attr("heroUrl", "");
                                            writer.openElt("resource").openElt("food").content(std::to_string((int64_t)npc->m_resources.food)).closeElt().openElt("wood").content(std::to_string((int64_t)npc->m_resources.wood)).closeElt().openElt("iron").content(std::to_string((int64_t)npc->m_resources.iron)).closeElt().openElt("stone").content(std::to_string((int64_t)npc->m_resources.stone)).closeElt().closeElt();
                                            if (memcmp(&testMemory, &npc->m_forts, sizeof(stForts))) {
                                                writer.openElt("fortifications");
                                                if (npc->m_forts.traps > 0) writer.openElt("fortificationsType").attr("typeId", std::to_string(TR_TRAP)).attr("count", std::to_string(npc->m_forts.traps)).closeElt();
                                                if (npc->m_forts.abatis > 0) writer.openElt("fortificationsType").attr("typeId", std::to_string(TR_ABATIS)).attr("count", std::to_string(npc->m_forts.abatis)).closeElt();
                                                if (npc->m_forts.logs > 0) writer.openElt("fortificationsType").attr("typeId", std::to_string(TR_ROLLINGLOG)).attr("count", std::to_string(npc->m_forts.logs)).closeElt();
                                                if (npc->m_forts.towers > 0) writer.openElt("fortificationsType").attr("typeId", std::to_string(TR_ARCHERTOWER)).attr("count", std::to_string(npc->m_forts.towers)).closeElt();
                                                if (npc->m_forts.trebs>0) writer.openElt("fortificationsType").attr("typeId", std::to_string(TR_TREBUCHET)).attr("count", std::to_string(npc->m_forts.trebs)).closeElt();
                                                writer.closeElt();
                                            }
                                            if (memcmp(&testMemory, &npc->m_troops, sizeof(NpcCity::stTroops))) {
                                                writer.openElt("troops");
                                                if (npc->m_troops.warrior > 0) writer.openElt("troopStrType").attr("typeId", std::to_string(TR_WARRIOR)).attr("count", std::to_string(npc->m_troops.warrior)).closeElt();
                                                if (npc->m_troops.pike > 0) writer.openElt("troopStrType").attr("typeId", std::to_string(TR_PIKE)).attr("count", std::to_string(npc->m_troops.pike)).closeElt();
                                                if (npc->m_troops.sword > 0) writer.openElt("troopStrType").attr("typeId", std::to_string(TR_SWORDS)).attr("count", std::to_string(npc->m_troops.sword)).closeElt();
                                                if (npc->m_troops.archer > 0) writer.openElt("troopStrType").attr("typeId", std::to_string(TR_ARCHER)).attr("count", std::to_string(npc->m_troops.archer)).closeElt();
                                                if (npc->m_troops.cavalry > 0) writer.openElt("troopStrType").attr("typeId", std::to_string(TR_CAVALRY)).attr("count", std::to_string(npc->m_troops.cavalry)).closeElt();
                                                writer.closeElt();
                                            }
                                            
                                            writer.openElt("buildings");
                                            std::stringstream vec[31];
                                            for (stBuilding& b : npc->m_innerbuildings) {
                                                if (b.type > 30 || b.type==0) continue;
                                                if (vec[b.type].tellp() > 0) vec[b.type] << ",";
                                                vec[b.type] << b.level;
                                            }
                                            for (stBuilding& b : npc->m_outerbuildings) {
                                                if (b.type > 30 || b.type == 0) continue;
                                                if (vec[b.type].tellp() > 0) vec[b.type] << ",";
                                                vec[b.type] << b.level;
                                            }
                                            for (int btype = 0; btype < 31; ++btype) {
                                                if (vec[btype].tellp() > 0) {
                                                    writer.openElt("buildingType").attr("type", std::to_string(btype)).attr("levels", vec[btype].str()).closeElt();
                                                }
                                            }
                                            writer.closeElt();
                                            writer.closeElt();
                                            writer.openElt("battleInfo").attr("isAttack", "true").attr("unNomal", "No defending troops found.");
                                            writer.openElt("backTroop");
                                            writer.openElt("troops");
                                            if (am->hero != 0) {
                                                writer.attr("heroLevel", std::to_string(am->hero->m_level)).attr("heroName", am->hero->m_name).attr("heroUrl", am->hero->m_logourl).attr("isHeroBeSeized", "false");
                                            }
                                            stTroops st = am->troops;
                                            int64_t* trc = (int64_t*)&st;
                                            for (int ij = 0; ij < 12; ij++) {
                                                if ((*(trc + ij)) > 0) writer.openElt("troopInfo").attr("typeId", std::to_string(ij + 2)).attr("remain", std::to_string(*(trc + ij))).closeElt();
                                            }
                                            writer.closeAll();
                                            file.close();
                                            fclient->reportlist.push_front(r);
                                            am->direction=DIRECTION_BACKWARD;
                                            am->reachtime=Utils::time()+am->reachtime-am->starttime-am->resttime;
                                            am->resttime=0;
                                            am->starttime=Utils::time();
                                            if (am->hero) am->hero->m_status=DEF_HERORETURN;
                                            am->client->SelfArmyUpdate();
                                            am->client->ReportUpdate();
                                            continue;
                                        }
                                    }
                                    else if (am->missiontype==MISSION_ATTACK) {
                                        // TODO
                                    }
                                }
                                // if not valid target, just send the army back
                                else {
                                    am->direction=DIRECTION_BACKWARD;
                                    am->reachtime=Utils::time()+am->reachtime-am->starttime-am->resttime;
                                    am->resttime=0;
                                    am->starttime=Utils::time();
                                    if (am->hero) am->hero->m_status=DEF_HERORETURN;
                                    am->client->SelfArmyUpdate();
                                    continue;
                                }
                            }
                            else 
                            {
                                //returning to city from attack/reinforce/etc

                                if (am->hero) am->hero->m_status = DEF_HEROIDLE;

                                am->city->m_resources += am->resources;
                                ((PlayerCity*)am->city)->m_troops += am->troops;
                                am->client->armymovement.remove(am);

                                if (am->hero) ((PlayerCity*)am->city)->HeroUpdate(am->hero, 2);

                                am->client->SelfArmyUpdate();
                                am->client->PlayerInfoUpdate();
                                ((PlayerCity*)am->city)->TroopUpdate();
                                ((PlayerCity*)am->city)->ResourceUpdate();

                                stReport r;
                                r.guid = Utils::generaterandomstring(28);
                                r.attack = false;
                                r.back = true;
                                r.armytype = am->missiontype;
                                r.isread = false;
                                r.reportid = fclient->currentreportid++;
                                int xid, yid;
                                GETXYFROMID4(xid,yid,am->startfieldid,mapsize);
                                std::stringstream ss;
                                ss << am->startposname << " (" << xid << "," << yid << ")";
                                r.startpos = ss.str();
                                std::stringstream ss1;
                                GETXYFROMID4(xid,yid,am->targetfieldid,mapsize);
                                ss1 << am->targetposname << " (" << xid << "," << yid << ")";
                                r.targetpos = ss1.str();
                                if (am->missiontype == MISSION_SCOUT) r.title = "Scout Returned";
                                else if (am->missiontype == MISSION_ATTACK) r.title = "Attack Returned";
                                else r.title = "Returned";
                                r.type_id = 1;
                                r.eventtime = Utils::time();
                                std::string path = reportbasepath + r.guid + ".xml";
                                std::ofstream file;
                                file.open(path, std::ios::out);
                                Writer writer(file);
                                writer.openElt("reportData").attr("reportUrl", reportbaseurl + r.guid + ".xml");
                                writer.openElt("troopMovement").attr("isBack", "true").attr("type", std::to_string(am->missiontype));
                                if (am->hero != 0) {
                                    writer.attr("heroLevel", std::to_string(am->hero->m_level)).attr("heroName", am->hero->m_name).attr("heroUrl", am->hero->m_logourl);
                                }
                                stTroops st = am->troops;
                                int64_t* trc = (int64_t*)&st;
                                for (int ij = 0; ij < 12; ij++) {
                                    if ((*(trc + ij)) > 0) writer.openElt("troops").attr("typeId", std::to_string(ij + 2)).attr("count", std::to_string(*(trc + ij))).closeElt();
                                }
                                writer.closeElt();
                                writer.closeAll();
                                file.close();
                                fclient->reportlist.push_front(r);
                                fclient->ReportUpdate();

                                armylist.remove(*iter);
                                delete am;
                            }
                        }
                    }
                }

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
                                        if (city->m_innheroes[i])
                                        {
                                            delete city->m_innheroes[i];
                                            city->m_innheroes[i] = 0;
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
                                data["castleId"] = city->m_castleid;

                                buildinglist.erase(iter++);

                                double gain = GetPrestigeOfAction(DEF_BUILDING, bldg->type, bldg->level, city->m_level);
                                client->Prestige(gain);

                                delete ba;

                                client->CalculateResources();
                                city->CalculateStats();
                                if (city->m_mayor)
                                {
                                    city->m_mayor->m_experience += gain;
                                    city->HeroUpdate(city->m_mayor, 2);
                                }
                                //city->CastleUpdate();
                                client->PlayerInfoUpdate();
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
                                res.food = m_buildingconfig[bldg->type][bldg->level].food / 3;
                                res.wood = m_buildingconfig[bldg->type][bldg->level].wood / 3;
                                res.stone = m_buildingconfig[bldg->type][bldg->level].stone / 3;
                                res.iron = m_buildingconfig[bldg->type][bldg->level].iron / 3;
                                res.gold = m_buildingconfig[bldg->type][bldg->level].gold / 3;
                                ba->city->m_resources += res;

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
                                data["castleId"] = city->m_castleid;

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
                                city->m_researching = false;
                                client->research[ra->researchid].level++;
                                client->research[ra->researchid].endtime = 0;
                                client->research[ra->researchid].starttime = 0;
                                client->research[ra->researchid].castleid = 0;

                                amf3object obj = amf3object();
                                obj["cmd"] = "server.ResearchCompleteUpdate";
                                obj["data"] = amf3object();

                                amf3object & data = obj["data"];

                                data["castleId"] = city->m_castleid;


                                researchlist.erase(iter++);

                                double gain = GetPrestigeOfAction(DEF_RESEARCH, ra->researchid, client->research[ra->researchid].level, city->m_level);
                                client->Prestige(gain);


                                client->CalculateResources();
                                city->CalculateStats();
                                if (city->m_mayor)
                                {
                                    city->m_mayor->m_experience += gain;
                                    city->HeroUpdate(city->m_mayor, 2);
                                }
                                //city->CastleUpdate();
                                client->PlayerInfoUpdate();
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
                m_alliances->SortAlliances();
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
                //                    savethread = shared_ptr<thread>(new thread(std::bind(std::mem_fun(&Server::SaveData), this)));
                //                std::thread timerthread(std::bind(std::mem_fun(&Server::TimerThread), gserver));
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
                    for (int i = 0; i < m_city.size(); ++i)
                    {
                        if (m_city.at(i)->m_type == NPC)
                        {
                            ((NpcCity*)m_city.at(i))->CalculateStats(true, true);
                        }
                        else if (m_city.at(i)->m_type == CASTLE)
                        {
                            ((PlayerCity*)m_city.at(i))->RecalculateCityStats();
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
            //                 LOCK(M_RANKEDLIST);
            //                 gserver->SortPlayers();
            //                 gserver->SortHeroes();
            //                 gserver->SortCastles();
            //                 gserver->m_alliances->SortAlliances();
            //                 UNLOCK(M_RANKEDLIST);
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
        if (m_items[i].name == name)
            return &m_items[i];
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

// rand() % 10000 gives a number in the range 0..9999
// int rnd = rand() % 10000; gives us a granularity of 10000
// a possible one in 10000 chance.
// 6500 times out of 10000 a normal hero is generated
// with a stat of between 15 and 35.
// Then, 'else if' is used to divide up the remaining 3500
// into sections 30, 45, 60, 70 etc
// a small random number is added to each to add a
// spread within that range.
// e.g. rand() % 6 gives a number in the range 0..5
// a value of 10 is returned at the end if all else fails.

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
    // a 25 in 10000 chance of getting one of these
    {
        return rand() % 5 + 75; // 75-80
    }
    // Leave out these two lines, max stat is 79. //
    //    else if ((rnd >= 9975) && (rnd < 10000))
    //    {
    //        return rand() % 6 + 80; // 80-85
    //    }

    // anything above 9975 gets a 10
    // a 25 in 10000 chance of getting a 10
    // for this stat.
    return 10;
}


Hero * spitfire::CreateRandomHero(int innlevel) const
{
    Hero * hero = new Hero();

    int maxherolevel = innlevel * 5;

    hero->m_level = (rand() % maxherolevel) + 1;
    hero->m_basemanagement = RandomStat();
    hero->m_basestratagem = RandomStat();
    hero->m_basepower = RandomStat();

    int remainpoints = hero->m_level;

    hero->m_power = rand() % remainpoints;
    remainpoints -= hero->m_power;
    hero->m_power += hero->m_basepower;
    if (remainpoints > 0)
    {
        hero->m_management = rand() % remainpoints;
        remainpoints -= hero->m_management;
        hero->m_management += hero->m_basemanagement;
    }
    if (remainpoints > 0)
    {
        hero->m_stratagem = remainpoints;
        remainpoints -= hero->m_stratagem;
        hero->m_stratagem += hero->m_basestratagem;
    }


    hero->m_loyalty = 70;
    hero->m_experience = 0;
    hero->m_upgradeexp = hero->m_level * hero->m_level * 100;
    hero->m_id = 0;
    char tempstr[30];
    sprintf(tempstr, "Test Name%d%d%d", hero->m_power, hero->m_management, hero->m_stratagem);
    hero->m_name = tempstr;
    hero->m_logourl = "images/icon/player/faceA20.jpg";

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
bool spitfire::comparearmies(stTimedEvent& x,stTimedEvent& y)
{
    return (((stArmyMovement*)x.data)->reachtime < ((stArmyMovement*)y.data)->reachtime);
}

std::string spitfire::readreport(std::string report_id)
{
    try {
        std::ifstream file;
        std::ostringstream ss;
        file.open(_instance->reportbasepath+report_id+".xml", std::ios::in);
        ss << file.rdbuf();
        return ss.str();
    }
    catch (...) {
        return "";
    }
}

void spitfire::SortPlayers()
{
    m_prestigerank.clear();
    m_honorrank.clear();
    m_titlerank.clear();
    m_populationrank.clear();
    m_citiesrank.clear();


    std::list<Client*>::iterator playeriter;
    for (playeriter = players.begin(); playeriter != players.end(); ++playeriter)
    {
        Client * client = *playeriter;

        stClientRank rank = stClientRank();
        rank.client = client;
        rank.rank = 0;

        m_prestigerank.push_back(rank);
        m_honorrank.push_back(rank);
        m_titlerank.push_back(rank);
        m_populationrank.push_back(rank);
        m_citiesrank.push_back(rank);
    }

    m_prestigerank.sort(compareprestige);
    m_honorrank.sort(comparehonor);
    m_titlerank.sort(comparetitle);
    m_populationrank.sort(comparepop);
    m_citiesrank.sort(comparecities);


    int num = 1;
    for (stClientRank & ranklist : m_prestigerank)
    {
        ranklist.rank = num;
        ranklist.client->prestigerank = num++;
        if (ranklist.client->connected)
            ranklist.client->PlayerInfoUpdate();
    }
    num = 1;
    for (stClientRank & ranklist : m_honorrank)
    {
        ranklist.rank = num++;
    }
    num = 1;
    for (stClientRank & ranklist : m_titlerank)
    {
        ranklist.rank = num++;
    }
    num = 1;
    for (stClientRank & ranklist : m_populationrank)
    {
        ranklist.rank = num++;
    }
    num = 1;
    for (stClientRank & ranklist : m_citiesrank)
    {
        ranklist.rank = num++;
    }
}

void spitfire::SortHeroes()
{
    m_herorankstratagem.clear();
    m_herorankpower.clear();
    m_herorankmanagement.clear();
    m_herorankgrade.clear();


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
                    if (client->citylist[j]->m_heroes[k])
                    {
                        Hero * hero = client->citylist[j]->m_heroes[k];
                        stHeroRank rank = stHeroRank();
                        assert(hero->m_level > 0);
                        assert(hero->m_stratagem > 0);
                        assert(hero->m_management > 0);
                        assert(hero->m_power > 0);
                        rank.grade = hero->m_level;
                        rank.stratagem = hero->m_stratagem;
                        rank.management = hero->m_management;
                        rank.power = hero->m_power;
                        rank.name = hero->m_name;
                        rank.kind = client->playername;
                        rank.rank = 0;
                        m_herorankstratagem.push_back(rank);
                        m_herorankpower.push_back(rank);
                        m_herorankmanagement.push_back(rank);
                        m_herorankgrade.push_back(rank);
                    }
                }
            }
        }
    }

    m_herorankstratagem.sort(comparestratagem);
    m_herorankpower.sort(comparepower);
    m_herorankmanagement.sort(comparemanagement);
    m_herorankgrade.sort(comparegrade);

    std::list<stHeroRank>::iterator iter;
    int num = 1;
    for (iter = m_herorankstratagem.begin(); iter != m_herorankstratagem.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = m_herorankpower.begin(); iter != m_herorankpower.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = m_herorankmanagement.begin(); iter != m_herorankmanagement.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = m_herorankgrade.begin(); iter != m_herorankgrade.end(); ++iter)
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
    m_castleranklevel.clear();
    m_castlerankpopulation.clear();

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
                    rank.alliance = client->GetAlliance()->m_name;
                else
                    rank.alliance = "";
                rank.level = level;
                rank.population = city->m_population;
                rank.name = city->m_cityname;
                rank.grade = grade;
                rank.kind = client->playername;
                rank.rank = 0;
                m_castleranklevel.push_back(rank);
                m_castlerankpopulation.push_back(rank);
            }
        }
    }

    m_castleranklevel.sort(comparelevel);
    m_castlerankpopulation.sort(comparepopulation);

    std::list<stCastleRank>::iterator iter;
    int num = 1;
    for (iter = m_castleranklevel.begin(); iter != m_castleranklevel.end(); ++iter)
    {
        iter->rank = num++;
    }
    num = 1;
    for (iter = m_castlerankpopulation.begin(); iter != m_castlerankpopulation.end(); ++iter)
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
        for (iter = m_searchclientranklist.begin(); iter != m_searchclientranklist.end();)
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
        m_searchclientranklist.push_back(searchrank);
        return &m_searchclientranklist.back();
    }
    else if (type == 2)//hero lists
    {
        std::list<stSearchHeroRank>::iterator iter;
        for (iter = m_searchheroranklist.begin(); iter != m_searchheroranklist.end();)
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
        m_searchheroranklist.push_back(searchrank);
        return &m_searchheroranklist.back();
    }
    else if (type == 3)//castle lists
    {
        std::list<stSearchCastleRank>::iterator iter;
        for (iter = m_searchcastleranklist.begin(); iter != m_searchcastleranklist.end();)
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
        m_searchcastleranklist.push_back(searchrank);
        return &m_searchcastleranklist.back();
    }
    else// if (type == 4)//alliance lists
    {
        std::list<stSearchAllianceRank>::iterator iter;
        for (iter = m_searchallianceranklist.begin(); iter != m_searchallianceranklist.end();)
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
            if (Utils::ci_find(iteralliance->ref->m_name, key) != std::string::npos)
            {
                stAlliance alliancerank;
                alliancerank.ref = iteralliance->ref;
                alliancerank.rank = iteralliance->rank;
                searchrank.ranklist.push_back(alliancerank);
            }
            ++iteralliance;
        }
        m_searchallianceranklist.push_back(searchrank);
        return &m_searchallianceranklist.back();
    }
    return 0;
}

void spitfire::CheckRankSearchTimeouts(uint64_t time)
{
    std::list<stSearchClientRank>::iterator iterclient;
    std::list<stSearchHeroRank>::iterator iterhero;
    std::list<stSearchCastleRank>::iterator itercastle;

    for (iterclient = m_searchclientranklist.begin(); iterclient != m_searchclientranklist.end();)
    {
        if (iterclient->lastaccess + 30000 < time)
        {
            m_searchclientranklist.erase(iterclient++);
            continue;
        }
        ++iterclient;
    }

    for (iterhero = m_searchheroranklist.begin(); iterhero != m_searchheroranklist.end();)
    {
        if (iterhero->lastaccess + 30000 < time)
        {
            m_searchheroranklist.erase(iterhero++);
            continue;
        }
        ++iterhero;
    }

    for (itercastle = m_searchcastleranklist.begin(); itercastle != m_searchcastleranklist.end();)
    {
        if (itercastle->lastaccess + 30000 < time)
        {
            m_searchcastleranklist.erase(itercastle++);
            continue;
        }
        ++itercastle;
    }
}

bool spitfire::CreateMail(std::string sender, std::string receiver, std::string subject, std::string content, int8_t type)
{
    Client * snd = this->GetClientByName(sender);
    Client * rcv = this->GetClientByName(receiver);

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

bool spitfire::Init()
{
    try
    {
        std::ifstream config("config.json");

        json obj;

        config >> obj;


        log->info("Loading config.json");

        bindaddress = obj["bindaddress"];
        log->info("bindaddress: {}", bindaddress);

        bindport = obj["bindport"];
        log->info("bindport: {}", bindport);

        maxplayers = obj["maxplayers"];
        log->info("maxplayers: {}", maxplayers);

        mapsize = obj["mapsize"];
        log->info("mapsize: {}", mapsize);

        sqlhost = obj["sqlhost"];
        log->info("sqlhost: {}", sqlhost);

        sqluser = obj["sqluser"];
        log->info("sqluser: {}", sqluser);

        sqlpass = obj["sqlpass"];
        log->info("sqlpass set");

        dbmaintable = obj["dbmaintable"];
        log->info("dbmaintable: {}", dbmaintable);

        dbservertable = obj["dbservertable"];
        log->info("dbservertable: {}", dbservertable);

        servername = obj["servername"];
        log->info("servername: {}", servername);

        reportbasepath = obj["reportbasepath"];
        log->info("report base path: {}",reportbasepath);

        reportbaseurl = obj["reportbaseurl"];
        log->info("report base url: {}",reportbaseurl);
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
    return true;
}

void spitfire::setupLogging()
{
    spdlog::set_async_mode(32);
    log = spdlog::stdout_color_mt("spitfire");
    log->set_level(spdlog::level::level_enum::trace);
}
