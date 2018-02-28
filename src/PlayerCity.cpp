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


PlayerCity::PlayerCity(): availablepopulation(0)
{
    client = nullptr;
    castleid = 0;
    accountid = 0;
    memset(&troops, 0, sizeof(troops));
    memset(&injuredtroops, 0, sizeof(injuredtroops));
    memset(&maxresources, 0, sizeof(maxresources));
    memset(&production, 0, sizeof(production));
    memset(&workpopulation, 0, sizeof(workpopulation));
    memset(&workrate, 0, sizeof(workrate));
    memset(&storepercent, 0, sizeof(storepercent));

    population = 0;
    maxpopulation = 50;
    allowalliance = false;
    gooutforbattle = false;
    hasenemy = false;
    creation = 0;
    troopconsume = 0;
    productionefficiency = 100;


    resourcebaseproduction = 100;
    resourcemanagement = 0;
    resourcetech = 0;
    resourcevalley = 0;

    logurl = "images/icon/cityLogo/citylogo_01.png";

    for (auto i = 0; i < 10; ++i)
    {
        heroes[i] = nullptr;
        innheroes[i] = nullptr;
    }

    mayor = nullptr;
    lastcomfort = 0;
    lastlevy = 0;
    researching = false;
}

PlayerCity::~PlayerCity(void)
{
}

bool PlayerCity::SaveToDB()
{
    using CitySave = Poco::Tuple<std::string, int8_t, bool, std::string, int32_t, std::string, std::string, std::string, std::string, std::string, std::string, std::string, bool, bool, double, double, double, double, double>;


    CitySave savedata(DBMisc(), status, allowalliance, logurl, tileid, DBTransingtrades(), DBTroops(), DBTroopQueues(), cityname, DBBuildings(), DBFortifications(), DBTrades(), gooutforbattle, hasenemy, resources.gold, resources.food, resources.wood, resources.iron, resources.stone);


    try
    {
        auto ses(spitfire::GetSingleton().serverpool->get());
        ses << "UPDATE `cities` SET misc=?,status=?,allowalliance=?,logurl=?,fieldid=?,transingtrades=?,troop=?,troopqueues=?,name=?,buildings=?,fortification=?,trades=?,gooutforbattle=?,hasenemy=?,gold=?,food=?,wood=?,iron=?,stone=? WHERE id=?;", use(savedata), use(this->castleid), now;
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
    for (const stTroopQueue& x : troopqueue) {
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
        if (innerbuildings[i + 2].type > 0)
        {
            args.emplace_back(innerbuildings[i + 2].type);
            args.emplace_back(innerbuildings[i + 2].level);
            args.emplace_back(innerbuildings[i + 2].id);
            args.emplace_back(innerbuildings[i + 2].status);
            args.emplace_back(innerbuildings[i + 2].starttime);
            args.emplace_back(innerbuildings[i + 2].endtime);
            ss << "%?d,%?d,%?d,%?d,%f,%f|";
        }
    }
    for (auto i = 0; i < 41; ++i)
    {
        if (outerbuildings[i].type > 0)
        {
            args.emplace_back(outerbuildings[i].type);
            args.emplace_back(outerbuildings[i].level);
            args.emplace_back(outerbuildings[i].id);
            args.emplace_back(outerbuildings[i].status);
            args.emplace_back(outerbuildings[i].starttime);
            args.emplace_back(outerbuildings[i].endtime);
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
    args.emplace_back(troops.archer);
    args.emplace_back(TR_WORKER);
    args.emplace_back(troops.worker);
    args.emplace_back(TR_WARRIOR);
    args.emplace_back(troops.warrior);
    args.emplace_back(TR_SCOUT);
    args.emplace_back(troops.scout);
    args.emplace_back(TR_PIKE);
    args.emplace_back(troops.pike);
    args.emplace_back(TR_SWORDS);
    args.emplace_back(troops.sword);
    args.emplace_back(TR_TRANSPORTER);
    args.emplace_back(troops.transporter);
    args.emplace_back(TR_RAM);
    args.emplace_back(troops.ram);
    args.emplace_back(TR_CATAPULT);
    args.emplace_back(troops.catapult);
    args.emplace_back(TR_CAVALRY);
    args.emplace_back(troops.cavalry);
    args.emplace_back(TR_CATAPHRACT);
    args.emplace_back(troops.cataphract);
    Poco::format(res, "%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d|%?d,%?d", args);
    return res;
}

std::string PlayerCity::DBFortifications() const
{
    std::string res;
    std::vector<Poco::Any> args;
    args.emplace_back(forts.traps);
    args.emplace_back(forts.abatis);
    args.emplace_back(forts.towers);
    args.emplace_back(forts.logs);
    args.emplace_back(forts.trebs);
    Poco::format(res, "%?d,%?d,%?d,%?d,%?d", args);
    return res;
}

std::string PlayerCity::DBMisc() const
{
    std::string res;
    std::vector<Poco::Any> args;
    args.emplace_back(population);
    args.emplace_back(workrate.gold);
    args.emplace_back(workrate.food);
    args.emplace_back(workrate.wood);
    args.emplace_back(workrate.iron);
    args.emplace_back(workrate.stone);
    args.emplace_back((int32_t)loyalty);
    args.emplace_back((int32_t)grievance);
    args.emplace_back(timers.updateresources);
    Poco::format(res, "%?d,%?f,%?f,%?f,%?f,%?f,%?d,%?d,%?f", args);
    return res;
}
void PlayerCity::SetForts(int32_t type, int32_t count)
{
    switch (type)
    {
    case TR_TRAP:
        forts.traps += count;
        break;
    case TR_ABATIS:
        forts.abatis += count;
        break;
    case TR_ARCHERTOWER:
        forts.towers += count;
        break;
    case TR_ROLLINGLOG:
        forts.logs += count;
        break;
    case TR_TREBUCHET:
        forts.trebs += count;
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
    obj["status"] = status;
    obj["allowAlliance"] = false;
    obj["resource"] = Resources();
    obj["logUrl"] = logurl;
    obj["fieldId"] = tileid;
    obj["usePACIFY_SUCCOUR_OR_PACIFY_PRAY"] = 1;//Unknown value (was set to 1)
    obj["transingTrades"] = amf3array();
    obj["troop"] = Troops();
    obj["id"] = castleid;
    obj["name"] = cityname;
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
    for (auto & hero : heroes)
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
    obj["maxPopulation"] = maxpopulation;
    obj["taxIncome"] = production.gold;
    obj["support"] = loyalty;
    //obj["wood"] = amf3object();

    amf3object wood = amf3object();
    wood["amount"] = resources.wood;
    wood["storeRercent"] = (int)storepercent.wood;
    wood["workPeople"] = (int)workpopulation.wood;
    wood["max"] = (int)maxresources.wood;
    wood["increaseRate"] = (production.wood*(productionefficiency / 100)) * (1 + (resourcemanagement / 100)) + resourcebaseproduction;
    obj["wood"] = wood;

    amf3object stone = amf3object();
    stone["amount"] = resources.stone;
    stone["storeRercent"] = (int)storepercent.stone;
    stone["workPeople"] = (int)workpopulation.stone;
    stone["max"] = (int)maxresources.stone;
    stone["increaseRate"] = (production.stone*(productionefficiency / 100)) * (1 + (resourcemanagement / 100)) + resourcebaseproduction;
    obj["stone"] = stone;

    amf3object iron = amf3object();
    iron["amount"] = resources.iron;
    iron["storeRercent"] = (int)storepercent.iron;
    iron["workPeople"] = (int)workpopulation.iron;
    iron["max"] = (int)maxresources.iron;
    iron["increaseRate"] = (production.iron*(productionefficiency / 100)) * (1 + (resourcemanagement / 100)) + resourcebaseproduction;
    obj["iron"] = iron;

    amf3object food = amf3object();
    food["amount"] = resources.food;
    food["storeRercent"] = (int)storepercent.food;
    food["workPeople"] = (int)workpopulation.food;
    food["max"] = (int)maxresources.food;
    food["increaseRate"] = (production.food*(productionefficiency / 100)) * (1 + (resourcemanagement / 100)) - troopconsume + resourcebaseproduction;
    obj["food"] = food;

    obj["buildPeople"] = 0;// TODO: what is this
    obj["workPeople"] = int(workpopulation.food*workrate.food / 100 + workpopulation.wood*workrate.wood / 100 + workpopulation.stone*workrate.stone / 100 + workpopulation.iron*workrate.iron / 100);
    obj["curPopulation"] = population;
    int32_t herosalary = 0;
    for (auto & hero : heroes)
        if (hero)
            herosalary += hero->level * 20;
    obj["herosSalary"] = herosalary;
    obj["troopCostFood"] = troopconsume;
    obj["gold"] = resources.gold;
    obj["texRate"] = (int32_t)workrate.gold;
    obj["complaint"] = grievance;
    int32_t targetpopulation = (maxpopulation * (double(((loyalty + grievance) > 100) ? 100 : (loyalty + grievance)) / 100));
    //int targetpopulation = (maxpopulation * ((workrate.gold)/100));
    if (population > targetpopulation)
        obj["populationDirection"] = -1;
    else if (population < targetpopulation)
        obj["populationDirection"] = 1;
    else
        obj["populationDirection"] = 0;

    return obj;
}

amf3object PlayerCity::Troops() const
{
    amf3object obj = amf3object();
    obj["peasants"] = troops.worker;
    obj["archer"] = troops.archer;
    obj["catapult"] = troops.catapult;
    obj["ballista"] = troops.ballista;
    obj["scouter"] = troops.scout;
    obj["carriage"] = troops.transporter;
    obj["heavyCavalry"] = troops.cataphract;
    obj["militia"] = troops.warrior;
    obj["lightCavalry"] = troops.cavalry;
    obj["swordsmen"] = troops.sword;
    obj["pikemen"] = troops.pike;
    obj["batteringRam"] = troops.ram;
    return obj;
}

amf3object PlayerCity::InjuredTroops() const
{
    amf3object obj = amf3object();
    obj["peasants"] = injuredtroops.worker;
    obj["archer"] = injuredtroops.archer;
    obj["catapult"] = injuredtroops.catapult;
    obj["ballista"] = injuredtroops.ballista;
    obj["scouter"] = injuredtroops.scout;
    obj["carriage"] = injuredtroops.transporter;
    obj["heavyCavalry"] = injuredtroops.cataphract;
    obj["militia"] = injuredtroops.warrior;
    obj["lightCavalry"] = injuredtroops.cavalry;
    obj["swordsmen"] = injuredtroops.sword;
    obj["pikemen"] = injuredtroops.pike;
    obj["batteringRam"] = injuredtroops.ram;
    return obj;
}

amf3array PlayerCity::Buildings() const
{
    amf3array array = amf3array();
    //for (int i = 0; i < buildings.Count; ++i)
    {
        amf3object obj = amf3object();
        //age2?
        //obj["appearance"] = 96;
        //obj["help"] = 1;

        for (const auto & innerbuilding : innerbuildings)
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
        for (const auto & outerbuilding : outerbuildings)
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
    forts["trap"] = forts.traps;
    forts["rollingLogs"] = forts.logs;
    forts["rockfall"] = forts.trebs;
    forts["arrowTower"] = forts.towers;
    forts["abatis"] = forts.abatis;

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
                ba->client = this->client;
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
    memset(&maxresources, 0, sizeof(maxresources));
    memset(&production, 0, sizeof(production));
    memset(&workpopulation, 0, sizeof(workpopulation));

    maxresources.food = 10000;
    maxresources.wood = 10000;
    maxresources.stone = 10000;
    maxresources.iron = 10000;
    maxpopulation = 50;

    for (auto & innerbuilding : innerbuildings)
    {
        if (innerbuilding.type > 0)
        {
            if (innerbuilding.type == 1)
            {
                switch (innerbuilding.level)
                {
                case 1:
                    maxpopulation += 100;
                    break;
                case 2:
                    maxpopulation += 300;
                    break;
                case 3:
                    maxpopulation += 600;
                    break;
                case 4:
                    maxpopulation += 1000;
                    break;
                case 5:
                    maxpopulation += 1500;
                    break;
                case 6:
                    maxpopulation += 2100;
                    break;
                case 7:
                    maxpopulation += 2800;
                    break;
                case 8:
                    maxpopulation += 3600;
                    break;
                case 9:
                    maxpopulation += 4500;
                    break;
                case 10:
                    maxpopulation += 5500;
                    break;
                }
            }
        }
    }
    for (auto & outerbuilding : outerbuildings)
    {
        if (outerbuilding.type > 0)
        {
            double * pmax, *pprod, *pwork;
            if (outerbuilding.type == 7)
            {
                pmax = &maxresources.food;
                pprod = &production.food;
                pwork = &workpopulation.food;
            }
            else if (outerbuilding.type == 4)
            {
                pmax = &maxresources.wood;
                pprod = &production.wood;
                pwork = &workpopulation.wood;
            }
            else if (outerbuilding.type == 5)
            {
                pmax = &maxresources.stone;
                pprod = &production.stone;
                pwork = &workpopulation.stone;
            }
            else if (outerbuilding.type == 6)
            {
                pmax = &maxresources.iron;
                pprod = &production.iron;
                pwork = &workpopulation.iron;
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

    production.food *= (workrate.food / 100);
    production.wood *= (workrate.wood / 100);
    production.stone *= (workrate.stone / 100);
    production.iron *= (workrate.iron / 100);

    int32_t workpop = (workrate.food*workpopulation.food / 100) + (workrate.stone*workpopulation.stone / 100) + (workrate.wood*workpopulation.wood / 100) + (workrate.iron*workpopulation.iron / 100);
    availablepopulation = population - workpop;
    productionefficiency = (population >= workpop) ? 100 : (float(population) / workpop * 100);
    production.gold = double(population) * (workrate.gold / 100);
}

void PlayerCity::CalculateResourceStats()
{
    // TODO add resource production modifiers (tech and valley)
    //     resourcebaseproduction = 100;
    //     resourcemanagement = mayor!=0?(mayor->management+mayor->managementadded+mayor->managementbuffadded):0;
    //     resourcetech = 0;
    //     resourcevalley = 0;
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
        population = (uint16_t)atoi(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        workrate.gold = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        workrate.food = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        workrate.wood = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        workrate.iron = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        workrate.stone = atof(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        loyalty = (int8_t)atoi(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        grievance = (int8_t)atoi(tok);

        tok = strtok_s(0, ",", &ch);
        assert(tok != 0);
        timers.updateresources = atof(tok);

        delete[] str2;
    }
}
void PlayerCity::SetTroops(int8_t type, int64_t amount)
{
    if (type == TR_ARCHER)
        troops.archer += amount;
    else if (type == TR_WORKER)
        troops.worker += amount;
    else if (type == TR_WARRIOR)
        troops.warrior += amount;
    else if (type == TR_SCOUT)
        troops.scout += amount;
    else if (type == TR_PIKE)
        troops.pike += amount;
    else if (type == TR_SWORDS)
        troops.sword += amount;
    else if (type == TR_TRANSPORTER)
        troops.transporter += amount;
    else if (type == TR_RAM)
        troops.ram += amount;
    else if (type == TR_CATAPULT)
        troops.catapult += amount;
    else if (type == TR_BALLISTA)
        troops.ballista += amount;
    else if (type == TR_CAVALRY)
        troops.cavalry += amount;
    else if (type == TR_CATAPHRACT)
        troops.cataphract += amount;
    TroopUpdate();
}

int64_t PlayerCity::GetTroops(int8_t type) const
{
    if (type == TR_ARCHER)
        return troops.archer;
    else if (type == TR_WORKER)
        return troops.worker;
    else if (type == TR_WARRIOR)
        return troops.warrior;
    else if (type == TR_SCOUT)
        return troops.scout;
    else if (type == TR_PIKE)
        return troops.pike;
    else if (type == TR_SWORDS)
        return troops.sword;
    else if (type == TR_TRANSPORTER)
        return troops.transporter;
    else if (type == TR_RAM)
        return troops.ram;
    else if (type == TR_CATAPULT)
        return troops.catapult;
    else if (type == TR_CAVALRY)
        return troops.cavalry;
    else if (type == TR_CATAPHRACT)
        return troops.cataphract;
    else if (type==TR_BALLISTA)
        return troops.ballista;
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
    for (auto & innerbuilding : innerbuildings)
        if (innerbuilding.type > 0)
            if (innerbuilding.type == id)
                if (level < innerbuilding.level)
                    level = innerbuilding.level;
    for (auto & outerbuilding : outerbuildings)
        if (outerbuilding.type > 0)
            if (outerbuilding.type == id)
                if (level < outerbuilding.level)
                    level = outerbuilding.level;
    return level;
}
int16_t PlayerCity::GetBuildingCount(int16_t id)
{
    int32_t count = 0;
    for (auto & innerbuilding : innerbuildings)
        if (innerbuilding.type > 0)
            if (innerbuilding.type == id)
                count++;
    for (auto & outerbuilding : outerbuildings)
        if (outerbuilding.type > 0)
            if (outerbuilding.type == id)
                count++;
    return count;
}

stTrade* PlayerCity::GetTrade(int64_t id)
{
    //     for (auto & trade : trades)
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
        return &outerbuildings[position - 1000];
    }
    else
    {
        if ((position < -2) || (position > 35))
        {
            throw std::out_of_range(fmt::format("Inner position [{}] out of range -2>32", position));
        }

        position += 2;

        return &innerbuildings[position];
    }
}
int16_t PlayerCity::GetEffectiveTechLevel(int16_t id)
{
    int blevel = GetBuildingLevel(B_ACADEMY);//academy
    int research = client->GetResearchLevel(id);
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

    oldtime = timers.updateresources;
    diff = newtime - oldtime;

    timers.updateresources = newtime;

    double add;
    add = ((((production.food + resourcebaseproduction)*(productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (resourcemanagement / 100)) * diff) - troopconsume;
    if (maxresources.food > resources.food)
    {
        if (maxresources.food < resources.food + add)
        {
            resources.food = maxresources.food;
        }
        else
        {
            resources.food += add;
        }
    }
    add = ((((production.wood + resourcebaseproduction)*(productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (resourcemanagement / 100)) * diff);
    if (maxresources.wood > resources.wood)
    {
        if (maxresources.wood < resources.wood + add)
        {
            resources.wood = maxresources.wood;
        }
        else
        {
            resources.wood += add;
        }
    }
    add = ((((production.stone + resourcebaseproduction)*(productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (resourcemanagement / 100)) * diff);
    if (maxresources.stone > resources.stone)
    {
        if (maxresources.stone < resources.stone + add)
        {
            resources.stone = maxresources.stone;
        }
        else
        {
            resources.stone += add;
        }
    }
    add = ((((production.iron + resourcebaseproduction)*(productionefficiency / 100)) / 60 / 60 / 1000) * (1 + (resourcemanagement / 100)) * diff);
    if (maxresources.iron > resources.iron)
    {
        if (maxresources.iron < resources.iron + add)
        {
            resources.iron = maxresources.iron;
        }
        else
        {
            resources.iron += add;
        }
    }

    int32_t herosalary = 0;
    for (auto & hero : heroes)
        if (hero)
            herosalary += hero->level * 20;


    add = (production.gold - herosalary) * (double(diff) / 60 / 60 / 1000);
    resources.gold += add;
    if (resources.gold < 0)
        resources.gold = 0;
}

void PlayerCity::CastleUpdate()
{
    if (!client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.CastleUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["updateType"] = 2;
    data["castleBean"] = ToObject();

    spitfire::GetSingleton().SendObject(client, obj);
}

void PlayerCity::ResourceUpdate()
{
    if (!client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.ResourceUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = castleid;
    data["resource"] = Resources();

    spitfire::GetSingleton().SendObject(client, obj);
}

void PlayerCity::TradesUpdate(uint64_t amount, uint64_t id, int32_t tradetype, double price, std::string tradetypename, uint64_t dealedtotal, std::string resourcename, int32_t restype, int32_t updatetype) const
{
    if (!client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.TradesUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = castleid;
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

    spitfire::GetSingleton().SendObject(client, obj);
}

void PlayerCity::HeroUpdate(Hero * hero, int16_t updatetype) const
{
    if (!client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.HeroUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = castleid;

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

    spitfire::GetSingleton().SendObject(client, obj);
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
    if (!client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.TroopUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["caslteId"] = castleid;
    data["troop"] = Troops();

    spitfire::GetSingleton().SendObject(client, obj);
}

void PlayerCity::FortUpdate() const
{
    if (!client->connected)
        return;
    amf3object obj = amf3object();
    obj["cmd"] = "server.FortificationsUpdate";
    obj["data"] = amf3object();
    amf3object & data = obj["data"];
    data["castleId"] = castleid;
    data["fortification"] = Fortifications();

    spitfire::GetSingleton().SendObject(client, obj);
}

void PlayerCity::RecalculateCityStats()
{
    int targetpopulation = (maxpopulation * (double(((loyalty + grievance) > 100) ? 100 : (loyalty + grievance)) / 100));
    int add = maxpopulation * 0.05;
    if (population > targetpopulation)
    {
        if (population - add < targetpopulation)
            population = targetpopulation;
        else if (population - add < 0)
            population = maxpopulation;
        else
            population -= add;
    }
    else if (population < targetpopulation)
    {
        if (population + add > targetpopulation)
            population = targetpopulation;
        else if (population + add > maxpopulation)
            population = maxpopulation;
        else
            population += add;
    }
    else
    {
        //nothing, pop is as it should be
    }

    int32_t targetloyalty = 100 - (((workrate.gold + grievance) > 100) ? 100 : (workrate.gold + grievance));
    if (loyalty > 1)
    {
        if (targetloyalty < loyalty)
        {
            loyalty--;
        }
        else if (targetloyalty > loyalty)
        {
            loyalty++;
        }
        else if (targetloyalty == loyalty)
        {

        }
    }

    production.gold = population * (workrate.gold / 100);
    ResourceUpdate();
}

amf3array PlayerCity::ResourceProduceData() const
{
    amf3array bean;
    amf3object obj = amf3object();

    obj["technologicalPercentage"] = resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (production.food*(productionefficiency / 100)) * (resourcemanagement / 100);
    obj["maxLabour"] = workpopulation.food;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = resourcevalley;
    obj["naturalPercentage"] = resourcebaseproduction;
    obj["armyPercentage"] = troopconsume;
    obj["productionCapacity"] = production.food;
    obj["typeid"] = 1;
    obj["commenceRate"] = workrate.food;
    obj["cimeliaPercentage"] = 0; //buff plus maybe?
    obj["basicOutput"] = 0;

    bean.Add(obj);

    obj = amf3object();
    obj["technologicalPercentage"] = resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (production.wood*(productionefficiency / 100)) * (resourcemanagement / 100);
    obj["maxLabour"] = workpopulation.wood;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = resourcevalley;
    obj["naturalPercentage"] = resourcebaseproduction;
    obj["armyPercentage"] = 0;
    obj["productionCapacity"] = production.wood;
    obj["typeid"] = 2;
    obj["commenceRate"] = workrate.wood;
    obj["cimeliaPercentage"] = 0; //buff plus maybe?
    obj["basicOutput"] = 0;

    bean.Add(obj);

    obj = amf3object();
    obj["technologicalPercentage"] = resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (production.stone*(productionefficiency / 100)) * (resourcemanagement / 100);
    obj["maxLabour"] = workpopulation.stone;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = resourcevalley;
    obj["naturalPercentage"] = resourcebaseproduction;
    obj["armyPercentage"] = 0;
    obj["productionCapacity"] = production.stone;
    obj["typeid"] = 3;
    obj["commenceRate"] = workrate.stone;
    obj["cimeliaPercentage"] = 0; //buff plus maybe?
    obj["basicOutput"] = 0;

    bean.Add(obj);

    obj = amf3object();
    obj["technologicalPercentage"] = resourcetech;
    obj["totalOutput"] = 0;
    obj["heroPercentage"] = (production.iron*(productionefficiency / 100)) * (resourcemanagement / 100);
    obj["maxLabour"] = workpopulation.iron;
    obj["commenceDemands"] = 0;
    obj["fieldPercentage"] = resourcevalley;
    obj["naturalPercentage"] = resourcebaseproduction;
    obj["armyPercentage"] = 0;
    obj["productionCapacity"] = production.iron;
    obj["typeid"] = 4;
    obj["commenceRate"] = workrate.iron;
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
     stBuildingConfig * conf = &spitfire::GetSingleton().troopconfig[troopid];
     stTroopTrain troops;
     troops.troopid = troopid;
     troops.count = count;
 
     double costtime = conf->time;
     double mayorinf = 1;
 
     if (position == -2)
     {
         if (mayor)
             mayorinf = pow(0.995, mayor->GetManagement());
 
         costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_CONSTRUCTION)));
     }
     else
     {
         if (mayor)
             mayorinf = pow(0.995, mayor->GetPower());
 
         switch (troopid)
         {
        case TR_CATAPULT:
         case TR_RAM:
         case TR_TRANSPORTER:
         case TR_BALLISTA:
             costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_METALCASTING)));
         default:
             costtime = (costtime)* (mayorinf)* (pow(0.9, client->GetResearchLevel(T_MILITARYSCIENCE)));
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
    for (auto & hero : heroes)
        if (hero)
            ++cnt;
    return cnt;
}

stTroopQueue * PlayerCity::GetBarracksQueue(int16_t position)
{
    for (auto & tq : troopqueue)
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
    stBuildingConfig * cfg = &spitfire::GetSingleton().buildingconfig[type][level];

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
            if (client->GetItemCount(req.id) < req.level)
                test = false;
    }
    for (stPrereq & req : cfg->techs)
    {
        if (req.id)
            if (client->GetResearchLevel(req.id) < req.level)
                test = false;
    }
    return test;
}

bool PlayerCity::HasTroops(stTroops & troops) const
{
    if (troops.worker - troops.worker < 0)
        return false;
    if (troops.warrior - troops.warrior < 0)
        return false;
    if (troops.scout - troops.scout < 0)
        return false;
    if (troops.pike - troops.pike < 0)
        return false;
    if (troops.sword - troops.sword < 0)
        return false;
    if (troops.archer - troops.archer < 0)
        return false;
    if (troops.cavalry - troops.cavalry < 0)
        return false;
    if (troops.cataphract - troops.cataphract < 0)
        return false;
    if (troops.transporter - troops.transporter < 0)
        return false;
    if (troops.ballista - troops.ballista < 0)
        return false;
    if (troops.ram - troops.ram < 0)
        return false;
    if (troops.catapult - troops.catapult < 0)
        return false;
    return true;
}
Hero * PlayerCity::GetHero(uint64_t id)
{
    for (auto & hero : heroes)
    {
        if (hero && hero->id == id)
            return hero;
    }
    return nullptr;
}
