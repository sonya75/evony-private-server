/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "AllianceMgr.h"
#include "Alliance.h"
#include "spitfire.h"
#include "Client.h"


AllianceMgr::AllianceMgr()
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        m_alliances[i] = nullptr;
    }
}

AllianceMgr::~AllianceMgr()
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (m_alliances[i])
            delete m_alliances[i];
    }
}

void AllianceMgr::DeleteAlliance(Alliance * alliance)
{
    int32_t found = -1;
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (m_alliances[i])
        {
            if (m_alliances[i]->m_allianceid != alliance->m_allianceid)
            {
                if (m_alliances[i]->IsAlly(alliance->m_allianceid))
                    m_alliances[i]->UnAlly(alliance->m_allianceid);
                if (m_alliances[i]->IsNeutral(alliance->m_allianceid))
                    m_alliances[i]->UnNeutral(alliance->m_allianceid);
                if (m_alliances[i]->IsEnemy(alliance->m_allianceid))
                    m_alliances[i]->UnEnemy(alliance->m_allianceid);


                for (Client * client : spitfire::GetSingleton().players)
                {
                    if (client)
                    {
                        if ((client->allianceapply == alliance->m_name))
                        {
                            client->allianceapply = "";
                            client->allianceapplytime = 0;
                        }
                    }
                }
            }
            else
            {
                found = i;
            }
        }
    }
    if (found == -1)
    {
        //error point
        return;
    }
    alliance->m_invites.clear();
    alliance->DeleteFromDB();
    delete m_alliances[found];
    m_alliances[found] = nullptr;
    SortAlliances();
}

void AllianceMgr::CheckAlliance(Alliance * alliance)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (m_alliances[i] != 0)
        {
            if (m_alliances[i]->m_members.size() == 0)
            {
                //no more people left in alliance
                DeleteAlliance(m_alliances[i]);
            }
        }
    }
}

Alliance * AllianceMgr::CreateAlliance(std::string name, std::string founder, int64_t allianceid, bool savetodb)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (m_alliances[i] == nullptr)
        {
            m_alliances[i] = new Alliance(name, founder);
            if (allianceid == 0)
                m_alliances[i]->m_allianceid = spitfire::GetSingleton().m_allianceid++;
            else
                m_alliances[i]->m_allianceid = allianceid;
            if (savetodb)
                m_alliances[i]->InsertToDB();
            return m_alliances[i];
        }
    }
    return 0;
}

bool AllianceMgr::JoinAlliance(uint64_t allianceid, Client * client)
{
    if (client->allianceid > 0)
    {
        return false; //already in an alliance
    }
    Alliance * alliance = AllianceById(allianceid);
    if (alliance == (Alliance*)-1)
    {
        return false; //alliance doesn't exist
    }


    if (alliance->AddMember(client->accountid, DEF_ALLIANCEMEMBER))
    {
        client->allianceid = alliance->m_allianceid;
        client->alliancename = alliance->m_name;
        client->alliancerank = DEF_ALLIANCEMEMBER;
        client->PlayerUpdate();
        return true;
    }
    return false;
}

bool AllianceMgr::RemoveFromAlliance(uint64_t allianceid, Client * client)
{
    if (client->allianceid <= 0)
    {
        return false; //not in an alliance
    }
    Alliance * alliance = AllianceById(allianceid);
    if (alliance == (Alliance*)-1)
    {
        return false; //alliance doesn't exist
    }

    alliance->RemoveMember(client->accountid);

    client->allianceid = 0;
    client->alliancename = "";
    client->alliancerank = 0;

    client->PlayerUpdate();
    return true;
}

bool AllianceMgr::SetRank(uint64_t allianceid, Client * client, int8_t rank)
{
    if (client->allianceid <= 0)
    {
        return false; //no alliance id given
    }
    Alliance * alliance = AllianceById(allianceid);
    if (alliance == (Alliance*)-1)
    {
        return false; //alliance doesn't exist
    }


    for (Alliance::stMember & member : alliance->m_members)
    {
        if (member.clientid == client->accountid)
        {
            client->alliancerank = rank;
            member.rank = rank;
            return true;
        }
    }
    return false;
}

bool AllianceMgr::CheckName(std::string name)
{
    //     char * tempname;
    //     tempname = new char[strlen(name)+1];
    //     memset(tempname, 0, strlen(name)+1);
    //     for (int i = 0; i < strlen(name); ++i)
    //         tempname[i] = tolower(name[i]);

    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    if (name.find("penis") != std::string::npos)
        return false;
    if (name.find("cock") != std::string::npos)
        return false;
    if (name.find("dick") != std::string::npos)
        return false;
    if (name.find("asshole") != std::string::npos)
        return false;
    if (name.find("fuck") != std::string::npos)
        return false;
    if (name.find("shit") != std::string::npos)
        return false;
    if (name.find("bitch") != std::string::npos)
        return false;
    if (name.find("tits") != std::string::npos)
        return false;
    if (name.find("cunt") != std::string::npos)
        return false;
    if (name.find("testicle") != std::string::npos)
        return false;
    if (name.find("queer") != std::string::npos)
        return false;
    if (name.find("fag") != std::string::npos)
        return false;
    if (name.find("gay") != std::string::npos)
        return false;
    if (name.find("homo") != std::string::npos)
        return false;
    if (name.find("whore") != std::string::npos)
        return false;
    if (name.find("nigger") != std::string::npos)
        return false;
    return true;
}

int16_t AllianceMgr::GetRelation(int64_t client1, int64_t client2)
{
    if (client1 == client2)
        return DEF_SELFRELATION;

    Client * c1 = spitfire::GetSingleton().GetClient(client1);
    Client * c2 = spitfire::GetSingleton().GetClient(client2);

    if (!c1 || !c2 || c1->allianceid < 0 || c2->allianceid < 0)
        return DEF_NORELATION;

    if (c1->allianceid == c2->allianceid)
        return DEF_ALLIANCE;

    if (c1->allianceid <= 0 || c2->allianceid <= 0)
        return DEF_NORELATION;

    Alliance * a1 = AllianceById(c1->allianceid);
    Alliance * a2 = AllianceById(c2->allianceid);

    if (a1->IsEnemy(c2->allianceid))
        return DEF_ENEMY;
    else if (a1->IsAlly(c2->allianceid))
        return DEF_ALLY;
    else if (a1->IsNeutral(c2->allianceid))
        return DEF_NEUTRAL;
    else
        return DEF_NORELATION;
}

bool comparemembers(stAlliance first, stAlliance second)
{
    if (first.members > second.members)
        return true;
    else
        return false;
}
bool compareprestige(stAlliance first, stAlliance second)
{
    if (first.prestige > second.prestige)
        return true;
    else
        return false;
}
bool comparehonor(stAlliance first, stAlliance second)
{
    if (first.honor > second.honor)
        return true;
    else
        return false;
}

void AllianceMgr::SortAlliances()
{
    m_membersrank.clear();
    m_prestigerank.clear();
    m_honorrank.clear();

    stAlliance stmem;
    stAlliance stpre;
    stAlliance sthon;
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (m_alliances[i])
        {
            sthon.honor = stpre.prestige = 0;
            stmem.id = stpre.id = sthon.id = m_alliances[i]->m_allianceid;
            stmem.members = m_alliances[i]->m_members.size();
            m_alliances[i]->m_citycount = 0;
            for (Alliance::stMember & member : m_alliances[i]->m_members)
            {
                Client * client = spitfire::GetSingleton().GetClient(member.clientid);
                if (client)
                {
                    sthon.honor += client->honor;
                    stpre.prestige += client->prestige;
                    m_alliances[i]->m_citycount += client->citycount;
                }
            }
            stmem.ref = m_alliances[i];
            stpre.ref = m_alliances[i];
            sthon.ref = m_alliances[i];
            m_membersrank.push_back(stmem);
            m_prestigerank.push_back(stpre);
            m_honorrank.push_back(sthon);
        }
    }

    m_membersrank.sort(compareprestige);
    m_membersrank.sort(comparemembers);
    m_prestigerank.sort(compareprestige);
    m_honorrank.sort(comparehonor);


    int num = 1;
    for (stAlliance & alliance : m_membersrank)
    {
        alliance.rank = alliance.ref->m_membersrank = num++;
    }
    num = 1;
    for (stAlliance & alliance : m_prestigerank)
    {
        alliance.rank = alliance.ref->m_prestigerank = num++;
        alliance.ref->m_prestige = alliance.prestige;
    }
    num = 1;
    for (stAlliance & alliance : m_honorrank)
    {
        alliance.rank = alliance.ref->m_honorrank = num++;
        alliance.ref->m_honor = alliance.honor;
    }
}

Alliance * AllianceMgr::AllianceById(uint64_t id)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (m_alliances[i] && m_alliances[i]->m_allianceid == id)
        {
            return m_alliances[i];
        }
    }
    return (Alliance*)-1;
}

Alliance * AllianceMgr::AllianceByName(std::string name)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (m_alliances[i] && m_alliances[i]->m_name == name)
        {
            return m_alliances[i];
        }
    }
    return (Alliance*)-1;
}


/*
struct stHero
{
int32_t m_id;
int16_t m_level;
int8_t m_loyalty;
int16_t m_management;
int16_t m_managementbuffadded;
int16_t m_power;
int16_t m_powerbuffadded;
int16_t m_stratagem;
int16_t m_stratagembuffadded;
int8_t m_status;

};
struct stResources
{
double food;
double stone;
double iron;
double wood;
double gold;
};
struct stForts
{
int32_t traps;
int32_t abatis;
int32_t towers;
int32_t logs;
int32_t trebs;
};
struct stTroops
{
int32_t worker;
int32_t warrior;
int32_t scout;
int32_t pike;
int32_t sword;
int32_t archer;
int32_t cavalry;
int32_t cataphract;
int32_t transporter;
int32_t ballista;
int32_t ram;
int32_t catapult;
};
struct stResult
{
stResources resources;
bool attackerwon;
};
struct stBuff
{
string id;
double endtime;
} m_buffs;
struct stResearch
{
int16_t level;
double endtime;
} m_research;
struct stAttacker
{
stTroops troops;
stBuff ** buffs;
stResearch ** techs;
PlayerCity * city;
stHero hero;
};
struct stDefender
{
stTroops troops;
stForts forts;
stBuff ** buffs;
stResearch ** techs;
PlayerCity * city;
stHero hero;
};
stResult * CalculateBattle(stAttacker & attacker, stDefender & defender)
{

}*/
