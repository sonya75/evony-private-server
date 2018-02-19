/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "Tile.h"
#include "Client.h"
#include "PlayerCity.h"
#include "defines.h"

Tile::Tile()
{
    m_city = nullptr;
    m_castleicon = 0;
    m_castleid = -1;
    m_id = -1;
    m_npc = false;
    m_ownerid = -1;
    m_powerlevel = -1;
    m_state = -1;
    m_type = FLAT;
    m_status = -1;
    m_zoneid = -1;
    m_level = -1;
    /*    x = y = -1;*/
}


Tile::~Tile()
{
}


amf3object Tile::ToObject() const
{
    PlayerCity & city = *(PlayerCity*)m_city;
    amf3object obj = amf3object();
    obj["id"] = m_id;
    obj["name"] = city.m_cityname;
    obj["npc"] = m_npc;
    obj["prestige"] = city.m_client->prestige;
    obj["honor"] = city.m_client->honor;
    obj["state"] = city.m_client->status;
    obj["userName"] = city.m_client->playername;
    obj["flag"] = city.m_client->flag;
    obj["allianceName"] = city.m_client->alliancename;
    return obj;
}

std::string Tile::GetName() const
{
    //if (m_npc)
    switch (m_type)
    {
    case FOREST:
        return "Forest";
    case DESERT:
        return "Desert";
    case HILL:
        return "Hill";
    case SWAMP:
        return "Swamp";
    case GRASS:
        return "Grass";
    case LAKE:
        return "Lake";
    case FLAT:
        return "Flat";
    case CASTLE:
        return (m_city) ? m_city->m_cityname : "Invalid City";
    case NPC:
        return "Barbarian's City";
    default:
        return "null";
    }
}
