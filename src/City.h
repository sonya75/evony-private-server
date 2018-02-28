/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "structs.h"


class Client;


class City
{
public:
    City();
    ~City();

    bool SetBuilding(int16_t type, int8_t level, int16_t position, int16_t status = 0, double starttime = 0, double endtime = 0);
    void SetResources(double gold, double food, double wood, double stone, double iron);
    void SetMaxResources(double gold, double food, double wood, double stone, double iron);
    void SetForts(int32_t traps, int32_t abatis, int32_t towers, int32_t logs, int32_t trebs);
    int32_t GetForts(int32_t type) const;

    std::string cityname;
    int8_t status;
    int32_t tileid;
    int8_t type;
    int8_t level;

    int8_t loyalty;
    int8_t grievance;

    stResources resources;
    stResources maxresources;

    stForts forts;
    
    stBuilding innerbuildings[35]; // 735 bytes
    stBuilding outerbuildings[41]; // 840 bytes
                                     // 
                                     //     public bool npc = true;
                                     //     public Client client = null;
                                     //     public int herocount = 0;
                                     //     public ArrayList heroes = new ArrayList();
                                     //     public bool allowAlliance = false;
                                     //     //public ArrayList buildings = new ArrayList();
                                     //     public Building[] buildings;
};
