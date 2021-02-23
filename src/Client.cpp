/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "Client.h"
#include "Hero.h"
#include "Map.h"

#include "spitfire.h"
#include "Tile.h"
#include "City.h"
// #include "AllianceMgr.h"
// #include "Alliance.h"
#include <Poco/Data/MySQL/MySQLException.h>
// #include <boost/algorithm/string/split.hpp>
// #include <boost/algorithm/string/classification.hpp>

using namespace Poco::Data;
using namespace Poco::Data::Keywords;

#ifndef WIN32
#define _ASSERT(x)
#endif

Client::Client()
{
    honorrank = 0;
    tempvar = 0;
    clientdelay = 0;
    mailpid = 1;
    accountexists = false;
    socknum = 0;
    accountid = 0;
    PACKETSIZE = 1024 * 150;
    socket = nullptr;
    loggedin = false;
    internalid = -1;
    playername = "Testname";
    flag = "Evony";
    alliancename = "";
    email = "";
    password = "";
    prestige = 0;
    honor = 0;
    status = 0;
    rank = 0;
    prestigerank = 0;
    title = 0;
    connected = 0;
    //char * readytoprocess = new char[PACKETSIZE];
    processoffset = 0;
    citycount = 0;
    lastlogin = 0;
    creation = 0;
    sex = 0;
    allianceid = -1;
    alliancerank = 0;
    currentcityindex = -1;
    currentcityid = -1;
    population = 0;
    cents = 0;
    office = "Civilian";
    m_bdenyotherplayer = false;
    masteraccountid = 0;
    icon = 0;
    haschangedface = false;
    beginner = false;
    changeface = 0;//1 = xmas, 2 = halloween maybe?, more?
    currentreportid = 1;
    allianceapplytime = 0;

    currentcityid = currentcityindex = -1;
    //m_city;// = new ArrayList();

    //memset(&m_buffs, 0, sizeof(m_buffs));
//     memset(&research, 0, sizeof(research));
    //memset(&m_items, 0, sizeof(m_items));

//     itemlist.resize(30);
    debugmode = false;
    //     for (int i = 0; i < 30; ++i)
    //     {
    //         m_buffs[i].endtime = 0.0;
    //         m_buffs[i].id = "";
    //     }
}

Client::~Client(void)
{
    //    for (int i = 0; i < m_city.size(); ++i)
    //        delete (City*)m_city.at(i);
}

Alliance * Client::GetAlliance()
{
    return spitfire::GetSingleton().m_alliances->AllianceById(allianceid);
    //return 0;//TODO: FIX
};

std::string Client::DBBuffs()
{
    std::string res;
    std::stringstream ss;
    std::vector<Poco::Any> args;

    for (stBuff buff : bufflist)
    {
        args.push_back(buff.id);
        args.push_back(buff.desc);
        args.push_back(buff.endtime);
        ss << "%?d,%?d,%?f|";
    }
    Poco::format(res, ss.str(), args);
    return res;
}

std::string Client::DBResearch()
{
    std::stringstream ss;

    for (int i = 0; i < 25; ++i)
    {
        ss << i << "," << research[i].level << "," << research[i].castleid << "," << research[i].starttime << "," << research[i].endtime << "|";
    }
    return ss.str();
}

std::string Client::DBItems()
{
    std::string res;
    std::stringstream ss;
    std::vector<Poco::Any> args;

    for (stItem & sitem : itemlist)
    {
        if (sitem.count > 0)
        {
            args.push_back(sitem.id);
            args.push_back(sitem.count);
            ss << "%s,%?d|";
        }
    }
    Poco::format(res, ss.str(), args);
    return res;
}

std::string Client::DBMisc()
{
    std::stringstream ss;
    ss << icon << "," << (haschangedface) ? 1 : 0;
    return ss.str();
}

std::string Client::DBCastleSign()
{
    std::stringstream ss;

    for (stCastleSign castle : castlesignlist)
    {
        ss << castle.tileid << ",";
    }
    return ss.str();
}

bool Client::SaveToDB()
{
    typedef Poco::Tuple<std::string, std::string, std::string, std::string, int32_t, std::string, int32_t, std::string, std::string, int32_t, int16_t, uint64_t, double, double, double, bool, int8_t, std::string, int64_t, std::string> ClientSave;


    ClientSave savedata(DBBuffs(), DBResearch(), DBItems(), DBMisc(), status, ipaddress, sex, flag, faceurl, allianceid, alliancerank, cents, prestige, honor, lastlogin, haschangedface, icon, allianceapply, allianceapplytime, DBCastleSign());


    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "UPDATE `accounts` SET buffs=?,`research`=?,items=?,misc=?,`status`=?,ipaddress=?,sex=?,flag=?,faceurl=?,allianceid=?,alliancerank=?,cents=?,prestige=?,honor=?,lastlogin=?,changedface=?,icon=?,allianceapply=?,allianceapplytime=?,castlesign=? WHERE accountid=?;", use(savedata), use(accountid), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

amf3object  Client::ToObject()
{
    amf3object obj = amf3object();
    int newmailsys = 0;
    int newmailinbox = 0;
    int newmailall = 0;
    for (stMail mail : maillist)
    {
        if (!mail.isread())
        {
            if (mail.type_id == 1)
                newmailinbox++;
            else if (mail.type_id == 2)
                newmailsys++;
            newmailall++;
        }
    }
    int newreporttrade = 0;
    int newreportall = 0;
    int newreportother = 0;
//     for (stMail mail : maillist)
//     {
//         if (!mail.isread())
//         {
//             if (mail.type_id == 2)
//                 newreporttrade++;
//             else if (mail.type_id == 1)
//                 newreportother++;
//             newreportall++;
//         }
//     }
    obj["newReportCount_trade"] = newreporttrade;
    obj["newMaileCount_system"] = newmailsys;
    obj["newReportCount"] = newreportall;
    obj["isSetSecurityCode"] = false;
    obj["mapSizeX"] = spitfire::GetSingleton().mapsize;
    obj["mapSizeY"] = spitfire::GetSingleton().mapsize;
    obj["newReportCount_other"] = newreportother;
    obj["buffs"] = BuffsArray();
    obj["gamblingItemIndex"] = 1;
    obj["changedFace"] = haschangedface;
    obj["castles"] = CastleArray();
    obj["playerInfo"] = PlayerInfo();
    obj["redCount"] = 0;
    obj["usePACIFY_SUCCOUR_OR_PACIFY_PRAY"] = 1;//always 1?
    obj["newMaileCount_inbox"] = newmailinbox;

    std::string s;
    {
        time_t ttime;
        time(&ttime);
        struct tm * timeinfo;
        timeinfo = localtime(&ttime);

        std::stringstream ss;
        ss << (timeinfo->tm_year + 1900) << ".";
        if (timeinfo->tm_mon < 9)
            ss << "0" << (timeinfo->tm_mon + 1);
        else
            ss << (timeinfo->tm_mon + 1);
        ss << ".";
        if (timeinfo->tm_mday < 10)
            ss << "0" << timeinfo->tm_mday;
        else
            ss << timeinfo->tm_mday;
        ss << " ";
        if (timeinfo->tm_hour < 10)
            ss << "0" << timeinfo->tm_hour;
        else
            ss << timeinfo->tm_hour;
        ss << ".";
        if (timeinfo->tm_min < 10)
            ss << "0" << timeinfo->tm_min;
        else
            ss << timeinfo->tm_min;
        ss << ".";
        if (timeinfo->tm_sec < 10)
            ss << "0" << timeinfo->tm_sec;
        else
            ss << timeinfo->tm_sec;

        s = ss.str();
    }

    //    obj["currentDateTime"] = "2011.07.27 03.20.32";
    obj["currentDateTime"] = s.c_str();
    obj["newReportCount_army"] = 0;
    amf3array friendarmys;
    for (stArmyMovement* xo : friendarmymovement) {
        amf3object ox=xo->ToObject();
        friendarmys.Add(ox);
    }
    obj["friendArmys"] = friendarmys;
    obj["saleTypeBeans"] = SaleTypeItems();
    obj["autoFurlough"] = false;
    obj["castleSignBean"] = CastleSignArray();
    obj["furloughDay"] = 0;
    obj["tutorialStepId"] = 0;//10101; -- can set any tutorial
    obj["newReportCount_army"] = 0;
    obj["newMailCount"] = newmailall;
    obj["furlough"] = false;
    obj["gameSpeed"] = 1;
    amf3array enemyarmys;
    for (stArmyMovement* xo : enemyarmymovement) {
        amf3object ox=xo->ToObject();
        enemyarmys.Add(ox);
    }
    obj["enemyArmys"] = enemyarmys;
    obj["currentTime"] = (double)Utils::time();
    obj["items"] = Items();
    obj["freshMan"] = false;
    obj["finishedQuestCount"] = 0;
    amf3array selfarmys;
    for (stArmyMovement* xo : armymovement) {
        amf3object ox=xo->ToObject();
        selfarmys.Add(ox);
    }
    obj["selfArmys"] = selfarmys;
    obj["saleItemBeans"] = SaleItems();

    return obj;
}
amf3array Client::CastleSignArray()
{
    amf3array signarray = amf3array();
    amf3object obj = amf3object();

    for (stCastleSign castle : castlesignlist)
    {
        obj = amf3object();
        obj["id"] = castle.tileid;
        obj["name"] = castle.name;
        obj["y"] = castle.y;
        obj["x"] = castle.x;
        signarray.Add(obj);
    }
    return signarray;
}
amf3array Client::Items()
{
    //    amf3object retobj;
    //amf3array * itemarray = new amf3array();
    amf3array itemarray = amf3array();
    //    retobj.type = Array;
    //    retobj._value._array = itemarray;

    amf3object obj = amf3object();
    //age2?
    //obj["appearance"] = 96;
    //obj["help"] = 1;

    for (stItem & sitem : itemlist)
    {
        if (sitem.count > 0)
        {
            obj = amf3object();
            obj["id"] = sitem.id;
            obj["count"] = sitem.count;
            obj["minCount"] = sitem.mincount;
            obj["maxCount"] = sitem.maxcount;
            itemarray.Add(obj);
        }
    }
    return itemarray;
}
amf3array Client::SaleTypeItems()
{
    amf3array array = amf3array();
    amf3object obj1 = amf3object();
    amf3object obj2 = amf3object();
    amf3object obj3 = amf3object();
    amf3object obj4 = amf3object();
    obj1["typeName"] = "\xE5\x8A\xA0\xE9\x80\x9F";//加速
    obj2["typeName"] = "\xE5\xAE\x9D\xE7\xAE\xB1";//宝箱
    obj3["typeName"] = "\xE7\x94\x9F\xE4\xBA\xA7";//生产
    obj4["typeName"] = "\xE5\xAE\x9D\xE7\x89\xA9";//宝物
    std::stringstream ss1, ss2, ss3, ss4;

    bool count1, count2, count3, count4;
    count1 = count2 = count3 = count4 = false;

    for (int i = 0; i < DEF_MAXITEMS; ++i)
    {
        if (spitfire::GetSingleton().m_items[i].buyable)
        {
            if (spitfire::GetSingleton().m_items[i].type == 1)//speed up tab
            {
                if (count1)
                    ss1 << "#";
                ss1 << spitfire::GetSingleton().m_items[i].name;
                count1 = true;
            }
            else if (spitfire::GetSingleton().m_items[i].type == 2)//chest tab
            {
                if (count2)
                    ss2 << "#";
                ss2 << spitfire::GetSingleton().m_items[i].name;
                count2 = true;
            }
            else if (spitfire::GetSingleton().m_items[i].type == 3)//produce tab
            {
                if (count3)
                    ss3 << "#";
                ss3 << spitfire::GetSingleton().m_items[i].name;
                count3 = true;
            }
            else if (spitfire::GetSingleton().m_items[i].type == 4)//items tab
            {
                if (count4)
                    ss4 << "#";
                ss4 << spitfire::GetSingleton().m_items[i].name;
                count4 = true;
            }
        }
    }
    obj1["items"] = ss1.str();
    obj2["items"] = ss2.str();
    obj3["items"] = ss3.str();
    obj4["items"] = ss4.str();
    array.Add(obj1);
    array.Add(obj2);
    array.Add(obj3);
    array.Add(obj4);

    //    obj["items"] = "consume.2.a#consume.2.b#consume.2.b.1#consume.2.c#consume.2.c.1#consume.2.d#player.fort.1.c#player.troop.1.b#consume.transaction.1";

    //     obj = amf3object();
    //     obj["typeName"] = "\xE5\xAE\x9D\xE7\xAE\xB1";//宝箱
    //     obj["items"] = "player.box.special.1#player.box.special.2#player.box.special.3#player.box.currently.1#player.box.gambling.1#player.box.gambling.2#player.box.gambling.3#player.box.gambling.4#player.box.gambling.5#player.box.gambling.6#player.box.gambling.7#player.box.gambling.8#player.box.gambling.9#player.box.gambling.10#player.box.gambling.11#player.box.gambling.12";
    //     array.Add(obj);
    // 
    //     obj = amf3object();
    //     obj["typeName"] = "\xE7\x94\x9F\xE4\xBA\xA7";//生产
    //     obj["items"] = "player.resinc.1#player.resinc.1.b#player.resinc.2#player.resinc.2.b#player.resinc.3#player.resinc.3.b#player.resinc.4#player.resinc.4.b#player.gold.1.a#player.gold.1.b";
    //     array.Add(obj);
    // 
    //     obj = amf3object();
    //     obj["typeName"] = "\xE5\xAE\x9D\xE7\x89\xA9";//宝物
    //     obj["items"] = "player.speak.bronze_publicity_ambassador.permanent#player.speak.bronze_publicity_ambassador.permanent.15#player.pop.1.a#hero.management.1#player.experience.1.b#player.experience.1.a#player.queue.building#player.experience.1.c#player.peace.1#player.heart.1.a#hero.intelligence.1#consume.blueprint.1#consume.refreshtavern.1#consume.move.1#player.more.castle.1.a#player.name.1.a#consume.changeflag.1#player.troop.1.a#player.attackinc.1#player.attackinc.1.b#consume.hegemony.1#player.defendinc.1#player.defendinc.1.b#player.relive.1#hero.reset.1#hero.reset.1.a#player.destroy.1.a#hero.power.1#consume.1.a#consume.1.b#consume.1.c";
    //     array.Add(obj);
    return array;
}
amf3array Client::SaleItems()
{
    amf3array array = amf3array();
    amf3object obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "player.resinc.1";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "player.resinc.2";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "player.resinc.3";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "player.resinc.4";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 0;
    obj["items"] = "consume.1.c";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "consume.2.b.1";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "player.gold.1.a";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "consume.refreshtavern.1";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "consume.2.b";
    array.Add(obj);

    obj = amf3object();
    obj["saleType"] = 1;
    obj["items"] = "consume.transaction.1";
    array.Add(obj);
    return array;
}
amf3array Client::CastleArray()
{
    //     Amf3Array array = new Amf3Array();
    //     for (int i = 0; i < m_citycount; ++i)
    //         array.DenseArray.Add(((City)m_city[i]).ToObject());
    //     return array;
    amf3array array = amf3array();
    for (int32_t i = 0; i < citycount; ++i)
    {
        amf3object temp = ((PlayerCity*)citylist.at(i))->ToObject();
        array.Add(temp);
    }
    return array;
}
amf3array Client::BuffsArray()
{
    amf3array array = amf3array();
    for (int32_t i = 0; i < bufflist.size(); ++i)
    {
        amf3object temp = amf3object();
        temp["desc"] = bufflist[i].desc;
        temp["endTime"] = bufflist[i].endtime;
        temp["id"] = bufflist[i].id;
        array.Add(temp);
    }
    //     amf3object t = amf3object();
    //     t["id"] = "PlayerSpeakerBuf_BPAP_15";
    //     t["endTime"] = unixtime()+100000;
    //     t["desc"] = 3;
    //     array.Add(t);
    return array;
}
amf3array Client::Packages()
{
    amf3array array = amf3array();
    for (int32_t i = 0; i < packagelist.size(); ++i)
    {
        amf3object temp = amf3object();
        temp["desc"] = packagelist[i].desc;
        temp["packageName"] = packagelist[i].name;
        temp["status"] = packagelist[i].id;
        temp["type"] = packagelist[i].id;
        temp["minMedal"] = packagelist[i].id;
        temp["maxMedal"] = packagelist[i].id;
        temp["id"] = packagelist[i].id;
        temp["isUseNow"] = packagelist[i].id;
        temp["scores"] = packagelist[i].id;
        temp["provideTime"] = packagelist[i].id;
        temp["expiredTime"] = packagelist[i].id;
        temp["claimedTime"] = packagelist[i].id;
        amf3array itemarray = amf3array();
        for (int32_t a = 0; a < packagelist[i].items.size(); ++a)
        {
            amf3object tempitem = amf3object();
            tempitem["id"] = packagelist[i].items[a].id;
            tempitem["count"] = packagelist[i].items[a].count;
            tempitem["name"] = packagelist[i].items[a].name;
            tempitem["minCount"] = packagelist[i].items[a].mincount;
            tempitem["maxCount"] = packagelist[i].items[a].maxcount;
            itemarray.Add(tempitem);
        }
        temp["itemList"] = itemarray;
        array.Add(temp);
    }
    return array;
}
amf3object Client::PlayerInfo()
{
    amf3object obj = amf3object();
    if (allianceid > 0)
    {
        obj["alliance"] = "";// spitfire::GetSingleton().m_alliances->AllianceById(allianceid)->m_name;
        obj["allianceLevel"] = 0;// AllianceMgr::GetAllianceRank(alliancerank);
        obj["levelId"] = alliancerank;
    }
    obj["createrTime"] = creation;//-3*7*24*60*60*1000;
    obj["office"] = office;
    obj["sex"] = sex;
    obj["honor"] = honor;
    obj["bdenyotherplayer"] = m_bdenyotherplayer;
    obj["id"] = accountid;
    obj["accountName"] = email;
    obj["prestige"] = prestige;
    obj["faceUrl"] = faceurl;
    obj["flag"] = flag;
    obj["userId"] = masteraccountid;
    obj["userName"] = playername;
    obj["castleCount"] = citycount;
    obj["medal"] = cents;
    obj["ranking"] = prestigerank;
    obj["titleId"] = title;
    obj["lastLoginTime"] = lastlogin;
    population = 0;

    for (int i = 0; i < citycount; ++i)
        population += ((PlayerCity*)citylist.at(i))->m_population;

    obj["population"] = population;
    return obj;
}

void Client::ParseBuffs(std::string str)
{
    if (str.length() > 0)
    {
        std::vector<std::string> bufftokens;
        std::vector<std::string> tokens;
        my_split(bufftokens, str, "|");
        //boost::split(bufftokens, str, boost::is_any_of("|"));

        for (int i = 0; i < bufftokens.size(); ++i)
        {
            my_split(tokens, bufftokens[i], ",");
            //boost::split(tokens, bufftokens[i], boost::is_any_of(","));

            if (tokens.size() == 3)
                SetBuff(tokens[0], tokens[1], _atoi64(tokens[2].c_str()));
        }
    }
}
void Client::ParseResearch(std::string str)
{
    if (str.length() > 0)
    {
        std::vector<std::string> researchtokens;
        std::vector<std::string> tokens;
        my_split(researchtokens, str, "|");
        //boost::split(researchtokens, str, boost::is_any_of("|"));

        for (int i = 0; i < researchtokens.size(); ++i)
        {
            my_split(tokens, researchtokens[i], ",");
            //boost::split(tokens, researchtokens[i], boost::is_any_of(","));

            if (tokens.size() == 5)
            {
                SetResearch(atoi(tokens[0].c_str()), atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atof(tokens[3].c_str()), atof(tokens[4].c_str()));
            }
        }
    }
}
void Client::ParseItems(std::string str)
{
    if (str.length() > 0)
    {
        std::vector<std::string> itemtokens;
        std::vector<std::string> tokens;
        my_split(itemtokens, str, "|");
        //boost::split(itemtokens, str, boost::is_any_of("|"));

        for (int i = 0; i < itemtokens.size(); ++i)
        {
            my_split(tokens, itemtokens[i], ",");
            //boost::split(tokens, itemtokens[i], boost::is_any_of(","));

            if (tokens.size() == 2)
            {
                if (tokens[0].length() > 0)
                    SetItem(tokens[0], _atoi64(tokens[1].c_str()));
            }
        }
    }
}
void Client::ParseMisc(std::string str)
{
    if (str.length() > 0)
    {
        std::vector<std::string> tokens;
        my_split(tokens, str, ",");
        //boost::split(tokens, str, boost::is_any_of(","));

        if (tokens.size() == 2)
        {
            icon = atoi(tokens[0].c_str());
            haschangedface = (bool)tokens[1].c_str();
        }
        else
        {
            //error in token size
            spitfire::GetSingleton().log->error("Error in '{}' ParseMisc()", playername);
        }
    }
}

void Client::ParseCastleSign(std::string str)
{
    if (str.length() > 0)
    {
        std::vector<std::string> tokens;
        my_split(tokens, str, ",");
        //boost::split(tokens, str, boost::is_any_of(","));

        Tile * tile = nullptr;
        int32_t x = 0;
        int32_t y = 0;


        for (std::string t : tokens)
        {
            stCastleSign cst;
            tile = spitfire::GetSingleton().map->GetTileFromID(atoi(t.c_str()));
            if (tile->m_type == CASTLE)
            {
                cst.id = tile->m_castleid;
                cst.name = tile->GetName();
                short yfromid = short(atoi(t.c_str()) / spitfire::GetSingleton().map->mapsize);
                short xfromid = short(atoi(t.c_str()) % spitfire::GetSingleton().map->mapsize);
                cst.x = GETX;
                cst.y = GETY;

                castlesignlist.push_back(cst);
            }
        }
    }
}

void Client::SetBuff(std::string type, std::string desc, int64_t endtime, int8_t param)
{
    for (int i = 0; i < bufflist.size(); ++i)
    {
        if (bufflist[i].id == type)
        {
            bufflist[i].endtime = endtime;
            BuffUpdate(type, desc, endtime);
            return;
        }
    }
    stBuff buff;
    buff.desc = desc;
    buff.endtime = endtime;
    buff.id = type;

    bufflist.push_back(buff);

    BuffUpdate(type, desc, endtime, param);
    return;
}

void Client::RemoveBuff(std::string type)
{
    std::vector<stBuff>::iterator iter;
    if (bufflist.size() > 0)
        for (iter = bufflist.begin(); iter != bufflist.end();)
        {
            if (iter->id == type)
            {
                BuffUpdate(type, iter->desc, iter->endtime, 1);//1 = remove
                bufflist.erase(iter++);
                return;
            }
            ++iter;
        }
    return;
}

PlayerCity * Client::GetCity(int64_t castleid)
{
    for (int32_t i = 0; i < citycount; ++i)
    {
        if (citylist[i]->m_castleid == castleid)
            return citylist[i];
    }
    return nullptr;
};

PlayerCity * Client::GetFocusCity()
{
    return (PlayerCity*)citylist[currentcityindex];
};

void Client::SetResearch(uint16_t id, int16_t level, int32_t castleid, double starttime, double endtime)
{
    if (id < 0 || id > 24)
        _ASSERT(0);
    research[id].level = level;
    research[id].endtime = endtime;
    research[id].starttime = starttime;
    research[id].castleid = castleid;
}

void Client::AddItem(std::string type, int64_t dir)
{
    if (type == "")
        return;
    for (stItem & sitem : itemlist)
    {
        if (sitem.id == type)
        {
            sitem.count += dir;

            ItemUpdate(type);
            return;
        }
    }

    //should not be able to set an item to negative
    if (dir < 0)
        return;

    stItemConfig * cfg = spitfire::GetSingleton().GetItem(type);

    stItem newitem;
    newitem.id = type;
    newitem.name = cfg->name;
    newitem.count = dir;
    newitem.maxcount = 0;
    newitem.mincount = 0;

    itemlist.push_back(newitem);

    ItemUpdate(type);
}

void Client::SetItem(std::string type, int64_t amount)
{
    if (type == "")
        return;
    for (stItem & sitem : itemlist)
    {
        if (sitem.id == type)
        {
            sitem.count = amount;

            ItemUpdate(type);
            return;
        }
    }

    //should not be able to set an item to negative
    if (amount < 0)
        return;


    stItemConfig * cfg = spitfire::GetSingleton().GetItem(type);

    stItem newitem;
    newitem.id = type;
    newitem.name = cfg->name;
    newitem.count = amount;
    newitem.maxcount = 0;
    newitem.mincount = 0;

    itemlist.push_back(newitem);

    ItemUpdate(type);
}

int64_t Client::GetItemCount(std::string type)
{
    if (type == "")
        return 0;
    for (stItem & sitem : itemlist)
    {
        if (sitem.id == type)
        {
            return sitem.count;
        }
    }
    return 0;
}

int64_t Client::GetItemCount(int16_t type)
{
    if (type < 0 || type > DEF_MAXITEMS)
        return 0;
    return GetItemCount(spitfire::GetSingleton().m_items[type].name);
}
 
int16_t Client::GetResearchLevel(int16_t id)
{
    return research[id].level;
}

void Client::CalculateResources()
{
    population = 0;
    for (int i = 0; i < citycount; ++i)
        population += ((PlayerCity*)citylist.at(i))->m_population;

    for (int i = 0; i < citylist.size(); ++i)
    {
        if (citylist[i])
        {
            citylist[i]->CalculateResourceStats();
            citylist[i]->CalculateStats();
            citylist[i]->CalculateResources();
        }
    }
}

void Client::PlayerInfoUpdate()
{
    amf3object obj = amf3object();
    obj["cmd"] = "server.PlayerInfoUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["playerInfo"] = PlayerInfo();

    spitfire::GetSingleton().SendObject(this, obj);
}
void Client::EnemyArmyUpdate()
{
    if (!connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.EnemyArmysUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    amf3array armylist = amf3array();

    for (stArmyMovement * movement : enemyarmymovement)
    {
        amf3object tempobj = movement->ToObject();
        armylist.Add(tempobj);
    }

    spitfire::GetSingleton().SendObject(this, obj);
}
void Client::FriendArmyUpdate()
{
    if (!connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.FriendArmysUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    amf3array armylist = amf3array();

    for (stArmyMovement * movement : friendarmymovement)
    {
        amf3object tempobj = movement->ToObject();
        armylist.Add(tempobj);
    }

    spitfire::GetSingleton().SendObject(this, obj);
}
void Client::SelfArmyUpdate()
{
    if (!connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.SelfArmysUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    amf3array armylist = amf3array();

    for (stArmyMovement * movement : armymovement)
    {
        amf3object tempobj = movement->ToObject();
        armylist.Add(tempobj);
    }

    data["armys"] = armylist;

    spitfire::GetSingleton().SendObject(this, obj);
}

void Client::ItemUpdate(std::string itemname)
{
    amf3object obj = amf3object();
    obj["cmd"] = "server.ItemUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];

    amf3array items = amf3array();

    for (stItem & sitem : itemlist)
    {
        if (sitem.id == itemname)
        {
            amf3object ta = amf3object();
            ta["id"] = itemname;
            ta["count"] = sitem.count;
            ta["minCount"] = sitem.mincount;
            ta["maxCount"] = sitem.maxcount;
            items.Add(ta);
            break;
        }
    }

    data["items"] = items;

    spitfire::GetSingleton().SendObject(this, obj);
}

void Client::HeroUpdate(int heroid, int castleid)
{
    amf3object obj = amf3object();
    obj["cmd"] = "server.HeroUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];

    data["castleId"] = castleid;
    for (int i = 0; i < citycount; ++i)
    {
        for (int k = 0; k < 10; ++k)
        {
            if (citylist[i]->m_heroes[k]->m_id == heroid)
            {
                data["hero"] = citylist[i]->m_heroes[k]->ToObject();
                spitfire::GetSingleton().SendObject(this, obj);
                return;
            }
        }
    }
    // TODO error case maybe? - void Client::HeroUpdate(int heroid, int castleid)
    return;
}

void Client::MailUpdate()
{
    amf3object obj = amf3object();
    obj["cmd"] = "server.NewMail";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];

    int32_t sysmail = 0;
    int32_t totalmail = 0;
    int32_t inboxmail = 0;

    for (stMail mail : maillist)
    {
        if (!mail.isread())
        {
            totalmail++;
            if (mail.type_id == 1)
                inboxmail++;
            else if (mail.type_id == 2)
                sysmail++;
        }
    }
    data["count_system"] = sysmail;
    data["count"] = totalmail;
    data["count_inbox"] = inboxmail;

    spitfire::GetSingleton().SendObject(this, obj);

    return;
}

void Client::ReportUpdate()
{
    amf3object obj = amf3object();
    obj["cmd"] = "server.NewReport";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];

    int32_t totalreport = 0;
    int32_t armycount = 0;
    int32_t tradecount = 0;
    int32_t othercount = 0;

    for (stReport report : reportlist)
    {
        if (!report.isread)
        {
            totalreport++;
            if (report.type_id == 0)
                tradecount++;
            else if (report.type_id == 1)
                armycount++;
            else if (report.type_id == 2)
                othercount++;
        }
    }
    data["count"] = totalreport;
    data["army_count"] = armycount;
    data["trade_count"] = tradecount;
    data["other_count"] = othercount;

    spitfire::GetSingleton().SendObject(this, obj);

    return;
}

void Client::BuffUpdate(std::string name, std::string desc, int64_t endtime, int8_t type)
{
    amf3object obj = amf3object();
    obj["cmd"] = "server.PlayerBuffUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];

    amf3object buffbean = amf3object();

    buffbean["descName"] = desc;
    buffbean["endTime"] = endtime;
    buffbean["typeId"] = name;

    data["buffBean"] = buffbean;

    data["updateType"] = (int32_t)type;

    spitfire::GetSingleton().SendObject(this, obj);
    return;
}

void Client::CastleSignUpdate()
{
    amf3object obj = amf3object();
    obj["cmd"] = "castle.castleSignInfoArr";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];

    amf3array castlesign;

    for (stCastleSign castle : castlesignlist)
    {
        amf3object castleobj = amf3object();

        castleobj["id"] = castle.id;
        castleobj["name"] = castle.name;
        castleobj["y"] = castle.y;
        castleobj["x"] = castle.x;

        castlesign.Add(castleobj);
    }


    data["castleSignInfoArr"] = castlesign;

    spitfire::GetSingleton().SendObject(this, obj);
    return;
}
