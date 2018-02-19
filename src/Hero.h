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

    double m_experience;
    Client * m_client;
    uint64_t m_castleid;
    uint64_t m_ownerid;
    uint64_t m_id;
    uint32_t m_itemamount; //?
    uint32_t m_itemid; //?
    uint32_t m_level;
    std::string m_logourl;
    uint8_t m_loyalty;
    uint32_t m_basemanagement;
    uint32_t m_basepower;
    uint32_t m_basestratagem;
    uint32_t m_management;
    uint32_t m_managementadded;
    uint32_t m_managementbuffadded;
    std::string m_name;
    uint32_t m_power;
    uint32_t m_poweradded;
    uint32_t m_powerbuffadded;
    uint32_t m_remainpoint;
    int8_t m_status;
    uint32_t m_stratagem;
    uint32_t m_stratagemadded;
    uint32_t m_stratagembuffadded;
    double m_upgradeexp;
    stArmyMovement * movement;

    bool SaveToDB();
    bool InsertToDB();
    bool DeleteFromDB();

    inline int16_t GetManagement() const
    {
        return m_management + m_managementadded + m_managementbuffadded;
    }
    inline int16_t GetPower() const
    {
        return m_power + m_poweradded + m_powerbuffadded;
    }
    inline int16_t GetStratagem() const
    {
        return m_stratagem + m_stratagemadded + m_stratagembuffadded;
    }
    //array of buffs
}; // 75 bytes
