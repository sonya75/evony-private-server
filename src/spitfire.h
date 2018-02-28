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

#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>
#include <string>
#include <mutex>

#include <nlohmann/json.hpp>

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
    spitfire();
    ~spitfire();

    enum server_status
    {
        SERVERSTATUS_STOPPED,
        SERVERSTATUS_STARTING,
        SERVERSTATUS_ONLINE,
        SERVERSTATUS_SHUTDOWN
    };


    void start_logging();
    void run(size_t thread_pool_size);
    void stop();
    void start(connection_ptr c);
    void start_policy(connection_ptr c);
    bool initialize();
    void stop(connection_ptr c, bool checklock = true);
    void stop_all();
    void do_accept();
    void do_acceptpolicy();

    void player_count(int8_t amount)
    {
        players_online += amount;
        if (players_online < 0)
        {
            players_online = 0;
            std::cerr << "server.currentplayersonline less than 0 error\n";
        }
    }
    int64_t player_count() { return players_online; }

    Client * new_client();
    Client * get_client(int64_t accountid);
    Client * GetClientByParent(int64_t accountid);
    Client * get_client_by_name(std::string name);
    Client * get_client_by_castle(int64_t castleid);
    int32_t  get_client_index(int64_t accountid);

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
    void close_client(Client * client, int typecode = 1, std::string message = "Connection Closed") const;

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
    uint32_t players_online;

    // Whether the timer thread is running or not
    bool TimerThreadRunning;
    bool SaveThreadRunning;

    Map * game_map;


    // List of active cities on the server (NPC and Player)
    std::vector<City*> city;

    // Alliance Controller
    AllianceMgr * alliances;


    std::list<Client*> players;

    uint64_t ltime;

    uint64_t armycounter;
    uint64_t tecounter;
    std::string itemxml;
    stItemConfig items[400];
    int itemcount;
    stRarityGamble gambleitems;

    Market market;

#define DEF_TIMEDARMY 1
#define DEF_TIMEDBUILDING 2
#define DEF_TIMEDRESEARCH 3

    stItemConfig * GetItem(std::string name);

    double GetPrestigeOfAction(int8_t action, int8_t id, int8_t level, int8_t thlevel) const;


    void MassDisconnect();
    void AddTimedEvent(stTimedEvent & te);


    stBuildingConfig buildingconfig[35][10];
    stBuildingConfig researchconfig[25][10];
    stBuildingConfig troopconfig[20];

    std::list<stTimedEvent> armylist;
    std::list<stTimedEvent> buildinglist;
    std::list<stTimedEvent> researchlist;

    std::queue<stPacketOut> packetout;

    int64_t heroid;
    int64_t cityid;
    int32_t allianceid;

    std::vector<int32_t> deletedhero;
    std::vector<int32_t> deletedcity;

    int RandomStat() const;


    Hero * CreateRandomHero(int innlevel) const;



    static bool compareprestige(stClientRank first, stClientRank second);
    static bool comparehonor(stClientRank first, stClientRank second);
    static bool comparetitle(stClientRank first, stClientRank second);
    static bool comparepop(stClientRank first, stClientRank second);
    static bool comparecities(stClientRank first, stClientRank second);

    void SortPlayers();


    std::list<stClientRank> prestigerank;
    std::list<stClientRank> honorrank;
    std::list<stClientRank> titlerank;
    std::list<stClientRank> populationrank;
    std::list<stClientRank> citiesrank;



    std::list<stHeroRank> herorankstratagem;
    std::list<stHeroRank> herorankpower;
    std::list<stHeroRank> herorankmanagement;
    std::list<stHeroRank> herorankgrade;

    void SortHeroes();
    static bool comparestratagem(stHeroRank first, stHeroRank second);
    static bool comparepower(stHeroRank first, stHeroRank second);
    static bool comparemanagement(stHeroRank first, stHeroRank second);
    static bool comparegrade(stHeroRank first, stHeroRank second);


    std::list<stCastleRank> castleranklevel;
    std::list<stCastleRank> castlerankpopulation;

    void SortCastles();
    static bool comparepopulation(stCastleRank first, stCastleRank second);
    static bool comparelevel(stCastleRank first, stCastleRank second);


    std::list<stSearchClientRank> searchclientranklist;
    std::list<stSearchHeroRank> searchheroranklist;
    std::list<stSearchCastleRank> searchcastleranklist;
    std::list<stSearchAllianceRank> searchallianceranklist;

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

