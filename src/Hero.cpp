/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "Hero.h"
#include "Client.h"
#include "spitfire.h"
#include "amf3.h"
#include "defines.h"
#include <Poco/Data/MySQL/MySQLException.h>

using namespace Poco::Data::Keywords;
using namespace Poco::Data;

Hero::Hero(): m_client(nullptr), m_castleid(0), m_ownerid(0), m_basemanagement(0), m_basepower(0), m_basestratagem(0)
{
    m_experience = m_upgradeexp = 0;
    m_id = 0;
    m_itemamount = m_itemid = 0;
    m_level = m_remainpoint = 0;
    m_stratagem = m_stratagemadded = m_stratagembuffadded = 0;
    m_power = m_poweradded = m_powerbuffadded = 0;
    m_management = m_managementadded = m_managementbuffadded = 0;
    m_status = m_loyalty = 0;
    m_name = "";
    m_logourl = "";
    movement = 0;
}

Hero::~Hero()
{

}

bool Hero::InsertToDB()
{
    using HeroSave = Poco::Tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, double, double, int32_t, int32_t, uint32_t, std::string, int8_t>;


    HeroSave savedata(m_basemanagement, m_basepower, m_basestratagem, m_power, m_poweradded, m_powerbuffadded,
        m_stratagem, m_stratagemadded, m_stratagembuffadded, m_management, m_managementadded, m_managementbuffadded,
        m_experience, m_upgradeexp, m_itemamount, m_itemid, m_level, m_logourl, m_loyalty);


    std::string troop;
    //stArmyMovement * movement;
    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "INSERT INTO `heroes` (basemanagement,basepower,basestratagem,power,poweradded,powerbuffadded,"
            "stratagem,stratagemadded,stratagembuffadded,management,managementadded,managementbuffadded,"
            "experience,upgradeexp,itemamount,itemid,level,logurl,loyalty,troop,name,castleid,ownerid,status,remainpoint,id) "
            "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);", use(savedata), use(troop), use(m_name), use(m_castleid), use(m_ownerid), use(m_status), use(m_remainpoint), use(m_id), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

bool Hero::DeleteFromDB()
{
    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "DELETE FROM `heroes` WHERE id=?;", use(m_id), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

bool Hero::SaveToDB()
{
    using HeroSave = Poco::Tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, double, double, int32_t, int32_t, uint32_t, std::string, int8_t>;


    HeroSave savedata(m_basemanagement, m_basepower, m_basestratagem, m_power, m_poweradded, m_powerbuffadded,
        m_stratagem, m_stratagemadded, m_stratagembuffadded, m_management, m_managementadded, m_managementbuffadded,
        m_experience, m_upgradeexp, m_itemamount, m_itemid, m_level, m_logourl, m_loyalty);


    std::string troop;
    //stArmyMovement * movement;
    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "UPDATE `heroes` SET basemanagement=?,basepower=?,basestratagem=?,power=?,poweradded=?,powerbuffadded=?,"
            "stratagem=?,stratagemadded=?,stratagembuffadded=?,management=?,managementadded=?,managementbuffadded=?,"
            "experience=?,upgradeexp=?,itemamount=?,itemid=?,level=?,logurl=?,loyalty=?,troop=?,name=?,castleid=?,ownerid=?,status=?,remainpoint=? WHERE id=?;", use(savedata), use(troop), use(m_name), use(m_castleid), use(m_ownerid), use(m_status), use(m_remainpoint), use(m_id), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

amf3object Hero::ToObject() const
{
    amf3object obj = amf3object();
    obj["experience"] = m_experience;
    obj["id"] = m_id;
    obj["itemAmount"] = m_itemamount;
    //obj["itemId"] = m_itemid;
    obj["level"] = m_level;
    obj["logoUrl"] = m_logourl;
    obj["loyalty"] = m_loyalty;
    obj["management"] = m_management;
    obj["managementAdded"] = m_managementadded;
    obj["managementBuffAdded"] = m_managementbuffadded;
    obj["name"] = m_name;
    obj["power"] = m_power;
    obj["powerAdded"] = m_poweradded;
    obj["powerBuffAdded"] = m_powerbuffadded;
    obj["remainPoint"] = m_remainpoint;
    obj["status"] = m_status;
    obj["stratagem"] = m_stratagem;
    obj["stratagemAdded"] = m_stratagemadded;
    obj["stratagemBuffAdded"] = m_stratagembuffadded;
    obj["upgradeExp"] = m_upgradeexp;
    return obj;
}

