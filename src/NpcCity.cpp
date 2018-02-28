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

NpcCity::NpcCity(): temphero(nullptr), calculatestuff(0), ownerid(0)
{
    maxresources.food = maxresources.wood = maxresources.gold = maxresources.iron = maxresources.stone = 0;
    resources.food = resources.wood = resources.gold = resources.iron = resources.stone = 0;
    //     calculatestuff = unixtime();
    //     ownerid = 0;
    //     memset(&troops, 0, sizeof(troops));
    //     temphero.name = "Test NPC Hero";
}
void NpcCity::SetTroops(int32_t warrior, int32_t pike, int32_t sword, int32_t archer, int32_t cavalry)
{
    troops.warrior = warrior;
    troops.pike = pike;
    troops.sword = sword;
    troops.archer = archer;
    troops.cavalry = cavalry;
}
void NpcCity::SetupBuildings()
{
    SetBuilding(4, level, 1011 + ((level - 1) * 3), 0, 0, 0);
    SetBuilding(5, level, 1012 + ((level - 1) * 3), 0, 0, 0);
    SetBuilding(6, level, 1013 + ((level - 1) * 3), 0, 0, 0);
    for (int i = 0; i < 10 + (level - 1) * 3; ++i)
    {
        SetBuilding(7, level, 1001 + i, 0, 0, 0);
    }
    SetBuilding(25, level, 0, 0, 0, 0);
    SetBuilding(22, level, 1, 0, 0, 0);
    SetBuilding(28, level, 2, 0, 0, 0);
    SetBuilding(27, level, 3, 0, 0, 0);
    SetBuilding(23, level, 4, 0, 0, 0);
    SetBuilding(30, level, 5, 0, 0, 0);
    SetBuilding(20, level, 6, 0, 0, 0);

    SetBuilding(21, level, 7, 0, 0, 0);
    SetBuilding(29, level, 8, 0, 0, 0);
    SetBuilding(24, level, 9, 0, 0, 0);
    SetBuilding(26, level, 10, 0, 0, 0);
    SetBuilding(2, level, 11, 0, 0, 0);
    for (int i = 12; i < 32; ++i)
    {
        SetBuilding(1, level, i, 0, 0, 0);
    }

    SetBuilding(31, level, -1, 0, 0, 0);
    SetBuilding(32, level, -2, 0, 0, 0);
}

void NpcCity::Initialize(bool resources, bool troops)
{
    switch (level)
    {
    case 1:
        SetResources(55000, 100000, 20000, 20000, 20000);
        SetMaxResources(55000, 100000, 20000, 20000, 20000);
        SetTroops(50, 40, 30, 10, 8);
        SetForts(1000, 0, 0, 0, 0);
        SetupBuildings();
        break;
    case 2:
        SetResources(65000, 200000, 30000, 30000, 30000);
        SetMaxResources(65000, 200000, 30000, 30000, 30000);
        SetTroops(50, 45, 40, 30, 25);
        SetForts(1850, 550, 0, 0, 0);
        SetupBuildings();
        break;
    case 3:
        SetResources(75000, 900000, 75000, 75000, 75000);
        SetMaxResources(75000, 900000, 75000, 75000, 75000);
        SetTroops(200, 160, 65, 40, 60);
        SetForts(2000, 1000, 650, 0, 0);
        SetupBuildings();
        break;
    case 4:
        SetResources(300000, 1600000, 120000, 120000, 120000);
        SetMaxResources(300000, 1600000, 120000, 120000, 120000);
        SetTroops(400, 400, 100, 100, 150);
        SetForts(4500, 1875, 550, 0, 0);
        SetupBuildings();
        break;
    case 5:
        SetResources(450000, 3000000, 180000, 180000, 180000);
        SetMaxResources(450000, 3000000, 180000, 180000, 180000);
        SetTroops(750, 1000, 350, 250, 200);
        SetForts(3750, 1875, 1250, 750, 0);
        SetupBuildings();
        break;
    case 6:
        SetResources(600000, 4000000, 200000, 200000, 200000);
        SetMaxResources(600000, 4000000, 200000, 200000, 200000);
        SetTroops(4000, 1750, 550, 500, 450);
        SetForts(4250, 1500, 1500, 950, 400);
        SetupBuildings();
        break;
    case 7:
        SetResources(800000, 4500000, 500000, 500000, 500000);
        SetMaxResources(800000, 4500000, 500000, 500000, 500000);
        SetTroops(12000, 3000, 750, 800, 750);
        SetForts(5600, 2800, 1850, 1100, 700);
        SetupBuildings();
        break;
    case 8:
        SetResources(1000000, 8000000, 800000, 800000, 800000);
        SetMaxResources(1000000, 8000000, 800000, 800000, 800000);
        SetTroops(15000, 6750, 4000, 3000, 2000);
        SetForts(7200, 3600, 2400, 1440, 900);
        SetupBuildings();
        break;
    case 9:
        SetResources(1200000, 14000000, 550000, 550000, 550000);
        SetMaxResources(1200000, 14000000, 550000, 550000, 550000);
        SetTroops(60000, 18000, 2000, 6750, 2500);
        SetForts(9000, 4500, 3000, 1800, 1125);
        SetupBuildings();
        break;
    case 10:
        SetResources(1500000, 19000000, 600000, 600000, 600000);
        SetMaxResources(1500000, 19000000, 600000, 600000, 600000);
        SetTroops(400000, 0, 0, 0, 0);
        SetForts(11000, 5500, 3666, 2200, 1375);
        SetupBuildings();
        break;
    default:
        SetResources(55000, 100000, 20000, 20000, 20000);
        SetMaxResources(55000, 100000, 20000, 20000, 20000);
        SetTroops(50, 40, 30, 10, 8);
        SetForts(1000, 0, 0, 0, 0);
        SetupBuildings();
        break;
    }
    //     memcpy(&maxresources, &resources, sizeof(stResources));
    //     memcpy(&maxtroops, &troops, sizeof(stTroops));
    //     memcpy(&maxforts, &forts, sizeof(stForts));
}

void NpcCity::CalculateStats(bool resources, bool troops)
{
    double add;
    if (resources)
    {
        add = maxresources.food / 80;
        if (maxresources.food > (resources.food + add))
            resources.food += add;
        else
            resources.food = maxresources.food;

        add = maxresources.wood / 80;
        if (maxresources.wood > (resources.wood + add))
            resources.wood += add;
        else
            resources.wood = maxresources.wood;

        add = maxresources.stone / 80;
        if (maxresources.stone > (resources.stone + add))
            resources.stone += add;
        else
            resources.stone = maxresources.stone;

        add = maxresources.iron / 80;
        if (maxresources.iron > (resources.iron + add))
            resources.iron += add;
        else
            resources.iron = maxresources.iron;

        //         add = maxresources.gold/80;
        //         if (maxresources.gold > (resources.gold + add))
        //             resources.gold += add;
        //         else
        //             resources.gold = maxresources.gold;
    }
    if (troops)
    {
        add = maxtroops.archer / 10;
        if (maxtroops.archer > (troops.archer + add))
            troops.archer += add;
        else
            troops.archer = maxtroops.archer;

        add = maxtroops.cavalry / 10;
        if (maxtroops.cavalry > (troops.cavalry + add))
            troops.cavalry += add;
        else
            troops.cavalry = maxtroops.cavalry;

        add = maxtroops.pike / 10;
        if (maxtroops.pike > (troops.pike + add))
            troops.pike += add;
        else
            troops.pike = maxtroops.pike;

        add = maxtroops.sword / 10;
        if (maxtroops.sword > (troops.sword + add))
            troops.sword += add;
        else
            troops.sword = maxtroops.sword;

        add = maxtroops.warrior / 10;
        if (maxtroops.warrior > (troops.warrior + add))
            troops.warrior += add;
        else
            troops.warrior = maxtroops.warrior;

        add = maxforts.abatis / 10;
        if (maxforts.abatis > (forts.abatis + add))
            forts.abatis += add;
        else
            forts.abatis = maxforts.abatis;

        add = maxforts.logs / 10;
        if (maxforts.logs > (forts.logs + add))
            forts.logs += add;
        else
            forts.logs = maxforts.logs;

        add = maxforts.towers / 10;
        if (maxforts.towers > (forts.towers + add))
            forts.towers += add;
        else
            forts.towers = maxforts.towers;

        add = maxforts.traps / 10;
        if (maxforts.traps > (forts.traps + add))
            forts.traps += add;
        else
            forts.traps = maxforts.traps;

        add = maxforts.trebs / 10;
        if (maxforts.trebs > (forts.trebs + add))
            forts.trebs += add;
        else
            forts.trebs = maxforts.trebs;

    }
}

NpcCity::~NpcCity(void)
{
}
