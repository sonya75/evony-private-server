/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "structs.h"

class spitfire;
class Client;

#define DEF_ALLIANCE 0
#define DEF_ALLY 1
#define DEF_NEUTRAL 2
#define DEF_ENEMY 3
#define DEF_NORELATION 4
#define DEF_SELFRELATION 5

// white = 2
// green = 0?
// teal (ally) = 1
// red = 3
// white = 4
// 

class Alliance
{
public:
    Alliance(std::string name, std::string founder);
    ~Alliance();

    amf3object ToObject();

    bool SaveToDB();
    bool DeleteFromDB();
    bool InsertToDB();

    bool HasMember(std::string username);
    bool HasMember(uint64_t clientid);
    bool IsEnemy(int64_t allianceid);
    bool IsAlly(int64_t allianceid);
    bool IsNeutral(int64_t allianceid);
    void Ally(int64_t allianceid);
    void Neutral(int64_t allianceid);
    void Enemy(int64_t allianceid, bool skip = false);
    void UnAlly(int64_t allianceid);
    void UnNeutral(int64_t allianceid);
    void UnEnemy(int64_t allianceid);

    bool AddMember(uint64_t clientid, uint8_t rank);
    bool RemoveMember(uint64_t clientid);
    void ParseMembers(std::string str);
    void ParseRelation(std::list<int64_t> * list, std::string str);

    void RequestJoin(Client * client, uint64_t timestamp);
    void UnRequestJoin(Client * client);
    void UnRequestJoin(std::string client);

    void SendAllianceMessage(std::string msg, bool tv, bool nosender);


    amf3object indexAllianceInfoBean();

    uint64_t enemyactioncooldown;

    int16_t currentmembers;
    int16_t maxmembers;

    int64_t prestige;
    int64_t honor;
    int16_t prestigerank;
    int16_t honorrank;
    int16_t membersrank;
    int32_t citycount;

    struct stMember
    {
        bool operator==(const stMember& rhs) const
        {
            if ((clientid == rhs.clientid) && (rank == rhs.rank))
                return true;
            return false;
        }
        uint64_t clientid;
        int8_t rank;
    };

    struct stInviteList
    {
        bool operator==(const stInviteList& rhs) const
        {
            if ((client == rhs.client) && (inviteperson == rhs.inviteperson) && (invitetime == rhs.invitetime))
                return true;
            return false;
        }
        Client * client;
        std::string inviteperson;
        double invitetime;
    };

    std::list<stInviteList> invites;

    std::list<stMember> members;
    std::list<int64_t> enemies;
    std::list<int64_t> allies;
    std::list<int64_t> neutral;

    int64_t ownerid;
    std::string owner;
    std::string name;
    std::string founder;
    std::string note;
    std::string intro;
    std::string motd;

    int64_t allianceid;
};
