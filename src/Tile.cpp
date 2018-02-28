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
    city = nullptr;
    castleicon = 0;
    castleid = -1;
    id = -1;
    npc = false;
    ownerid = -1;
    powerlevel = -1;
    state = -1;
    type = FLAT;
    status = -1;
    zoneid = -1;
    level = -1;
    /*    x = y = -1;*/
}


Tile::~Tile()
{
}


amf3object Tile::ToObject() const
{
    PlayerCity & city = *(PlayerCity*)city;
    amf3object obj = amf3object();
    obj["id"] = id;
    obj["name"] = city.cityname;
    obj["npc"] = npc;
    obj["prestige"] = city.client->prestige;
    obj["honor"] = city.client->honor;
    obj["state"] = city.client->status;
    obj["userName"] = city.client->playername;
    obj["flag"] = city.client->flag;
    obj["allianceName"] = city.client->alliancename;
    return obj;
}

std::string Tile::GetName() const
{
    //if (npc)
    switch (type)
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
        return (city) ? city->cityname : "Invalid City";
    case NPC:
        return "Barbarian's City";
    default:
        return "null";
    }
}
