/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once


#include <cstdio>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <functional>
#include <memory>
#include <set>

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <Poco/StreamCopier.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Data/SessionPool.h>

// #include <boost/asio.hpp>
// #include <boost/bind.hpp>
// #include <boost/bind/placeholders.hpp>
// #include <boost/asio/steady_timer.hpp>
// #include <boost/lexical_cast.hpp>
// 
// #include <boost/asio/io_service.hpp>
// #include <boost/asio/ip/address.hpp>
// #include <boost/tuple/tuple.hpp>
// #include <boost/tokenizer.hpp>
// #include <boost/thread/future.hpp>

#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>
#include <string>
#include <mutex>

#include <nlohmann/json.hpp>
//#include "../lib/fmt/fmt/ostream.h"

#include "connection.h"
#include "amf3.h"
#include "Market.h"
#include "Utils.h"
#include "AllianceMgr.h"
#include "Map.h"
#include "structs.h"
#include <queue>


using json = nlohmann::json;

class spitfire
{
public:
    static spitfire & CreateInstance() { _instance = new spitfire(); return *_instance; }
    static spitfire & GetSingleton() { return *_instance; }
    static void DestroyInstance() { delete _instance; }
private:
    spitfire();
    ~spitfire();
    static spitfire * _instance;
    static int64_t testMemory[12];
public:

    enum server_status
    {
        SERVERSTATUS_STOPPED,
        SERVERSTATUS_STARTING,
        SERVERSTATUS_ONLINE,
        SERVERSTATUS_SHUTDOWN
    };


    void setupLogging();
    void io_thread();
    void run();
    void stop();
    void start(connection_ptr c);
    void startpolicy(connection_ptr c);
    bool Init();
    void stop(connection_ptr c, bool checklock = true);
    void stop_all();
    void do_accept();
    void do_acceptpolicy();

    void PlayerCount(int8_t amount)
    {
        currentplayersonline += amount;
        if (currentplayersonline < 0)
        {
            currentplayersonline = 0;
            std::cerr << "server.currentplayersonline less than 0 error\n";
        }
    }
    int64_t PlayerCount() { return currentplayersonline; }

    bool ConnectSQL();
    bool InitSockets();
    Client * NewClient();
    Client * GetClient(int64_t accountid);
    Client * GetClientByParent(int64_t accountid);
    Client * GetClientByName(std::string name);
    Client * GetClientByCastle(int64_t castleid);
    int32_t  GetClientIndex(int64_t accountid);

    std::shared_ptr<spdlog::logger> log;
    
    asio::io_service io_service_;
    asio::signal_set signals_;
    asio::ip::tcp::acceptor acceptor_;
    std::set<connection_ptr> connections_;
    asio::ip::tcp::socket socket_;
    request_handler request_handler_;
    asio::ip::tcp::acceptor acceptorpolicy_;
    asio::ip::tcp::socket socketpolicy_;
    request_handler request_handlerpolicy_;

    std::list<request> packetqueue;
    std::mutex m;

    server_status serverstatus;
    std::string servername;


    std::chrono::steady_clock::time_point starttime;
    asio::io_service io_service;
    std::thread workthread;

    bool isrunning = true;
    bool active = true;

    // reasonCode:
    // 1 = "Server restarting"
    // 2 = "Server is about to shut down. Logout as soon as possible"
    // 3 = "Another computer has logged into your account. Make sure you are not trying to log into the game from two computers or browser windows."
    // 4 = "Server undergoing maintenance."
    // 5 = custom message
    void CloseClient(Client * client, int typecode = 1, std::string message = "Connection Closed") const;

    City * AddPlayerCity(Client * client, int tileid, uint64_t castleid);
    City * AddNpcCity(int tileid);
    void MassMessage(std::string str, bool nosender = false, bool tv = false, bool all = false);
    void SendMessage(Client * client, std::string str, bool nosender = false, bool tv = false, bool all = false) const;
    void Shutdown();
    int32_t CalcTroopSpeed(PlayerCity * city, stTroops & troops, int32_t starttile, int32_t endtile) const;

    void SendObject(Client * c, const amf3object & object) const;

    void SendObject(connection * s, const amf3object & object) const
    {
        if (s == nullptr)
            return;
        try
        {
            if (serverstatus == 0)
                return;
            char buffer[15000];
            auto length = 0;
            amf3writer writer(buffer + 4);

            writer.Write(object);

            (*(int32_t*)buffer) = length = (uint32_t)writer.position;
            Utils::ByteSwap5((unsigned char *)&buffer, sizeof(*(int32_t*)buffer));

            if (s != nullptr)
                s->write(buffer, length + 4);
        }
        catch (std::exception& e)
        {
            std::cerr << s->address << "exception: " << __FILE__ << " @ " << __LINE__ << "\n";
            std::cerr << e.what() << "\n";
        }
    }

    // Parse chat for commands
    bool ParseChat(Client * client, std::string str);

    // Get Alliance based relation between two clients
    int16_t GetRelation(int32_t client1, int32_t client2) const;

    void TimerThread();
    void SaveThread();


    // MySQL
    std::string sqlhost, sqluser, sqlpass, bindaddress, bindport;
    std::string dbmaintable;
    std::string dbservertable;


    std::string reportbasepath, reportbaseurl;
    // Map size -- typically 500x500 or 800x800
    uint16_t mapsize;

    // MySQL connection pools
    Poco::Data::SessionPool * accountpool;
    Poco::Data::SessionPool * serverpool;

    // mutexes
    //         struct mutexes
    //         {
    //             shared_mutex tiledata;
    //             shared_mutex clientlist;
    //             shared_mutex lua;
    //             shared_mutex timers;
    //             shared_mutex market;
    //             shared_mutex alliance;
    //             shared_mutex ranklist;
    //             shared_mutex herocreate;
    //             shared_mutex maillist;
    //             shared_mutex reportlist;
    //         } mtxlist;

    // Max players allowed connected
    uint32_t maxplayers;

    // Current players connected
    uint32_t currentplayersonline;

    // Whether the timer thread is running or not
    bool TimerThreadRunning;
    bool SaveThreadRunning;

    Map * map;


    // List of active cities on the server (NPC and Player)
    std::vector<City*> m_city;

    // Alliance Controller
    AllianceMgr * m_alliances;


    std::list<Client*> players;

    uint64_t ltime;

    uint64_t armycounter;
    uint64_t tecounter;
    std::string m_itemxml;
    stItemConfig m_items[400];
    int m_itemcount;
    stRarityGamble m_gambleitems;

    Market market;

#define DEF_TIMEDARMY 1
#define DEF_TIMEDBUILDING 2
#define DEF_TIMEDRESEARCH 3

    stItemConfig * GetItem(std::string name);

    double GetPrestigeOfAction(int8_t action, int8_t id, int8_t level, int8_t thlevel) const;


    void MassDisconnect();
    void AddTimedEvent(stTimedEvent & te);


    stBuildingConfig m_buildingconfig[35][10];
    stBuildingConfig m_researchconfig[25][10];
    stBuildingConfig m_troopconfig[20];

    std::list<stTimedEvent> armylist;
    std::list<stTimedEvent> buildinglist;
    std::list<stTimedEvent> researchlist;

    std::queue<stPacketOut> m_packetout;

    int64_t m_heroid;
    int64_t m_cityid;
    int32_t m_allianceid;

    std::vector<int32_t> m_deletedhero;
    std::vector<int32_t> m_deletedcity;

    int RandomStat() const;


    Hero * CreateRandomHero(int innlevel) const;



    static bool compareprestige(stClientRank first, stClientRank second);
    static bool comparehonor(stClientRank first, stClientRank second);
    static bool comparetitle(stClientRank first, stClientRank second);
    static bool comparepop(stClientRank first, stClientRank second);
    static bool comparecities(stClientRank first, stClientRank second);

    void SortPlayers();


    std::list<stClientRank> m_prestigerank;
    std::list<stClientRank> m_honorrank;
    std::list<stClientRank> m_titlerank;
    std::list<stClientRank> m_populationrank;
    std::list<stClientRank> m_citiesrank;



    std::list<stHeroRank> m_herorankstratagem;
    std::list<stHeroRank> m_herorankpower;
    std::list<stHeroRank> m_herorankmanagement;
    std::list<stHeroRank> m_herorankgrade;

    void SortHeroes();
    static bool comparestratagem(stHeroRank first, stHeroRank second);
    static bool comparepower(stHeroRank first, stHeroRank second);
    static bool comparemanagement(stHeroRank first, stHeroRank second);
    static bool comparegrade(stHeroRank first, stHeroRank second);
    static bool comparearmies(stTimedEvent& x,stTimedEvent& y);

    static std::string readreport(std::string report_id);

    std::list<stCastleRank> m_castleranklevel;
    std::list<stCastleRank> m_castlerankpopulation;

    void SortCastles();
    static bool comparepopulation(stCastleRank first, stCastleRank second);
    static bool comparelevel(stCastleRank first, stCastleRank second);


    std::list<stSearchClientRank> m_searchclientranklist;
    std::list<stSearchHeroRank> m_searchheroranklist;
    std::list<stSearchCastleRank> m_searchcastleranklist;
    std::list<stSearchAllianceRank> m_searchallianceranklist;

    void * DoRankSearch(std::string key, int8_t type, void * subtype, int16_t page, int16_t pagesize);
    void CheckRankSearchTimeouts(uint64_t time);

    // Construct an error message to send to the client
    static amf3object CreateError(std::string cmd, int32_t id, std::string message)
    {
        amf3object obj;
        obj["cmd"] = cmd;
        obj["data"] = amf3object();
        amf3object & data = obj["data"];
        data["errorMsg"] = message;
        data["packageId"] = 0.0;
        data["ok"] = id;
        return obj;
    }

    // Construct an error message to send to the client
    // gserver->SendObject(&c, *gserver->CreateError2("error.error", -1, "message").get());
    static amf3object CreateError2(std::string cmd, int32_t id, std::string message)
    {
        amf3object obj;
        obj["cmd"] = cmd;
        obj["data"] = amf3object();
        amf3object & data = obj["data"];
        data["errorMsg"] = message;
        data["packageId"] = 0.0;
        data["ok"] = id;
        return obj;
    }

    static amf3object CreateError3(std::string cmd, int32_t code, std::string message = "")
    {
        amf3object obj;
        obj["cmd"] = cmd;
        obj["data"] = amf3object();
        amf3object & data = obj["data"];
        data["reasonCode"] = code;
        data["msg"] = message;
        return obj;
    }

    bool CreateMail(std::string sender, std::string receiver, std::string subject, std::string content, int8_t type);
};

