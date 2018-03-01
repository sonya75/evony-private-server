/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "NpcCity.h"
#include "Client.h"
#include "spitfire.h"
#include "Hero.h"
#include "defines.h"

NpcCity::NpcCity(): m_temphero(nullptr), m_calculatestuff(0), m_ownerid(0)
{
    m_maxresources.food = m_maxresources.wood = m_maxresources.gold = m_maxresources.iron = m_maxresources.stone = 0;
    m_resources.food = m_resources.wood = m_resources.gold = m_resources.iron = m_resources.stone = m_population = 0;
    //     m_calculatestuff = unixtime();
    //     m_ownerid = 0;
    //     memset(&m_troops, 0, sizeof(m_troops));
    //     m_temphero.m_name = "Test NPC Hero";
}
int32_t NpcCity::IDLE_POPULATIONS[10] = { 1000,4000,8000,16000,25000,33000,38000,58000,68000,78000 };
void NpcCity::SetTroops(int32_t warrior, int32_t pike, int32_t sword, int32_t archer, int32_t cavalry)
{
    m_troops.warrior = warrior;
    m_troops.pike = pike;
    m_troops.sword = sword;
    m_troops.archer = archer;
    m_troops.cavalry = cavalry;
}
void NpcCity::SetMaxTroops(int32_t warrior, int32_t pike, int32_t sword, int32_t archer, int32_t cavalry)
{
    m_maxtroops.warrior = warrior;
    m_maxtroops.pike = pike;
    m_maxtroops.sword = sword;
    m_maxtroops.archer = archer;
    m_maxtroops.cavalry = cavalry;
}
void NpcCity::SetMaxForts(int32_t traps, int32_t abatis, int32_t towers, int32_t logs, int32_t trebs)
{
    m_maxforts.traps = traps;
    m_maxforts.abatis = abatis;
    m_maxforts.towers = towers;
    m_maxforts.logs = logs;
    m_maxforts.trebs = trebs;
}
void NpcCity::SetupBuildings()
{
    SetBuilding(4, m_level, 1011 + ((m_level - 1) * 3), 0, 0, 0);
    SetBuilding(5, m_level, 1012 + ((m_level - 1) * 3), 0, 0, 0);
    SetBuilding(6, m_level, 1013 + ((m_level - 1) * 3), 0, 0, 0);
    for (int i = 0; i < 10 + (m_level - 1) * 3; ++i)
    {
        SetBuilding(7, m_level, 1001 + i, 0, 0, 0);
    }
    SetBuilding(25, m_level, 0, 0, 0, 0);
    SetBuilding(22, m_level, 1, 0, 0, 0);
    SetBuilding(28, m_level, 2, 0, 0, 0);
    SetBuilding(27, m_level, 3, 0, 0, 0);
    SetBuilding(23, m_level, 4, 0, 0, 0);
    SetBuilding(30, m_level, 5, 0, 0, 0);
    SetBuilding(20, m_level, 6, 0, 0, 0);

    SetBuilding(21, m_level, 7, 0, 0, 0);
    SetBuilding(29, m_level, 8, 0, 0, 0);
    SetBuilding(24, m_level, 9, 0, 0, 0);
    SetBuilding(26, m_level, 10, 0, 0, 0);
    SetBuilding(2, m_level, 11, 0, 0, 0);
    for (int i = 12; i < 32; ++i)
    {
        SetBuilding(1, m_level, i, 0, 0, 0);
    }

    SetBuilding(31, m_level, -1, 0, 0, 0);
    SetBuilding(32, m_level, -2, 0, 0, 0);
}

void NpcCity::Initialize(bool resources, bool troops)
{
    switch (m_level)
    {
    case 1:
        SetResources(55000, 100000, 20000, 20000, 20000);
        SetMaxResources(55000, 100000, 20000, 20000, 20000);
        SetTroops(50, 40, 30, 10, 8);
        SetMaxTroops(50, 40, 30, 10, 8);
        SetForts(1000, 0, 0, 0, 0);
        SetMaxForts(1000, 0, 0, 0, 0);
        SetupBuildings();
        break;
    case 2:
        SetResources(65000, 200000, 30000, 30000, 30000);
        SetMaxResources(65000, 200000, 30000, 30000, 30000);
        SetTroops(50, 45, 40, 30, 25);
        SetMaxTroops(50, 45, 40, 30, 25);
        SetForts(1850, 550, 0, 0, 0);
        SetMaxForts(1850, 550, 0, 0, 0);
        SetupBuildings();
        break;
    case 3:
        SetResources(75000, 900000, 75000, 75000, 75000);
        SetMaxResources(75000, 900000, 75000, 75000, 75000);
        SetTroops(200, 160, 65, 40, 60);
        SetMaxTroops(200, 160, 65, 40, 60);
        SetForts(2000, 1000, 650, 0, 0);
        SetMaxForts(2000, 1000, 650, 0, 0);
        SetupBuildings();
        break;
    case 4:
        SetResources(300000, 1600000, 120000, 120000, 120000);
        SetMaxResources(300000, 1600000, 120000, 120000, 120000);
        SetTroops(400, 400, 100, 100, 150);
        SetMaxTroops(400, 400, 100, 100, 150);
        SetForts(4500, 1875, 550, 0, 0);
        SetMaxForts(4500, 1875, 550, 0, 0);
        SetupBuildings();
        break;
    case 5:
        SetResources(450000, 3000000, 180000, 180000, 180000);
        SetMaxResources(450000, 3000000, 180000, 180000, 180000);
        SetTroops(750, 1000, 350, 250, 200);
        SetMaxTroops(750, 1000, 350, 250, 200);
        SetForts(3750, 1875, 1250, 750, 0);
        SetMaxForts(3750, 1875, 1250, 750, 0);
        SetupBuildings();
        break;
    case 6:
        SetResources(600000, 4000000, 200000, 200000, 200000);
        SetMaxResources(600000, 4000000, 200000, 200000, 200000);
        SetTroops(4000, 1750, 550, 500, 450);
        SetMaxTroops(4000, 1750, 550, 500, 450);
        SetForts(4250, 1500, 1500, 950, 400);
        SetMaxForts(4250, 1500, 1500, 950, 400);
        SetupBuildings();
        break;
    case 7:
        SetResources(800000, 4500000, 500000, 500000, 500000);
        SetMaxResources(800000, 4500000, 500000, 500000, 500000);
        SetTroops(12000, 3000, 750, 800, 750);
        SetMaxTroops(12000, 3000, 750, 800, 750);
        SetForts(5600, 2800, 1850, 1100, 700);
        SetMaxForts(5600, 2800, 1850, 1100, 700);
        SetupBuildings();
        break;
    case 8:
        SetResources(1000000, 8000000, 800000, 800000, 800000);
        SetMaxResources(1000000, 8000000, 800000, 800000, 800000);
        SetTroops(15000, 6750, 4000, 3000, 2000);
        SetMaxTroops(15000, 6750, 4000, 3000, 2000);
        SetForts(7200, 3600, 2400, 1440, 900);
        SetMaxForts(7200, 3600, 2400, 1440, 900);
        SetupBuildings();
        break;
    case 9:
        SetResources(1200000, 14000000, 550000, 550000, 550000);
        SetMaxResources(1200000, 14000000, 550000, 550000, 550000);
        SetTroops(60000, 18000, 2000, 6750, 2500);
        SetMaxTroops(60000, 18000, 2000, 6750, 2500);
        SetForts(9000, 4500, 3000, 1800, 1125);
        SetMaxForts(9000, 4500, 3000, 1800, 1125);
        SetupBuildings();
        break;
    case 10:
        SetResources(1500000, 19000000, 600000, 600000, 600000);
        SetMaxResources(1500000, 19000000, 600000, 600000, 600000);
        SetTroops(400000, 0, 0, 0, 0);
        SetMaxTroops(400000, 0, 0, 0, 0);
        SetForts(11000, 5500, 3666, 2200, 1375);
        SetMaxForts(11000, 5500, 3666, 2200, 1375);
        SetupBuildings();
        break;
    default:
        SetResources(55000, 100000, 20000, 20000, 20000);
        SetMaxResources(55000, 100000, 20000, 20000, 20000);
        SetTroops(50, 40, 30, 10, 8);
        SetMaxTroops(50, 40, 30, 10, 8);
        SetForts(1000, 0, 0, 0, 0);
        SetMaxForts(1000, 0, 0, 0, 0);
        SetupBuildings();
        break;
    }
    ResetHero();
    if (m_level > 2) m_loyalty = 90;
    else m_loyalty = 80;
    m_population = IDLE_POPULATIONS[m_level - 1];
    //     memcpy(&m_maxresources, &m_resources, sizeof(stResources));
    //     memcpy(&m_maxtroops, &m_troops, sizeof(stTroops));
    //     memcpy(&m_maxforts, &m_forts, sizeof(stForts));
}

void NpcCity::ResetHero()
{
    Hero* hero;
    if (m_temphero != nullptr) {
        hero = m_temphero;
    }
    else {
        hero = new Hero;
        m_temphero = hero;
    }
    hero->m_level = (rand() % (m_level * 10))+1;
    hero->m_power = 30 + (rand() % 30);
    hero->m_poweradded = hero->m_level;
    hero->m_management = 30 + (rand() % 30);
    hero->m_stratagem = 30 + (rand() % 30);
    hero->m_status = DEF_HEROIDLE;
    hero->m_name = "TestName" + std::to_string(rand()%100);
}

void NpcCity::RecoverFromAttack()
{
    double add;
    add = m_troops.archer * 18 / 100;
    if (m_maxtroops.archer > (m_troops.archer + add))
        m_troops.archer += add;
    else
        m_troops.archer = m_maxtroops.archer;

    add = m_troops.cavalry * 18 / 100;
    if (m_maxtroops.cavalry > (m_troops.cavalry + add))
        m_troops.cavalry += add;
    else
        m_troops.cavalry = m_maxtroops.cavalry;

    add = m_troops.pike * 18 / 100;
    if (m_maxtroops.pike > (m_troops.pike + add))
        m_troops.pike += add;
    else
        m_troops.pike = m_maxtroops.pike;

    add = m_troops.sword * 18 / 100;
    if (m_maxtroops.sword > (m_troops.sword + add))
        m_troops.sword += add;
    else
        m_troops.sword = m_maxtroops.sword;

    add = m_troops.warrior * 18 / 100;
    if (m_maxtroops.warrior > (m_troops.warrior + add))
        m_troops.warrior += add;
    else
        m_troops.warrior = m_maxtroops.warrior;

    add = m_forts.abatis * 18 / 100;
    if (m_maxforts.abatis > (m_forts.abatis + add))
        m_forts.abatis += add;
    else
        m_forts.abatis = m_maxforts.abatis;

    add = m_forts.logs * 18 / 100;
    if (m_maxforts.logs > (m_forts.logs + add))
        m_forts.logs += add;
    else
        m_forts.logs = m_maxforts.logs;

    add = m_forts.towers * 18 / 100;
    if (m_maxforts.towers > (m_forts.towers + add))
        m_forts.towers += add;
    else
        m_forts.towers = m_maxforts.towers;

    add = m_forts.traps * 18 / 100;
    if (m_maxforts.traps > (m_forts.traps + add))
        m_forts.traps += add;
    else
        m_forts.traps = m_maxforts.traps;

    add = m_forts.trebs * 18 / 100;
    if (m_maxforts.trebs > (m_forts.trebs + add))
        m_forts.trebs += add;
    else
        m_forts.trebs = m_maxforts.trebs;
}
void NpcCity::CalculateStats(bool resources, bool troops)
{
    double add;
    if (resources)
    {
        add = m_maxresources.food / 80;
        if (m_maxresources.food > (m_resources.food + add))
            m_resources.food += add;
        else
            m_resources.food = m_maxresources.food;

        add = m_maxresources.wood / 80;
        if (m_maxresources.wood > (m_resources.wood + add))
            m_resources.wood += add;
        else
            m_resources.wood = m_maxresources.wood;

        add = m_maxresources.stone / 80;
        if (m_maxresources.stone > (m_resources.stone + add))
            m_resources.stone += add;
        else
            m_resources.stone = m_maxresources.stone;

        add = m_maxresources.iron / 80;
        if (m_maxresources.iron > (m_resources.iron + add))
            m_resources.iron += add;
        else
            m_resources.iron = m_maxresources.iron;

        //         add = m_maxresources.gold/80;
        //         if (m_maxresources.gold > (m_resources.gold + add))
        //             m_resources.gold += add;
        //         else
        //             m_resources.gold = m_maxresources.gold;
    }
    if (troops)
    {
        add = m_maxtroops.archer / 10;
        if (m_maxtroops.archer > (m_troops.archer + add))
            m_troops.archer += add;
        else
            m_troops.archer = m_maxtroops.archer;

        add = m_maxtroops.cavalry / 10;
        if (m_maxtroops.cavalry > (m_troops.cavalry + add))
            m_troops.cavalry += add;
        else
            m_troops.cavalry = m_maxtroops.cavalry;

        add = m_maxtroops.pike / 10;
        if (m_maxtroops.pike > (m_troops.pike + add))
            m_troops.pike += add;
        else
            m_troops.pike = m_maxtroops.pike;

        add = m_maxtroops.sword / 10;
        if (m_maxtroops.sword > (m_troops.sword + add))
            m_troops.sword += add;
        else
            m_troops.sword = m_maxtroops.sword;

        add = m_maxtroops.warrior / 10;
        if (m_maxtroops.warrior > (m_troops.warrior + add))
            m_troops.warrior += add;
        else
            m_troops.warrior = m_maxtroops.warrior;

        add = m_maxforts.abatis / 10;
        if (m_maxforts.abatis > (m_forts.abatis + add))
            m_forts.abatis += add;
        else
            m_forts.abatis = m_maxforts.abatis;

        add = m_maxforts.logs / 10;
        if (m_maxforts.logs > (m_forts.logs + add))
            m_forts.logs += add;
        else
            m_forts.logs = m_maxforts.logs;

        add = m_maxforts.towers / 10;
        if (m_maxforts.towers > (m_forts.towers + add))
            m_forts.towers += add;
        else
            m_forts.towers = m_maxforts.towers;

        add = m_maxforts.traps / 10;
        if (m_maxforts.traps > (m_forts.traps + add))
            m_forts.traps += add;
        else
            m_forts.traps = m_maxforts.traps;

        add = m_maxforts.trebs / 10;
        if (m_maxforts.trebs > (m_forts.trebs + add))
            m_forts.trebs += add;
        else
            m_forts.trebs = m_maxforts.trebs;
    }
}

NpcCity::~NpcCity(void)
{
}
