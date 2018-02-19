/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "PlayerCity.h"
#include "Client.h"
#include "spitfire.h"
#include "Hero.h"
#include "defines.h"
#include <Poco/Data/MySQL/MySQLException.h>
#include <spdlog/fmt/fmt.h>

using namespace Poco::Data::Keywords;


PlayerCity::PlayerCity(): m_availablepopulation(0)
{
    m_client = nullptr;
    m_castleid = 0;
    m_accountid = 0;
    memset(&m_troops, 0, sizeof(m_troops));
    memset(&m_injuredtroops, 0, sizeof(m_injuredtroops));
    memset(&m_maxresources, 0, sizeof(m_maxresources));
    memset(&m_production, 0, sizeof(m_production));
    memset(&m_workpopulation, 0, sizeof(m_workpopulation));
    memset(&m_workrate, 0, sizeof(m_workrate));
    memset(&m_storepercent, 0, sizeof(m_storepercent));

    m_population = 0;
    m_maxpopulation = 50;
    m_allowalliance = false;
    m_gooutforbattle = false;
    m_hasenemy = false;
    m_creation = 0;
    m_troopconsume = 0;
    m_productionefficiency = 100;


    m_resourcebaseproduction = 100;
    m_resourcemanagement = 0;
    m_resourcetech = 0;
    m_resourcevalley = 0;

    m_logurl = "images/icon/cityLogo/citylogo_01.png";

    for (auto i = 0; i < 10; ++i)
    {
        m_heroes[i] = nullptr;
        m_innheroes[i] = nullptr;
    }

    m_mayor = nullptr;
    m_lastcomfort = 0;
    m_lastlevy = 0;
    m_researching = false;
}

PlayerCity::~PlayerCity(void)
{
}

bool PlayerCity::SaveToDB()
{
    using CitySave = Poco::Tuple<std::string, int8_t, bool, std::string, int32_t, std::string, std::string, std::string, std::string, std::string, std::string, std::string, bool, bool, double, double, double, double, double>;


    CitySave savedata(DBMisc(), m_status, m_allowalliance, m_logurl, m_tileid, DBTransingtrades(), DBTroops(), DBTroopQueues(), m_cityname, DBBuildings(), DBFortifications(), DBTrades(), m_gooutforbattle, m_hasenemy, m_resources.gold, m_resources.food, m_resources.wood, m_resources.iron, m_resources.stone);


    try
    {
        auto ses(spitfire::GetSingleton().serverpool->get());
        ses << "UPDATE `cities` SET misc=?,status=?,allowalliance=?,logurl=?,fieldid=?,transingtrades=?,troop=?,troopqueues=?,name=?,buildings=?,fortification=?,trades=?,gooutforbattle=?,hasenemy=?,gold=?,food=?,wood=?,iron=?,stone=? WHERE id=?;", use(savedata), use(this->m_castleid), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

std::string PlayerCity::DBTroopQueues() const
{
    std::stringstream ss;
    std::vector<Poco::Any> z;
    std::string res;
    for (const stTroopQueue& x : m_troopqueue) {
        if (x.queue.size() == 0) continue;
        ss << "%?d,%?f";
        z.emplace_back(x.positionid);
        z.emplace_back(x.queue.front().endtime);
        for (const stTroopTrain& y : x.queue) {
            z.emplace_back(y.troopid);
            z.emplace_back(y.count);
            z.emplace_back(y.costtime);
            ss << ",%?d,%?d,%?f";
        }
        ss << "|";
    }
    Poco::format(res, ss.str(), z);
    return res;
}
int8_t PlayerCity::SetTroopQueue(int32_t position, int32_t troopid, int32_t count, int64_t costtime, int64_t endtime)
{
    stTroopQueue* x = GetBarracksQueue(position);
    if (x == nullptr) return false;
    if (endtime <= Utils::time()) {
        SetTroops(troopid, count);
        return false;
    }
    stTroopTrain z;
    z.troopid = troopid;
    z.count = count;
    z.costtime = costtime;
    z.queueid = x->nextqueueid++;
    if (x->queue.size() > 0) z.endtime = 0;
    else z.endtime = Utils::time() + z.costtime;
    x->queue.push_back(z);
    return true;
}
void PlayerCity::ParseTroopQueues(std::string str)
{
    if (str.length() > 0) {
        char* x = new char[str.length() + 1];
        memcpy(x, str.c_str(), str.length());
        x[str.length()] = 0;
        char* tk1 = nullptr;
        char* tk2 = nullptr;
        char* token;
        char* token1;
        int position;
        double endtime;
        double costtime;
        int count;
        int num;
        int troopid;
        int queueid;
        bool status;
        for (token=strtok_s(x,"|",&tk1) ; token != nullptr;token=strtok_s(nullptr,"|",&tk1)) {
            tk2 = nullptr;
            token1 = strtok_s(token, ",", &tk2);
            if (token1 == nullptr) continue;
            position = atoi(token1);
            token1 = strtok_s(nullptr, ",", &tk2);
            if (token1 == nullptr) continue;
            endtime = atof(token1);
            for (token1=strtok_s(nullptr,",",&tk2); token1 != nullptr; token1 = strtok_s(nullptr, ",", &tk2)) {
                troopid = atoi(token1);
                token1 = strtok_s(nullptr, ",", &tk2);
                if (token1 == nullptr) break;
                count = atoi(token1);
                token1 = strtok_s(nullptr, ",", &tk2);
                if (token1 == nullptr) break;
                costtime = atof(token1);
                if (SetTroopQueue(position, troopid, count, costtime, endtime)) endtime = 0;
                else endtime += costtime;
            }
        }
    }
}
std::string PlayerCity::DBBuildings() const
{
    std::string res;
    std::vector<Poco::Any> args;
    std::stringstream ss;

    for (auto i = -2; i < 32; ++i)
    {
        if (m_innerbuildings[i + 2].type > 0)
        {
            args.emplace_back(m_innerbuildings[i + 2].type);
            args.emplace_back(m_innerbuildings[i + 2].level);
            args.emplace_back(m_innerbuildings[i + 2].id);
            args.emplace_back(m_innerbuildings[i + 2].status);
            args.emplace_back(m_innerbuildings[i + 2].starttime);
            args.emplace_back(m_innerbuildings[i + 2].endtime);
            ss << "%?d,%?d,%?d,%?d,%f,%f|";
        }
    }
    for (auto i = 0; i < 41; ++i)
    {
        if (m_outerbuildings[i].type > 0)
        {
            args.emplace_back(m_outerbuildings[i].type);
            args.emplace_back(m_outerbuildings[i].level);
            args.emplace_back(m_outerbuildings[i].id);
            args.emplace_back(m_outerbuildings[i].status);
            args.emplace_back(m_outerbuildings[i].starttime);
            args.emplace_back(m_outerbuildings[i].endtime);
            ss << "%?d,%?d,%?d,%?d,%f,%f|";
        }
    }
    Poco::format(res, ss.str(), args);
    return res;
}
std::string PlayerCity::DBTransingtrades() const
{
    std::stringstream ss;
    return ss.str();
}

std::string PlayerCity::DBTrades() const
{
    std::stringstream ss;
    return ss.str();
}

std::string PlayerCity::DBTroops() const
{
    std::string res;
    std::vector<Poco::Any> args;
    args.emplace_back(TR_ARCHER);
    args.emplace_back(m_troops.archer);
    args.emplace_back(TR_WORKER);
    args.emplace_back(m_troops.worker);
    args.emplace_back(TR_WARRIOR);
    args.emplace_back(m_troops.warrior);
    args.emplace_back(TR_SCOUT);
    args.emplace_back(m_troops.scout);
    args.emplace_back(TR_PIKE);
    args.emplace_back(m_troops.pike);
    args.emplace_back(TR_SWORDS);
    args.emplace_back(m_troops.sword);
    args.emplace_back(TR_TRANSPORTER);
    args.emplace_back(m_troops.transporter);
    args.emplace_back(TR_RAM);
    args.emplace_back(m_troops.ram);
    args.emplace_back(TR_CATAPULT);
    args.emplace_back(m_troops.catapult);
    args.emplace_back(TR_CAVALRY);
    args.emplace_back(m_troops.cavalry);
    args.emplace_back(TR_CATAPHRACT);
    args.emplace_back(m_troops.cataphract);
    Poco::format(res, "%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d", args);
    return res;
}

std::string PlayerCity::DBFortifications() const
{
    std::string res;
    std::vector<Poco::Any> args;
    args.emplace_back(m_forts.traps);
    args.emplace_back(m_forts.abatis);
    args.emplace_back(m_forts.towers);
    args.emplace_back(m_forts.logs);
    args.emplace_back(m_forts.trebs);
    Poco::format(res, "%?d,%?d,%?d,%?d,%?d", args);
    return res;
}

std::string PlayerCity::DBMisc() const
{
    std::string res;
    std::vector<Poco::Any> args;
    args.emplace_back(m_population);
    args.emplace_back(m_workrate.gold);
    args.emplace_back(m_workrate.food);
    args.emplace_back(m_workrate.wood);
    args.emplace_back(m_workrate.iron);
    args.emplace_back(m_workrate.stone);
    args.emplace_back((int32_t)m_loyalty);
    args.emplace_back((int32_t)m_grievance);
    args.emplace_back(m_timers.updateresources);
    Poco::format(res, "%?d,%?f,%?f,%?f,%?f,%?f,%?d,%?d,%?f", args);
    return res;
}
void PlayerCity::SetForts(int32_t type, int32_t count)
{
    switch (type)
    {
    case TR_TRAP:
        m_forts.traps += count;
        break;
    case TR_ABATIS:
        m_forts.abatis += count;
        break;
    case TR_ARCHERTOWER:
        m_forts.towers += count;
        break;
    case TR_ROLLINGLOG:
        m_forts.logs += count;
        break;
    case TR_TREBUCHET:
        m_forts.trebs += count;
        break;
    }
    FortUpdate();
}
amf3object PlayerCity::ToObject()
{
    amf3object obj = amf3object();
    //obj["heroes"] = HeroArray();
    obj["buildingQueues"] = amf3array();
    obj["heros"] = HeroArray();
    obj["status"] = m_status;
    obj["allowAlliance"] = false;
    obj["resource"] = Resources();
    obj["logUrl"] = m_logurl;
    obj["fieldId"] = m_tileid;
    obj["usePACIFY_SUCCOUR_OR_PACIFY_PRAY"] = 1;//Unknown value (was set to 1)
    obj["transingTrades"] = amf3array();
    obj["troop"] = Troops();
    obj["id"] = m_castleid;
    obj["name"] = m_cityname;
    obj["buildings"] = Buildings();
    obj["fortification"] = Fortifications();
    obj["trades"] = amf3array();
    obj["fields"] = amf3array();//tiles owned?
    obj["goOutForBattle"] = false;
    obj["hasEnemy"] = false;
    return obj;
}

amf3array PlayerCity::HeroArray()
{
    amf3array array = amf3array();
    for (auto & hero : m_heroes)
    {
        if (hero)
        {
            amf3object temphero = hero->ToObject();
            array.Add(temphero);
        }
    }
    return array;
}

amf3object PlayerCity::Resources()
{
    amf3object obj = amf3object();
    obj["maxPopulation"] = m_maxpopulation;
    obj["taxIncome"] = m_production.gold;
    obj["support"] = m_loyalty;
    //obj["wood"] = amf3object();

    amf3object wood = amf3object();
    wood["amount"] = m_resources.wood;
    wood["storeRercent"] = (int)m_storepercent.wood;
    wood["workPeople"] = (int)m_workpopulation.wood;
    wood["max"] = (int)m_maxresources.wood;
    wood["increaseRate"] = (m_production.wood*(m_productionefficiency / 100)) * (1 + (m_resourcemanagement / 100)) + m_resourcebaseproduction;
    obj["wood"] = wood;

    amf3object stone = amf3object();
    stone["amount"] = m_resources.stone;
    stone["storeRercent"] = (int)m_storepercent.stone;
    stone["workPeople"] = (int)m_workpopulation.stone;
    stone["max"] = (int)m_maxresources.stone;
    stone["increaseRate"] = (m_production.stone*(m_productionefficiency / 100)) * (1 + (m_resourcemanagement / 100)) + m_resourcebaseproduction;
    obj["stone"] = stone;

    amf3object iron = amf3object();
    iron["amount"] = m_resources.iron;
    iron["storeRercent"] = (int)m_storepercent.iron;
    iron["workPeople"] = (int)m_workpopulation.iron;
    iron["max"] = (int)m_maxresources.iron;
    iron["increaseRate"] = (m_production.iron*(m_productionefficiency / 100)) * (1 + (m_resourcemanagement / 100)) + m_resourcebaseproduction;
    obj["iron"] = iron;

    amf3object food = amf3object();
    food["amount"] = m_resources.food;
    food["storeRercent"] = (int)m_storepercent.food;
    food["workPeople"] = (int)m_workpopulation.food;
    food["max"] = (int)m_maxresources.food;
    food["increaseRate"] = (m_production.food*(m_productionefficiency / 100)) * (1 + (m_resourcemanagement / 100)) - m_troopconsume + m_resourcebaseproduction;
    obj["food"] = food;

    obj["buildPeople"] = 0;// TODO: what is this
    obj["workPeople"] = int(m_workpopulation.food*m_workrate.food / 100 + m_workpopulation.wood*m_workrate.wood / 100 + m_workpopulation.stone*m_workrate.stone / 100 + m_workpopulation.iron*m_workrate.iron / 100);
    obj["curPopulation"] = m_population;
    int32_t herosalary = 0;
    for (auto & hero : m_heroes)
        if (hero)
            herosalary += hero->m_level * 20;
    obj["herosSalary"] = herosalary;
    obj["troopCostFood"] = m_troopconsume;
    obj["gold"] = m_resources.gold;
    obj["texRate"] = (int32_t)m_workrate.gold;
    obj["complaint"] = m_grievance;
    int32_t targetpopulation = (m_maxpopulation * (double(((m_loyalty + m_grievance) > 100) ? 100 : (m_loyalty + m_grievance)) / 100));
    //int targetpopulation = (m_maxpopulation * ((m_workrate.gold)/100));
    if (m_population > targetpopulation)
        obj["populationDirection"] = -1;
    else if (m_population < targetpopulation)
        obj["populationDirection"] = 1;
    else
        obj["populationDirection"] = 0;

    return obj;
}

amf3object PlayerCity::Troops() const
{
    amf3object obj = amf3object();
    obj["peasants"] = m_troops.worker;
    obj["archer"] = m_troops.archer;
    obj["catapult"] = m_troops.catapult;
    obj["ballista"] = m_troops.ballista;
    obj["scouter"] = m_troops.scout;
    obj["carriage"] = m_troops.transporter;
    obj["heavyCavalry"] = m_troops.cataphract;
    obj["militia"] = m_troops.warrior;
    obj["lightCavalry"] = m_troops.cavalry;
    obj["swordsmen"] = m_troops.sword;
    obj["pikemen"] = m_troops.pike;
    obj["batteringRam"] = m_troops.ram;
    return obj;
}

amf3object PlayerCity::InjuredTroops() const
{
    amf3object obj = amf3object();
    obj["peasants"] = m_injuredtroops.worker;
    obj["archer"] = m_injuredtroops.archer;
    obj["catapult"] = m_injuredtroops.catapult;
    obj["ballista"] = m_injuredtroops.ballista;
    obj["scouter"] = m_injuredtroops.scout;
    obj["carriage"] = m_injuredtroops.transporter;
    obj["heavyCavalry"] = m_injuredtroops.cataphract;
    obj["militia"] = m_injuredtroops.warrior;
    obj["lightCavalry"] = m_injuredtroops.cavalry;
    obj["swordsmen"] = m_injuredtroops.sword;
    obj["pikemen"] = m_injuredtroops.pike;
    obj["batteringRam"] = m_injuredtroops.ram;
    return obj;
}

amf3array PlayerCity::Buildings() const
{
    amf3array array = amf3array();
    //for (int i = 0; i < m_buildings.Count; ++i)
    {
        amf3object obj = amf3object();
        //age2?
        //obj["appearance"] = 96;
        //obj["help"] = 1;

        for (const auto & innerbuilding : m_innerbuildings)
        {
            if (innerbuilding.type > 0)
            {
                obj = amf3object();
                obj["startTime"] = innerbuilding.starttime;
                obj["endTime"] = innerbuilding.endtime;
                obj["level"] = innerbuilding.level;
                obj["status"] = innerbuilding.status;
                obj["positionId"] = innerbuilding.id;
                obj["name"] = Utils::GetBuildingName(innerbuilding.type);
                obj["typeId"] = innerbuilding.type;
                array.Add(obj);
            }
        }
        for (const auto & outerbuilding : m_outerbuildings)
        {
            if (outerbuilding.type > 0)
            {
                obj = amf3object();
                obj["startTime"] = outerbuilding.starttime;
                obj["endTime"] = outerbuilding.endtime;
                obj["level"] = outerbuilding.level;
                obj["status"] = outerbuilding.status;
                obj["positionId"] = outerbuilding.id;
                obj["name"] = Utils::GetBuildingName(outerbuilding.type);
                obj["typeId"] = outerbuilding.type;
                array.Add(obj);
            }
        }
    }
    return array;
}

amf3object PlayerCity::Fortifications() const
{
    amf3object forts = amf3object();
    forts["trap"] = m_forts.traps;
    forts["rollingLogs"] = m_forts.logs;
    forts["rockfall"] = m_forts.trebs;
    forts["arrowTower"] = m_forts.towers;
    forts["abatis"] = m_forts.abatis;

    return forts;
}


void PlayerCity::ParseBuildings(std::string str)
{
    if (str.length() > 0)
    {
        char * str2 = new char[str.length() + 1];
        memset(str2, 0, str.length() + 1);
        memcpy(str2, str.c_str(), str.length());

        int type = 0;
        int level = 0;
        int position = 0;
        int status = 0;
        double starttime = 0.0;
        double endtime = 0.0;

        char * ch = 0, *cr = 0;
        char * tok;

        char * temp;
        temp = new char[strlen(str2) + 10];
        memset(temp, 0, strlen(str2) + 10);
        memcpy(temp, str2, strlen(str2));
        temp[strlen(str2) + 1] = 0;
        tok = strtok_s(temp, "|", &ch);
        do
        {
            tok = strtok_s(tok, ",", &cr);
            if (tok != 0)
                type = atoi(tok);
            tok = strtok_s(0, ",", &cr);
            if (tok != 0)
                level = atoi(tok);
            tok = strtok_s(0, ",", &cr);
            if (tok != 0)
                position = atoi(tok);
            tok = strtok_s(0, ",", &cr);
            if (tok != 0)
                status = atoi(tok);
            tok = strtok_s(0, ",", &cr);
            if (tok != 0)
                starttime = atof(tok);
            tok = strtok_s(0, ",", &cr);
            if (tok != 0)
                endtime = atof(tok);

            SetBuilding(type, level, position, status, starttime, endtime);

            if ((status == 1) || (status == 2))
            {
                stBuildingAction * ba = new stBuildingAction;

                stTimedEvent te;
                ba->city = this;
                ba->client = this->m_client;
                ba->positionid = position;
                te.data = ba;
                te.type = DEF_TIMEDBUILDING;

                spitfire::GetSingleton().AddTimedEvent(te);
            }

            tok = strtok_s(0, "|", &ch);
        } while (tok != 0);
        delete[] temp;
        delete[] str2;
    }
}

void PlayerCity::CalculateStats()
{
    memset(&m_maxresources, 0, sizeof(m_maxresources));
    memset(&m_production, 0, sizeof(m_production));
    memset(&m_workpopulation, 0, sizeof(m_workpopulation));

    m_maxresources.food = 10000;
    m_maxresources.wood = 10000;
    m_maxresources.stone = 10000;
    m_maxresources.iron = 10000;
    m_maxpopulation = 50;

    for (auto & innerbuilding : m_innerbuildings)
    {
        if (innerbuilding.type > 0)
        {
            if (innerbuilding.type == 1)
            {
                switch (innerbuilding.level)
                {
                case 1:
                    m_maxpopulation += 100;
                    break;
                case 2:
                    m_maxpopulation += 300;
                    break;
                case 3:
                    m_maxpopulation += 600;
                    break;
                case 4:
                    m_maxpopulation += 1000;
                    break;
                case 5:
                    m_maxpopulation += 1500;
                    break;
                case 6:
                    m_maxpopulation += 2100;
                    break;
                case 7:
                    m_maxpopulation += 2800;
                    break;
                case 8:
                    m_maxpopulation += 3600;
                    break;
                case 9:
                    m_maxpopulation += 4500;
                    break;
                case 10:
                    m_maxpopulation += 5500;
                    break;
                }
            }
        }
    }
    for (auto & outerbuilding : m_outerbuildings)
    {
        if (outerbuilding.type > 0)
        {
            double * pmax, *pprod, *pwork;
            if (outerbuilding.type == 7)
            {
                pmax = &m_maxresources.food;
                pprod = &m_production.food;
                pwork = &m_workpopulation.food;
            }
            else if (outerbuilding.type == 4)
            {
                pmax = &m_maxresources.wood;
                pprod = &m_production.wood;
                pwork = &m_workpopulation.wood;
            }
            else if (outerbuilding.type == 5)
            {
                pmax = &m_maxresources.stone;
                pprod = &m_production.stone;
                pwork = &m_workpopulation.stone;
            }
            else if (outerbuilding.type == 6)
            {
                pmax = &m_maxresources.iron;
                pprod = &m_production.iron;
                pwork = &m_workpopulation.iron;
            }
            else
            {
                continue;
            }
            switch (outerbuilding.level)
            {
            case 1:
                *pmax += 10000;
                *pprod += 100;
                *pwork += 10;
                break;
            case 2:
                *pmax += 30000;
                *pprod += 300;
                *pwork += 30;
                break;
            case 3:
                *pmax += 60000;
                *pprod += 600;
                *pwork += 60;
                break;
            case 4:
                *pmax += 100000;
                *pprod += 1000;
                *pwork += 100;
                break;
            case 5:
                *pmax += 150000;
                *pprod += 1500;
                *pwork += 150;
                break;
            case 6:
                *pmax += 210000;
                *pprod += 2100;
                *pwork += 210;
                break;
            case 7:
                *pmax += 280000;
                *pprod += 2800;
                *pwork += 280;
                break;
            case 8:
                *pmax += 360000;
                *pprod += 3600;
                *pwork += 360;
                break;
            case 9:
                *pmax += 450000;
                *pprod += 4500;
                *pwork += 450;
                break;
            case 10:
                *pmax += 550000;
                *pprod += 5500;
                *pwork += 550;
                break;
            }
        }
    }

    m_production.food *= (m_workrate.food / 100);
    m_production.wood *= (m_workrate.wood / 100);
    m_production.stone *= (m_workrate.stone / 100);
    m_production.iron *= (m_workrate.iron / 100);

    int32_t workpop = (m_workrate.food*m_workpopulation.food / 100) + (m_workrate.stone*m_workpopulation.stone / 100) + (m_workrate.wood*m_workpopulation.wood / 100) + (m_workrate.iron*m_workpopulation.iron / 100);
    m_availablepopulation = m_population - workpop;
    m_productionefficiency = (m_population >= workpop) ? 100 : (float(m_population) / workpop * 100);
    m_production.gold = double(m_population) * (m_workrate.gold / 100);
}

void PlayerCity::CalculateResourceStats()
{
    // TODO add resource production modifiers (tech and valley)
    //     m_resourcebaseproduction = 100;
    //     m_resourcemanagement = m_mayor!=0?(m_mayor->m_management+m_mayor->m_managementadded+m_mayor->m_managementbuffadded):0;
    //     m_resourcetech = 0;
    //     m_resourcevalley = 0;
}

void PlayerCity::ParseMisc(std::string str)
{
    if (str.length() > 0)
    {
        char * str2 = new char[str.length() + 1];
        memset(str2, 0, str.length() + 1);
        memcpy(str2, str.c_str(), str.length());
        char * ch = 0, *cr = 0;
        char * tok;
        tok = strtok_s(str2, ",", &ch);
        assert(tok != 0);
        m_population = (uint16_t)atoi(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_workrate.gold = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_workrate.food = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_workrate.wood = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_workrate.iron = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_workrate.stone = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_loyalty = (int8_t)atoi(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_grievance = (int8_t)atoi(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        m_timers.updateresources = atof(tok);

        delete[] str2;
    }
}
void PlayerCity::SetTroops(int8_t type, int64_t amount)
{
    if (type == TR_ARCHER)
        m_troops.archer += amount;
    else if (type == TR_WORKER)
        m_troops.worker += amount;
    else if (type == TR_WARRIOR)
        m_troops.warrior += amount;
    else if (type == TR_SCOUT)
        m_troops.scout += amount;
    else if (type == TR_PIKE)
        m_troops.pike += amount;
    else if (type == TR_SWORDS)
        m_troops.sword += amount;
    else if (type == TR_TRANSPORTER)
        m_troops.transporter += amount;
    else if (type == TR_RAM)
        m_troops.ram += amount;
    else if (type == TR_CATAPULT)
        m_troops.catapult += amount;
    else if (type == TR_BALLISTA)
        m_troops.ballista += amount;
    else if (type == TR_CAVALRY)
        m_troops.cavalry += amount;
    else if (type == TR_CATAPHRACT)
        m_troops.cataphract += amount;
    TroopUpdate();
}

int64_t PlayerCity::GetTroops(int8_t type) const
{
    if (type == TR_ARCHER)
        return m_troops.archer;
    else if (type == TR_WORKER)
        return m_troops.worker;
    else if (type == TR_WARRIOR)
        return m_troops.warrior;
    else if (type == TR_SCOUT)
        return m_troops.scout;
    else if (type == TR_PIKE)
        return m_troops.pike;
    else if (type == TR_SWORDS)
        return m_troops.sword;
    else if (type == TR_TRANSPORTER)
        return m_troops.transporter;
    else if (type == TR_RAM)
        return m_troops.ram;
    else if (type == TR_CATAPULT)
        return m_troops.catapult;
    else if (type == TR_CAVALRY)
        return m_troops.cavalry;
    else if (type == TR_CATAPHRACT)
        return m_troops.cataphract;
    else if (type==TR_BALLISTA)
        return m_troops.ballista;
    return 0;
}
/*
void PlayerCity::ParseTroops(std::string str)
{
    if (str.length() > 0)
    {
        std::vector<std::string> tokens;
        std::vector<std::string> tokens2;
        my_split(tokens, str, "|");
        //boost::split(tokens, str, boost::is_any_of("|"));

        int64_t tr = 0;
        int64_t num = 0;
        for (std::string & toks : tokens)
        {
            my_split(tokens2, toks, ",");
            //boost::split(tokens2, toks, boost::is_any_of(","));
            tr = _atoi64(tokens2[0].c_str());
            num = _atoi64(tokens2[1].c_str());
            if (num < 0)
                SetTroops(tr, 0);
            else
                SetTroops(tr, num);
        }
    }
}

*/
void PlayerCity::ParseTroops(std::string str)
{
    if (str.length() > 0)
    {
        char * str2 = new char[str.length() + 1];
        memset(str2, 0, str.length() + 1);
        memcpy(str2, str.c_str(), str.length());

        int amount;

        char * ch = str2;
        char * tok;
        int64_t tr = 0, ty = 0;
        //for (int i = 0; i < 12; ++i,tok = strtok_s(0, "|", &ch))
        while (tok = strtok_s(ch, "|", &ch))
        {
            //char * tok2 = new char[tok.length()+1];
            //memset(tok2, 0, tok.length()+1);
            //memcpy(tok2, tok.c_str(), tok.length());
            char* cr = tok;
            if (tok != 0)
            {
                ty = atoi(strtok_s(cr, ",", &cr));
                tr = _atoi64(strtok_s(cr, ",", &cr));
                if (tr < 0)
                    SetTroops(ty, 0);
                else
                    SetTroops(ty, tr);
            }
            //delete[] tok2;
        }

        delete[] str2;
    }
}



void PlayerCity::ParseFortifications(std::string str)
{
    if (str.length() > 0)
    {
        char * str2 = new char[str.length() + 1];
        memset(str2, 0, str.length() + 1);
        memcpy(str2, str.c_str(), str.length());

        int traps;
        int abatis;
        int towers;
        int logs;
        int trebs;

        char * cr = 0;
        char * tok;
        tok = strtok_s(str2, ",", &cr);
        if (tok != 0)
            traps = atoi(tok);
        tok = strtok_s(0, ",", &cr);
        if (tok != 0)
            abatis = atoi(tok);
        tok = strtok_s(0, ",", &cr);
        if (tok != 0)
            towers = atoi(tok);
        tok = strtok_s(0, ",", &cr);
        if (tok != 0)
            logs = atoi(tok);
        tok = strtok_s(0, ",", &cr);
        if (tok != 0)
            trebs = atoi(tok);
        City::SetForts(traps, abatis, towers, logs, trebs);

        delete[] str2;
    }
}

int16_t PlayerCity::GetBuildingLevel(int16_t id)
{
    int level = 0;
    for (auto & innerbuilding : m_innerbuildings)
        if (innerbuilding.type > 0)
            if (innerbuilding.type == id)
                if (level < innerbuilding.level)
                    level = innerbuilding.level;
    for (auto & outerbuilding : m_outerbuildings)
        if (outerbuilding.type > 0)
            if (outerbuilding.type == id)
                if (level < outerbuilding.level)
                    level = outerbuilding.level;
    return level;
}
int16_t PlayerCity::GetBuildingCount(int16_t id)
{
    int32_t count = 0;
    for (auto & innerbuilding : m_innerbuildings)
        if (innerbuilding.type > 0)
            if (innerbuilding.type == id)
                count++;
    for (auto & outerbuilding : m_outerbuildings)
        if (outerbuilding.type > 0)
            if (outerbuilding.type == id)
                count++;
    return count;
}

stTrade* PlayerCity::GetTrade(int64_t id)
{
    //     for (auto & trade : m_trades)
    //     {
    //         if (trade.id == id)
    //             return trade;
    //     }
    return nullptr;
}

stTransingTrade* PlayerCity::GetTransingTrade(int64_t id)
{
    return nullptr;
}

stBuilding * PlayerCity::GetBuilding(int16_t position)
{
    if (position > 1000)
    {
        if (position - 1000 > 40)
        {
            throw std::out_of_range(fmt::format("Outer position [{}] out of range pos-1000>40", position));
        }
        return &m_outerbuildings[position - 1000];
    }
    else
    {
        if ((position < -2) || (position > 35))
        {
            throw std::out_of_range(fmt::format("Inner position [{}] out of range -2>32", position));
        }

        position += 2;

        return &m_innerbuildings[position];
    }
}
int16_t PlayerCity::GetEffectiveTechLevel(int16_t id)
{
    int blevel = GetBuildingLevel(B_ACADEMY);//academy
    int research = m_client->GetResearchLevel(id);
    if (blevel != 0)
    {
        return blevel <= research ? blevel : research;
    }
    return 0;
}

//TODO: server is a bit slower than client for resource calculation
void PlayerCity::CalculateResources()
{
    CalculateResourceStats();
    uint64_t newtime, oldtime, diff;
    newtime = Utils::time();

    oldtime = m_timers.updateresources;
    diff = newtime - oldtime;

    m_timers.updateresources = newtime;

    double add;
    add = ((((m_production.food + m_resourcebaseproduction)*(m_productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (m_resourcemanagement / 100)) * diff) - m_troopconsume;
    if (m_maxresources.food > m_resources.food)
    {
        if (m_maxresources.food < m_resources.food + add)
        {
            m_resources.food = m_maxresources.food;
        }
        else
        {
            m_resources.food += add;
        }
    }
    add = ((((m_production.wood + m_resourcebaseproduction)*(m_productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (m_resourcemanagement / 100)) * diff);
    if (m_maxresources.wood > m_resources.wood)
    {
        if (m_maxresources.wood < m_resources.wood + add)
        {
            m_resources.wood = m_maxresources.wood;
        }
        else
        {
            m_resources.wood += add;
        }
    }
    add = ((((m_production.stone + m_resourcebaseproduction)*(m_productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (m_resourcemanagement / 100)) * diff);
    if (m_maxresources.stone > m_resources.stone)
    {
        if (m_maxresources.stone < m_resources.stone + add)
        {
            m_resources.stone = m_maxresources.stone;
        }
        else
        {
            m_resources.stone += add;
        }
    }
    add = ((((m_production.iron + m_resourcebaseproduction)*(m_productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (m_resourcemanagement / 100)) * diff);
    if (m_maxresources.iron > m_resources.iron)
    {
        if (m_maxresources.iron < m_resources.iron + add)
        {
            m_resources.iron = m_maxresources.iron;
        }
        else
        {
            m_resources.iron += add;
        }
    }

    int32_t herosalary = 0;
    for (auto & hero : m_heroes)
        if (hero)
            herosalary += hero->m_level * 20;


    add = (m_production.gold - herosalary) * (double(diff) / 60 / 60 / 1000);
    m_resources.gold += add;
    if (m_resources.gold < 0)
        m_resources.gold = 0;
}

void PlayerCity::CastleUpdate()
{
    if (!m_client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.CastleUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["updateType"] = 2;
    data["castleBean"] = ToObject();

    spitfire::GetSingleton().SendObject(m_client, obj);
}

void PlayerCity::ResourceUpdate()
{
    if (!m_client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.ResourceUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = m_castleid;
    data["resource"] = Resources();

    spitfire::GetSingleton().SendObject(m_client, obj);
}

void PlayerCity::TradesUpdate(uint64_t amount, uint64_t id, int32_t tradetype, double price, std::string tradetypename, uint64_t dealedtotal, std::string resourcename, int32_t restype, int32_t updatetype) const
{
    if (!m_client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.TradesUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = m_castleid;
    amf3object bean = amf3object();
    bean["amount"] = amount;
    bean["id"] = id;
    bean["tradeType"] = tradetype;
    bean["price"] = price;
    bean["tradeTypeName"] = tradetypename;
    bean["dealedTotal"] = dealedtotal;
    bean["resourceName"] = resourcename;
    bean["resType"] = restype;
    data["tradeBean"] = bean;
    data["updateType"] = updatetype;

    spitfire::GetSingleton().SendObject(m_client, obj);
}

void PlayerCity::HeroUpdate(Hero * hero, int16_t updatetype) const
{
    if (!m_client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.HeroUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = m_castleid;

    if (updatetype == 0)//hire hero
    {
        data["hero"] = hero->ToObject();
        data["updateType"] = updatetype;
    }
    else if (updatetype == 1)//fire hero
    {
        data["hero"] = hero->ToObject();
        data["updateType"] = updatetype;
    }
    else if (updatetype == 2)//update hero
    {
        data["hero"] = hero->ToObject();
        data["updateType"] = updatetype;
    }

    spitfire::GetSingleton().SendObject(m_client, obj);
}

int16_t PlayerCity::GetReliefMultiplier()
{
    switch (GetBuildingLevel(B_RELIEFSTATION))
    {
    default:
        return 1;
    case 1:
    case 2:
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
    case 6:
    case 7:
        return 4;
    case 8:
    case 9:
        return 5;
    case 10:
        return 6;
    }
}

void PlayerCity::TroopUpdate() const
{
    if (!m_client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.TroopUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["caslteId"] = m_castleid;
    data["troop"] = Troops();

    spitfire::GetSingleton().SendObject(m_client, obj);
}

void PlayerCity::FortUpdate() const
{
    if (!m_client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.FortificationsUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = m_castleid;
    data["fortification"] = Fortifications();

    spitfire::GetSingleton().SendObject(m_client, obj);
}

void PlayerCity::RecalculateCityStats()
{
    int targetpopulation = (m_maxpopulation * (double(((m_loyalty + m_grievance) > 100) ? 100 : (m_loyalty + m_grievance)) / 100));
    int add = m_maxpopulation * 0.05;
    if (m_population > targetpopulation)
    {
        if (m_population - add < targetpopulation)
            m_population = targetpopulation;
        else if (m_population - add < 0)
            m_population = m_maxpopulation;
        else
            m_population -= add;
    }
    else if (m_population < targetpopulation)
    {
        if (m_population + add > targetpopulation)
            m_population = targetpopulation;
        else if (m_population + add > m_maxpopulation)
            m_population = m_maxpopulation;
        else
            m_population += add;
    }
    else
    {
        //nothing, pop is as it should be
    }

    int32_t targetloyalty = 100 - (((m_workrate.gold + m_grievance) > 100) ? 100 : (m_workrate.gold + m_grievance));
    if (m_loyalty > 1)
    {
        if (targetloyalty < m_loyalty)
        {
            m_loyalty--;
        }
        else if (targetloyalty > m_loyalty)
        {
            m_loyalty++;
        }
        else if (targetloyalty == m_loyalty)
        {

        }
    }

    m_production.gold = m_population * (m_workrate.gold / 100);
    ResourceUpdate();
}

amf3array PlayerCity::ResourceProduceData() const
{
    amf3array bean;
    amf3object obj = amf3object();

    obj["technologicalPercentage"] = m_resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (m_production.food*(m_productionefficiency / 100)) * (m_resourcemanagement / 100);
    obj["maxLabour"] = m_workpopulation.food;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = m_resourcevalley;
    obj["naturalPercentage"] = m_resourcebaseproduction;
    obj["armyPercentage"] = m_troopconsume;
    obj["productionCapacity"] = m_production.food;
    obj["typeid"] = 1;
    obj["commenceRate"] = m_workrate.food;
    obj["cimeliaPercentage"] = 0; //buff plus maybe?
    obj["basicOutput"] = 0;

    bean.Add(obj);

    obj = amf3object();
    obj["technologicalPercentage"] = m_resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (m_production.wood*(m_productionefficiency / 100)) * (m_resourcemanagement / 100);
    obj["maxLabour"] = m_workpopulation.wood;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = m_resourcevalley;
    obj["naturalPercentage"] = m_resourcebaseproduction;
    obj["armyPercentage"] = 0;
    obj["productionCapacity"] = m_production.wood;
    obj["typeid"] = 2;
    obj["commenceRate"] = m_workrate.wood;
    obj["cimeliaPercentage"] = 0; //buff plus maybe?
    obj["basicOutput"] = 0;

    bean.Add(obj);

    obj = amf3object();
    obj["technologicalPercentage"] = m_resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (m_production.stone*(m_productionefficiency / 100)) * (m_resourcemanagement / 100);
    obj["maxLabour"] = m_workpopulation.stone;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = m_resourcevalley;
    obj["naturalPercentage"] = m_resourcebaseproduction;
    obj["armyPercentage"] = 0;
    obj["productionCapacity"] = m_production.stone;
    obj["typeid"] = 3;
    obj["commenceRate"] = m_workrate.stone;
    obj["cimeliaPercentage"] = 0; //buff plus maybe?
    obj["basicOutput"] = 0;

    bean.Add(obj);

    obj = amf3object();
    obj["technologicalPercentage"] = m_resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (m_production.iron*(m_productionefficiency / 100)) * (m_resourcemanagement / 100);
    obj["maxLabour"] = m_workpopulation.iron;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = m_resourcevalley;
    obj["naturalPercentage"] = m_resourcebaseproduction;
    obj["armyPercentage"] = 0;
    obj["productionCapacity"] = m_production.iron;
    obj["typeid"] = 4;
    obj["commenceRate"] = m_workrate.iron;
    obj["cimeliaPercentage"] = 0; //buff plus maybe?
    obj["basicOutput"] = 0;

    bean.Add(obj);
    return bean;
}

int8_t PlayerCity::AddToBarracksQueue(int8_t position, int16_t troopid, int32_t count, bool isshare, bool isidle)
{
     stTroopQueue * queue = GetBarracksQueue(position);
     stBuilding * bldg = GetBuilding(position);
     if (queue->queue.size() >= bldg->level)
     {
         return -1;
     }
     stBuildingConfig * conf = &spitfire::GetSingleton().m_troopconfig[troopid];
     stTroopTrain troops;
     troops.troopid = troopid;
     troops.count = count;
 
     double costtime = conf->time;
     double mayorinf = 1;
 
     if (position == -2)
     {
         if (m_mayor)
             mayorinf = pow(0.995, m_mayor->GetManagement());
 
         costtime = (costtime)* (mayorinf)* (pow(0.9, m_client->GetResearchLevel(T_CONSTRUCTION)));
     }
     else
     {
         if (m_mayor)
             mayorinf = pow(0.995, m_mayor->GetPower());
 
         switch (troopid)
         {
        case TR_CATAPULT:
         case TR_RAM:
         case TR_TRANSPORTER:
         case TR_BALLISTA:
             costtime = (costtime)* (mayorinf)* (pow(0.9, m_client->GetResearchLevel(T_METALCASTING)));
         default:
             costtime = (costtime)* (mayorinf)* (pow(0.9, m_client->GetResearchLevel(T_MILITARYSCIENCE)));
         }
     }
 
     troops.costtime = floor(costtime) * count * 1000;
     troops.starttime = Utils::time();
     troops.queueid = queue->nextqueueid++;
 
     if (queue->queue.size() > 0)
         troops.endtime = 0;
     else
         troops.endtime = Utils::time() + troops.costtime;
     queue->queue.push_back(troops);
    return 1;
}
int16_t PlayerCity::HeroCount()
{
    int16_t cnt = 0;
    for (auto & hero : m_heroes)
        if (hero)
            ++cnt;
    return cnt;
}

stTroopQueue * PlayerCity::GetBarracksQueue(int16_t position)
{
    for (auto & tq : m_troopqueue)
    {
        if (tq.positionid == position)
        {
            return &tq;
        }
    }
    return nullptr;
}

bool PlayerCity::CheckBuildingPrereqs(int16_t type, int16_t level)
{
    if (type <= 0 || type > 35)
        return false;
    stBuildingConfig * cfg = &spitfire::GetSingleton().m_buildingconfig[type][level];

    bool test = true;


    for (stPrereq & req : cfg->buildings)
    {
        if (req.id)
            if (GetBuildingLevel(req.id) < req.level)
                test = false;
    }
    for (stPrereq & req : cfg->items)
    {
        if (req.id)
            if (m_client->GetItemCount(req.id) < req.level)
                test = false;
    }
    for (stPrereq & req : cfg->techs)
    {
        if (req.id)
            if (m_client->GetResearchLevel(req.id) < req.level)
                test = false;
    }
    return test;
}

bool PlayerCity::HasTroops(stTroops & troops) const
{
    if (m_troops.worker - troops.worker < 0)
        return false;
    if (m_troops.warrior - troops.warrior < 0)
        return false;
    if (m_troops.scout - troops.scout < 0)
        return false;
    if (m_troops.pike - troops.pike < 0)
        return false;
    if (m_troops.sword - troops.sword < 0)
        return false;
    if (m_troops.archer - troops.archer < 0)
        return false;
    if (m_troops.cavalry - troops.cavalry < 0)
        return false;
    if (m_troops.cataphract - troops.cataphract < 0)
        return false;
    if (m_troops.transporter - troops.transporter < 0)
        return false;
    if (m_troops.ballista - troops.ballista < 0)
        return false;
    if (m_troops.ram - troops.ram < 0)
        return false;
    if (m_troops.catapult - troops.catapult < 0)
        return false;
    return true;
}
Hero * PlayerCity::GetHero(uint64_t id)
{
    for (auto & hero : m_heroes)
    {
        if (hero && hero->m_id == id)
            return hero;
    }
    return nullptr;
}
