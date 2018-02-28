/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <string>
#include "structs.h"


class Client;

class Hero
{
public:
    Hero();
    ~Hero();
    amf3object ToObject() const;

    double experience;
    Client * client;
    uint64_t castleid;
    uint64_t ownerid;
    uint64_t id;
    uint32_t itemamount; //?
    uint32_t itemid; //?
    uint32_t level;
    std::string logourl;
    uint8_t loyalty;
    uint32_t basemanagement;
    uint32_t basepower;
    uint32_t basestratagem;
    uint32_t management;
    uint32_t managementadded;
    uint32_t managementbuffadded;
    std::string name;
    uint32_t power;
    uint32_t poweradded;
    uint32_t powerbuffadded;
    uint32_t remainpoint;
    int8_t status;
    uint32_t stratagem;
    uint32_t stratagemadded;
    uint32_t stratagembuffadded;
    double upgradeexp;
    stArmyMovement * movement;

    bool SaveToDB();
    bool InsertToDB();
    bool DeleteFromDB();

    inline int16_t GetManagement() const
    {
        return management + managementadded + managementbuffadded;
    }
    inline int16_t GetPower() const
    {
        return power + poweradded + powerbuffadded;
    }
    inline int16_t GetStratagem() const
    {
        return stratagem + stratagemadded + stratagembuffadded;
    }
    //array of buffs
}; // 75 bytes
