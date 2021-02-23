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

    int16_t m_currentmembers;
    int16_t m_maxmembers;

    int64_t m_prestige;
    int64_t m_honor;
    int16_t m_prestigerank;
    int16_t m_honorrank;
    int16_t m_membersrank;
    int32_t m_allcitycount;
    int32_t m_allicitycount;
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

    std::list<stInviteList> m_invites;

    std::list<stMember> m_members;
    std::list<int64_t> m_enemies;
    std::list<int64_t> m_allies;
    std::list<int64_t> m_neutral;
    std::vector<stReport> m_alliancereports;
    std::vector<stReport> m_allianceevents;

    int64_t m_ownerid;
    std::string m_owner;
    std::string m_name;
    std::string m_founder;
    int64_t m_founderid;
    std::string m_note;
    std::string m_intro;
    std::string m_motd;

    int64_t m_allianceid;
};
