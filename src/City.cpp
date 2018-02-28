/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "City.h"
#include "PlayerCity.h"
#include "NpcCity.h"
#include "Client.h"
#include "spitfire.h"
#include "Hero.h"
#include "defines.h"


City::City()
{
    cityname = "";
    status = 0;
    tileid = 0;
    type = 0;
    level = 0;
    loyalty = 0;
    grievance = 0;
    memset(&resources, 0, sizeof(resources));
    memset(&forts, 0, sizeof(forts));
    memset(&outerbuildings, 0, sizeof(outerbuildings));
    memset(&innerbuildings, 0, sizeof(innerbuildings));
}


City::~City(void)
{
}



bool City::SetBuilding(int16_t type, int8_t level, int16_t position, int16_t status, double starttime, double endtime)
{
    if (position > 1000)
    {
        if (position - 1000 > 40)
        {
            if (this->type == NPC)
                spitfire::GetSingleton().log->error("Problem setting out of range NPC building id : {} - tileid: {}", position, tileid);
            else if (this->type == CASTLE)
                spitfire::GetSingleton().log->error("Problem setting out of range PLAYER building id : {} - castleid: {} - accountid: {}", position, ((PlayerCity*)this)->castleid, ((PlayerCity*)this)->accountid);
            return false;
        }
        outerbuildings[position - 1000].id = position;
        outerbuildings[position - 1000].type = type;
        outerbuildings[position - 1000].level = level;
        outerbuildings[position - 1000].status = status;
        outerbuildings[position - 1000].starttime = starttime;
        outerbuildings[position - 1000].endtime = endtime;
        return true;
    }
    else
    {
        if ((position < -2) || (position > 31))
        {
            if (this->type == NPC)
                spitfire::GetSingleton().log->error("Problem setting out of range NPC building id : {} - tileid: {}", position, tileid);
            else if (this->type == CASTLE)
                spitfire::GetSingleton().log->error("Problem setting out of range PLAYER building id : {} - castleid: {} - accountid: {}", position, ((PlayerCity*)this)->castleid, ((PlayerCity*)this)->accountid);
            return false;
        }


        if ((this->type == CASTLE) && ((type == B_BARRACKS) || (type == B_WALLS)))
        {
            if ((level > 0) && (((PlayerCity*)this)->GetBarracksQueue(position) == 0))
            {
                stTroopQueue queue;
                queue.nextqueueid = 0;
                queue.positionid = position;
                queue.status = 0;
                ((PlayerCity*)this)->troopqueue.push_back(queue);
            }
            else if ((level == 0) && (((PlayerCity*)this)->GetBarracksQueue(position) != 0))//TODO this causes some error if building barracks. destroy barracks then rebuild?
            {
                if (((PlayerCity*)this)->troopqueue.size() > 0)
                {
                    for (std::vector<stTroopQueue>::iterator iter = ((PlayerCity*)this)->troopqueue.begin(); iter != ((PlayerCity*)this)->troopqueue.end(); ++iter)
                    {
                        if (iter->positionid == position)
                        {
                            ((PlayerCity*)this)->troopqueue.erase(iter);
                            break;
                        }
                    }
                }
            }
        }
        if ((type == B_TOWNHALL) && ((this->type == CASTLE) || (this->type == NPC)))
        {
            level = level;
        }

        position += 2;

        innerbuildings[position].id = position - 2;
        innerbuildings[position].type = type;
        innerbuildings[position].level = level;
        innerbuildings[position].status = status;
        innerbuildings[position].starttime = starttime;
        innerbuildings[position].endtime = endtime;
        return true;
    }
    return false;
}




void City::SetResources(double food, double wood, double stone, double iron, double gold)
{
    resources.food = food;
    resources.gold = gold;
    resources.wood = wood;
    resources.iron = iron;
    resources.stone = stone;
}

void City::SetMaxResources(double food, double wood, double stone, double iron, double gold)
{
    maxresources.food = food;
    maxresources.gold = gold;
    maxresources.wood = wood;
    maxresources.iron = iron;
    maxresources.stone = stone;
}



void City::SetForts(int32_t traps, int32_t abatis, int32_t towers, int32_t logs, int32_t trebs)
{
    forts.traps = traps;
    forts.abatis = abatis;
    forts.towers = towers;
    forts.logs = logs;
    forts.trebs = trebs;
}



int32_t City::GetForts(int32_t type) const
{
    switch (type)
    {
    case TR_TRAP:
        return forts.traps;
        break;
    case TR_ABATIS:
        return forts.abatis;
        break;
    case TR_ARCHERTOWER:
        return forts.towers;
        break;
    case TR_ROLLINGLOG:
        return forts.logs;
        break;
    case TR_TREBUCHET:
        return forts.trebs;
        break;
    }
    return 0;
}

