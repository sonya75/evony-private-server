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

Hero::Hero(): client(nullptr), castleid(0), ownerid(0), basemanagement(0), basepower(0), basestratagem(0)
{
    experience = upgradeexp = 0;
    id = 0;
    itemamount = itemid = 0;
    level = remainpoint = 0;
    stratagem = stratagemadded = stratagembuffadded = 0;
    power = poweradded = powerbuffadded = 0;
    management = managementadded = managementbuffadded = 0;
    status = loyalty = 0;
    name = "";
    logourl = "";
    movement = 0;
}

Hero::~Hero()
{

}

bool Hero::InsertToDB()
{
    using HeroSave = Poco::Tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, double, double, int32_t, int32_t, uint32_t, std::string, int8_t>;


    HeroSave savedata(basemanagement, basepower, basestratagem, power, poweradded, powerbuffadded,
        stratagem, stratagemadded, stratagembuffadded, management, managementadded, managementbuffadded,
        experience, upgradeexp, itemamount, itemid, level, logourl, loyalty);


    std::string troop;
    //stArmyMovement * movement;
    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "INSERT INTO `heroes` (basemanagement,basepower,basestratagem,power,poweradded,powerbuffadded,"
            "stratagem,stratagemadded,stratagembuffadded,management,managementadded,managementbuffadded,"
            "experience,upgradeexp,itemamount,itemid,level,logurl,loyalty,troop,name,castleid,ownerid,status,remainpoint,id) "
            "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);", use(savedata), use(troop), use(name), use(castleid), use(ownerid), use(status), use(remainpoint), use(id), now;
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
        ses << "DELETE FROM `heroes` WHERE id=?;", use(id), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

bool Hero::SaveToDB()
{
    using HeroSave = Poco::Tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, double, double, int32_t, int32_t, uint32_t, std::string, int8_t>;


    HeroSave savedata(basemanagement, basepower, basestratagem, power, poweradded, powerbuffadded,
        stratagem, stratagemadded, stratagembuffadded, management, managementadded, managementbuffadded,
        experience, upgradeexp, itemamount, itemid, level, logourl, loyalty);


    std::string troop;
    //stArmyMovement * movement;
    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "UPDATE `heroes` SET basemanagement=?,basepower=?,basestratagem=?,power=?,poweradded=?,powerbuffadded=?,"
            "stratagem=?,stratagemadded=?,stratagembuffadded=?,management=?,managementadded=?,managementbuffadded=?,"
            "experience=?,upgradeexp=?,itemamount=?,itemid=?,level=?,logurl=?,loyalty=?,troop=?,name=?,castleid=?,ownerid=?,status=?,remainpoint=? WHERE id=?;", use(savedata), use(troop), use(name), use(castleid), use(ownerid), use(status), use(remainpoint), use(id), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

amf3object Hero::ToObject() const
{
    amf3object obj = amf3object();
    obj["experience"] = experience;
    obj["id"] = id;
    obj["itemAmount"] = itemamount;
    //obj["itemId"] = itemid;
    obj["level"] = level;
    obj["logoUrl"] = logourl;
    obj["loyalty"] = loyalty;
    obj["management"] = management;
    obj["managementAdded"] = managementadded;
    obj["managementBuffAdded"] = managementbuffadded;
    obj["name"] = name;
    obj["power"] = power;
    obj["powerAdded"] = poweradded;
    obj["powerBuffAdded"] = powerbuffadded;
    obj["remainPoint"] = remainpoint;
    obj["status"] = status;
    obj["stratagem"] = stratagem;
    obj["stratagemAdded"] = stratagemadded;
    obj["stratagemBuffAdded"] = stratagembuffadded;
    obj["upgradeExp"] = upgradeexp;
    return obj;
}

