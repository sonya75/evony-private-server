/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <string.h>
#include "structs.h"
#include "defines.h"


class spitfire;
class Alliance;
class Client;

#define DEF_MAXALLIANCES 1000

class AllianceMgr
{
public:
    AllianceMgr();
    ~AllianceMgr();
    int16_t GetRelation(int64_t client1, int64_t client2);
    Alliance * CreateAlliance(std::string name, std::string founder, int64_t allianceid = 0, bool savetodb = true);
    void DeleteAlliance(Alliance * alliance);
    void CheckAlliance(Alliance * alliance);

    bool CheckName(std::string name);

    static std::string GetAllianceRank(int16_t rank)
    {
        if (rank == DEF_ALLIANCEHOST)
        {
            return "Host";
        }
        else if (rank == DEF_ALLIANCEVICEHOST)
        {
            return "Vice Host";
        }
        else if (rank == DEF_ALLIANCEPRES)
        {
            return "Presbyter";
        }
        else if (rank == DEF_ALLIANCEOFFICER)
        {
            return "Officer";
        }
        else if (rank == DEF_ALLIANCEMEMBER)
        {
            return "Member";
        }
        return "No Rank";
    };
    //     string GetAllianceRank(int16_t rank)
    //     {
    //         if (rank == 1)
    //         {
    //             return "Leader";
    //         }
    //         else if (rank == 2)
    //         {
    //             return "Vice Host";
    //         }
    //         else if (rank == 3)
    //         {
    //             return "Presbyter";
    //         }
    //         else if (rank == 4)
    //         {
    //             return "Officer";
    //         }
    //         else if (rank == 5)
    //         {
    //             return "Member";
    //         }
    //         return "No Rank";
    //     };

    void SortAlliances();
    bool JoinAlliance(uint64_t allianceid, Client * client);
    bool RemoveFromAlliance(uint64_t allianceid, Client * client);
    bool SetRank(uint64_t allianceid, Client * client, int8_t rank);

    Alliance * AllianceById(uint64_t id);
    Alliance * AllianceByName(std::string name);

    Alliance * m_alliances[DEF_MAXALLIANCES];

    std::list<stAlliance> m_membersrank;
    std::list<stAlliance> m_prestigerank;
    std::list<stAlliance> m_honorrank;
};

