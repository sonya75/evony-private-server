/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "City.h"
#include "structs.h"

class NpcCity : public City
{
public:
    NpcCity();
    ~NpcCity(void);
    void Initialize(bool resources, bool troops);
    void SetTroops(int32_t warrior, int32_t pike, int32_t sword, int32_t archer, int32_t cavalry);
    void SetupBuildings();
    void CalculateStats(bool resources, bool troops);

    struct stTroops
    {
        stTroops() { warrior = pike = sword = archer = cavalry = 0; }
        int32_t warrior;
        int32_t pike;
        int32_t sword;
        int32_t archer;
        int32_t cavalry;
    } m_troops, m_maxtroops;

    stForts m_maxforts;

    Hero * m_temphero;

    uint64_t m_calculatestuff;


    int32_t m_ownerid;
};
