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
    m_cityname = "";
    m_status = 0;
    m_tileid = 0;
    m_type = 0;
    m_level = 0;
    m_loyalty = 0;
    m_grievance = 0;
    memset(&m_resources, 0, sizeof(m_resources));
    memset(&m_forts, 0, sizeof(m_forts));
    memset(&m_outerbuildings, 0, sizeof(m_outerbuildings));
    memset(&m_innerbuildings, 0, sizeof(m_innerbuildings));
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
            if (this->m_type == NPC)
                spitfire::GetSingleton().log->error("Problem setting out of range NPC building id : {} - tileid: {}", position, m_tileid);
            else if (this->m_type == CASTLE)
                spitfire::GetSingleton().log->error("Problem setting out of range PLAYER building id : {} - castleid: {} - accountid: {}", position, ((PlayerCity*)this)->m_castleid, ((PlayerCity*)this)->m_accountid);
            return false;
        }
        m_outerbuildings[position - 1000].id = position;
        m_outerbuildings[position - 1000].type = type;
        m_outerbuildings[position - 1000].level = level;
        m_outerbuildings[position - 1000].status = status;
        m_outerbuildings[position - 1000].starttime = starttime;
        m_outerbuildings[position - 1000].endtime = endtime;
        return true;
    }
    else
    {
        if ((position < -2) || (position > 31))
        {
            if (this->m_type == NPC)
                spitfire::GetSingleton().log->error("Problem setting out of range NPC building id : {} - tileid: {}", position, m_tileid);
            else if (this->m_type == CASTLE)
                spitfire::GetSingleton().log->error("Problem setting out of range PLAYER building id : {} - castleid: {} - accountid: {}", position, ((PlayerCity*)this)->m_castleid, ((PlayerCity*)this)->m_accountid);
            return false;
        }


        if ((this->m_type == CASTLE) && ((type == B_BARRACKS) || (type == B_WALLS)))
        {
            if ((level > 0) && (((PlayerCity*)this)->GetBarracksQueue(position) == 0))
            {
                stTroopQueue queue;
                queue.nextqueueid = 0;
                queue.positionid = position;
                queue.status = 0;
                ((PlayerCity*)this)->m_troopqueue.push_back(queue);
            }
            else if ((level == 0) && (((PlayerCity*)this)->GetBarracksQueue(position) != 0))//TODO this causes some error if building barracks. destroy barracks then rebuild?
            {
                if (((PlayerCity*)this)->m_troopqueue.size() > 0)
                {
                    for (std::vector<stTroopQueue>::iterator iter = ((PlayerCity*)this)->m_troopqueue.begin(); iter != ((PlayerCity*)this)->m_troopqueue.end(); ++iter)
                    {
                        if (iter->positionid == position)
                        {
                            ((PlayerCity*)this)->m_troopqueue.erase(iter);
                            break;
                        }
                    }
                }
            }
        }
        if ((type == B_TOWNHALL) && ((this->m_type == CASTLE) || (this->m_type == NPC)))
        {
            m_level = level;
        }

        position += 2;

        m_innerbuildings[position].id = position - 2;
        m_innerbuildings[position].type = type;
        m_innerbuildings[position].level = level;
        m_innerbuildings[position].status = status;
        m_innerbuildings[position].starttime = starttime;
        m_innerbuildings[position].endtime = endtime;
        return true;
    }
    return false;
}




void City::SetResources(double gold, double food, double wood, double stone, double iron)
{
    m_resources.food = food;
    m_resources.gold = gold;
    m_resources.wood = wood;
    m_resources.iron = iron;
    m_resources.stone = stone;
}

void City::SetMaxResources(double gold, double food, double wood, double stone, double iron)
{
    m_maxresources.food = food;
    m_maxresources.gold = gold;
    m_maxresources.wood = wood;
    m_maxresources.iron = iron;
    m_maxresources.stone = stone;
}



void City::SetForts(int32_t traps, int32_t abatis, int32_t towers, int32_t logs, int32_t trebs)
{
    m_forts.traps = traps;
    m_forts.abatis = abatis;
    m_forts.towers = towers;
    m_forts.logs = logs;
    m_forts.trebs = trebs;
}



int32_t City::GetForts(int32_t type) const
{
    switch (type)
    {
    case TR_TRAP:
        return m_forts.traps;
        break;
    case TR_ABATIS:
        return m_forts.abatis;
        break;
    case TR_ARCHERTOWER:
        return m_forts.towers;
        break;
    case TR_ROLLINGLOG:
        return m_forts.logs;
        break;
    case TR_TREBUCHET:
        return m_forts.trebs;
        break;
    }
    return 0;
}

