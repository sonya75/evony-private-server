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
        alliances[i] = nullptr;
    }
}

AllianceMgr::~AllianceMgr()
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (alliances[i])
            delete alliances[i];
    }
}

void AllianceMgr::DeleteAlliance(Alliance * alliance)
{
    int32_t found = -1;
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (alliances[i])
        {
            if (alliances[i]->allianceid != alliance->allianceid)
            {
                if (alliances[i]->IsAlly(alliance->allianceid))
                    alliances[i]->UnAlly(alliance->allianceid);
                if (alliances[i]->IsNeutral(alliance->allianceid))
                    alliances[i]->UnNeutral(alliance->allianceid);
                if (alliances[i]->IsEnemy(alliance->allianceid))
                    alliances[i]->UnEnemy(alliance->allianceid);


                for (Client * client : spitfire::GetSingleton().players)
                {
                    if (client)
                    {
                        if ((client->allianceapply == alliance->name))
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
    alliance->invites.clear();
    alliance->DeleteFromDB();
    delete alliances[found];
    alliances[found] = nullptr;
    SortAlliances();
}

void AllianceMgr::CheckAlliance(Alliance * alliance)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (alliances[i] != 0)
        {
            if (alliances[i]->members.size() == 0)
            {
                //no more people left in alliance
                DeleteAlliance(alliances[i]);
            }
        }
    }
}

Alliance * AllianceMgr::CreateAlliance(std::string name, std::string founder, int64_t allianceid, bool savetodb)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (alliances[i] == nullptr)
        {
            alliances[i] = new Alliance(name, founder);
            if (allianceid == 0)
                alliances[i]->allianceid = spitfire::GetSingleton().allianceid++;
            else
                alliances[i]->allianceid = allianceid;
            if (savetodb)
                alliances[i]->InsertToDB();
            return alliances[i];
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
        client->allianceid = alliance->allianceid;
        client->alliancename = alliance->name;
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


    for (Alliance::stMember & member : alliance->members)
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
    membersrank.clear();
    prestigerank.clear();
    honorrank.clear();

    stAlliance stmem;
    stAlliance stpre;
    stAlliance sthon;
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (alliances[i])
        {
            sthon.honor = stpre.prestige = 0;
            stmem.id = stpre.id = sthon.id = alliances[i]->allianceid;
            stmem.members = alliances[i]->members.size();
            alliances[i]->citycount = 0;
            for (Alliance::stMember & member : alliances[i]->members)
            {
                Client * client = spitfire::GetSingleton().GetClient(member.clientid);
                if (client)
                {
                    sthon.honor += client->honor;
                    stpre.prestige += client->prestige;
                    alliances[i]->citycount += client->citycount;
                }
            }
            stmem.ref = alliances[i];
            stpre.ref = alliances[i];
            sthon.ref = alliances[i];
            membersrank.push_back(stmem);
            prestigerank.push_back(stpre);
            honorrank.push_back(sthon);
        }
    }

    membersrank.sort(compareprestige);
    membersrank.sort(comparemembers);
    prestigerank.sort(compareprestige);
    honorrank.sort(comparehonor);


    int num = 1;
    for (stAlliance & alliance : membersrank)
    {
        alliance.rank = alliance.ref->membersrank = num++;
    }
    num = 1;
    for (stAlliance & alliance : prestigerank)
    {
        alliance.rank = alliance.ref->prestigerank = num++;
        alliance.ref->prestige = alliance.prestige;
    }
    num = 1;
    for (stAlliance & alliance : honorrank)
    {
        alliance.rank = alliance.ref->honorrank = num++;
        alliance.ref->honor = alliance.honor;
    }
}

Alliance * AllianceMgr::AllianceById(uint64_t id)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (alliances[i] && alliances[i]->allianceid == id)
        {
            return alliances[i];
        }
    }
    return (Alliance*)-1;
}

Alliance * AllianceMgr::AllianceByName(std::string name)
{
    for (int i = 0; i < DEF_MAXALLIANCES; ++i)
    {
        if (alliances[i] && alliances[i]->name == name)
        {
            return alliances[i];
        }
    }
    return (Alliance*)-1;
}


/*
struct stHero
{
int32_t id;
int16_t level;
int8_t loyalty;
int16_t management;
int16_t managementbuffadded;
int16_t power;
int16_t powerbuffadded;
int16_t stratagem;
int16_t stratagembuffadded;
int8_t status;

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
} buffs;
struct stResearch
{
int16_t level;
double endtime;
} research;
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
