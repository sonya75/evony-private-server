/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "Map.h"
#include "Client.h"
#include "PlayerCity.h"
#include "spitfire.h"
#include "Tile.h"
#include "AllianceMgr.h"
#include "defines.h"

Map::Map(uint32_t size)
{
    mapsize = size;
    tile = new Tile[mapsize*mapsize];
    for (int y = 0; y < mapsize; ++y)
    {
        for (int x = 0; x < mapsize; ++x)
        {
//             tile[y*gserver->mapsize+x].x = x;
//             tile[y*gserver->mapsize+x].y = y;
            GETIDFROMXY(x, y);
            tile[y*mapsize + x].id = GETID;
            tile[y*mapsize + x].zoneid = GetStateFromXY(x, y);
        }
    }
    memset(&openflats, 0, sizeof(openflats));
    memset(&totalflats, 0, sizeof(totalflats));
    memset(&cities, 0, sizeof(cities));
    memset(&occupiedtiles, 0, sizeof(occupiedtiles));
    memset(&occupiabletiles, 0, sizeof(occupiabletiles));
    memset(&npcs, 0, sizeof(npcs));
    memset(&stats, 0, sizeof(stats));

    //CalculateOpenTiles();
}


Map::~Map()
{
    delete[] tile;
}

bool Map::AddCity(int id, City * city)
{
    return true;
}


void Map::CalculateOpenTiles()
{
    int tempstate = 0;
    Tile * tile;
    memset(&openflats, 0, sizeof(openflats));
    memset(&totalflats, 0, sizeof(totalflats));
    memset(&cities, 0, sizeof(cities));
    memset(&occupiedtiles, 0, sizeof(occupiedtiles));
    memset(&occupiabletiles, 0, sizeof(occupiabletiles));
    memset(&npcs, 0, sizeof(npcs));
    for (int i = 0; i < DEF_STATES; ++i)
        openflatlist[i].clear();
    memset(&stats, 0, sizeof(stats));
    for (int y = 0; y < spitfire::GetSingleton().mapsize; ++y)
    {
        for (int x = 0; x < spitfire::GetSingleton().mapsize; ++x)
        {
            tempstate = GetStateFromXY(x, y);


            tile = &(tile[y * spitfire::GetSingleton().mapsize + x]);

            if (tile->type >= FLAT)
            {
                occupiabletiles[tempstate]++;
                if (tile->type == FLAT)
                {
                    totalflats[tempstate]++;
                    if (tile->ownerid == -1)
                    {
                        openflats[tempstate]++;

                        openflatlist[tempstate].push_back(y * spitfire::GetSingleton().mapsize + x);
                    }
                    else
                    {
                        occupiedtiles[tempstate]++;
                    }
                }
                else
                {
                    occupiedtiles[tempstate]++;

                    if (tile->type == CASTLE)
                    {
                        cities[tempstate]++;
                    }
                    else
                    {
                        npcs[tempstate]++;
                    }
                }
            }
        }
    }
    for (int i = 0; i < DEF_STATES; ++i)
    {
        stats[i].numbercities = cities[i] + npcs[i];
        stats[i].playerrate = int((float(occupiedtiles[i]) / float(occupiabletiles[i])) * 100);
        stats[i].players = cities[i];
    }
}

int Map::GetRandomOpenTile(int zone)
{
    if (openflats[zone] == 0 || openflatlist[zone].size() == 0)
        return -1;
    int index = rand() % openflatlist[zone].size();
    int randomid = openflatlist[zone].at(index);
    std::vector<int32_t>::iterator iter;
    iter = openflatlist[zone].begin();
    iter += index;
    openflatlist[zone].erase(iter);
    return randomid;
}

int Map::GetStateFromXY(int x, int y)
{
    if (y >= 0 && y < spitfire::GetSingleton().mapsize*0.25)
    {
        if (x >= 0 && x < spitfire::GetSingleton().mapsize*0.25)
        {
            return 0;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.25 && x < spitfire::GetSingleton().mapsize*0.5)
        {
            return 1;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.5 && x < spitfire::GetSingleton().mapsize*0.75)
        {
            return 2;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.75 && x < spitfire::GetSingleton().mapsize)
        {
            return 3;
        }
    }
    else if (y >= spitfire::GetSingleton().mapsize*0.25 && y < spitfire::GetSingleton().mapsize*0.5)
    {
        if (x >= 0 && x < spitfire::GetSingleton().mapsize*0.25)
        {
            return 4;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.25 && x < spitfire::GetSingleton().mapsize*0.5)
        {
            return 5;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.5 && x < spitfire::GetSingleton().mapsize*0.75)
        {
            return 6;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.75 && x < spitfire::GetSingleton().mapsize)
        {
            return 7;
        }
    }
    else if (y >= spitfire::GetSingleton().mapsize*0.5 && y < spitfire::GetSingleton().mapsize*0.75)
    {
        if (x >= 0 && x < spitfire::GetSingleton().mapsize*0.25)
        {
            return 8;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.25 && x < spitfire::GetSingleton().mapsize*0.5)
        {
            return 9;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.5 && x < spitfire::GetSingleton().mapsize*0.75)
        {
            return 10;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.75 && x < spitfire::GetSingleton().mapsize)
        {
            return 11;
        }
    }
    else if (y >= spitfire::GetSingleton().mapsize*0.75 && y < spitfire::GetSingleton().mapsize)
    {
        if (x >= 0 && x < spitfire::GetSingleton().mapsize*0.25)
        {
            return 12;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.25 && x < spitfire::GetSingleton().mapsize*0.5)
        {
            return 13;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.5 && x < spitfire::GetSingleton().mapsize*0.75)
        {
            return 14;
        }
        else if (x >= spitfire::GetSingleton().mapsize*0.75 && x < spitfire::GetSingleton().mapsize)
        {
            return 15;
        }
    }
    return -1;
}

int Map::GetStateFromID(int id)
{
    GETXYFROMID(id);
    return GetStateFromXY(GETX, GETY);
}

Tile * Map::GetTileFromID(int id) const
{
    return &tile[id];
}

amf3object Map::GetTileRangeObject(int32_t clientid, int x1, int x2, int y1, int y2)
{
    amf3object data = amf3object();

    amf3array castles;

    std::string mapStr = "";

    data["x1"] = x1;
    data["x2"] = x2;
    data["y1"] = y1;
    data["y2"] = y2;

    if ((x1 > x2) || (y1 > y2))
    {
        data["ok"] = -1;
        data["errorMsg"] = "Error retrieving results. x1";
        data["packageId"] = 0.0;

        return data;
    }



    if ((abs(x1) >= spitfire::GetSingleton().mapsize) && (abs(x2) >= spitfire::GetSingleton().mapsize))
    {
        x1 = x1%spitfire::GetSingleton().mapsize;
        x2 = x2%spitfire::GetSingleton().mapsize;
    }
    else if (abs(x2) >= spitfire::GetSingleton().mapsize)
    {
        x1 -= spitfire::GetSingleton().mapsize;
        x2 = x2%spitfire::GetSingleton().mapsize;
    }
    if ((abs(y1) >= spitfire::GetSingleton().mapsize) && (abs(y2) >= spitfire::GetSingleton().mapsize))
    {
        y1 = y1%spitfire::GetSingleton().mapsize;
        y2 = y2%spitfire::GetSingleton().mapsize;
    }
    else if (abs(y2) >= spitfire::GetSingleton().mapsize)
    {
        y1 -= spitfire::GetSingleton().mapsize;
        y2 = y2%spitfire::GetSingleton().mapsize;
    }

    //gserver->consoleLogger->information(Poco::format("x1 %d x2 %d y1 %d y2 %d", x1, x2, y1, y2));

    if (x1 < 0 || y1 < 0)
    {
        int xdiff = 0;
        int ydiff = 0;
        if (x1 < 0)
        {
            if (x2 < 0)
            {
                xdiff = x2 - x1;
            }
            else
            {
                xdiff = abs(x1) + x2;
            }
        }
        else
        {
            xdiff = x2 - x1;
        }
        if (y1 < 0)
        {
            if (y2 < 0)
            {
                ydiff = y2 - y1;
            }
            else
            {
                ydiff = abs(y1) + y2;
            }
        }
        else
        {
            ydiff = y2 - y1;
        }
        if (xdiff > 20 || ydiff > 20 || xdiff <= 0 || ydiff <= 0)
        {
            data["ok"] = -1;
            data["errorMsg"] = "Out of range.";
            data["packageId"] = 0.0;

            return data;
        }
        //gserver->consoleLogger->information(Poco::format("xdiff %d ydiff %d", xdiff, ydiff));

        //valid request 20x20 max

        for (int y = y1; y <= y1 + ydiff; ++y)
        {
            for (int x = x1; x <= x1 + xdiff; ++x)
            {
                x1 = x1%spitfire::GetSingleton().mapsize;
                x2 = x2%spitfire::GetSingleton().mapsize;

                int idfromxy = y*spitfire::GetSingleton().mapsize + x;
                if ((x < 0) && (y < 0))
                {
                    idfromxy = (y + spitfire::GetSingleton().mapsize)*spitfire::GetSingleton().mapsize + (x + spitfire::GetSingleton().mapsize);
                }
                else if (x < 0)
                {
                    idfromxy = y*spitfire::GetSingleton().mapsize + (x + spitfire::GetSingleton().mapsize);
                }
                else if (y < 0)
                {
                    idfromxy = (y + spitfire::GetSingleton().mapsize)*spitfire::GetSingleton().mapsize + x;
                }
                else if ((x > spitfire::GetSingleton().mapsize) && (y > spitfire::GetSingleton().mapsize))
                {
                    idfromxy = (y%spitfire::GetSingleton().mapsize)*spitfire::GetSingleton().mapsize + ((x%spitfire::GetSingleton().mapsize) + spitfire::GetSingleton().mapsize);
                }
                else if (x > spitfire::GetSingleton().mapsize)
                {
                    idfromxy = y*spitfire::GetSingleton().mapsize + ((x%spitfire::GetSingleton().mapsize) + spitfire::GetSingleton().mapsize);
                }
                else if (y > spitfire::GetSingleton().mapsize)
                {
                    idfromxy = ((y%spitfire::GetSingleton().mapsize) + spitfire::GetSingleton().mapsize)*spitfire::GetSingleton().mapsize + x;
                }


                if (tile[idfromxy].type > 10)
                {
                    amf3object castleobject = amf3object();
                    castleobject["id"] = tile[idfromxy].id;
                    castleobject["name"] = tile[idfromxy].city->cityname.c_str();
                    castleobject["state"] = tile[idfromxy].city->status;
                    if (tile[idfromxy].npc)
                    {
                        castleobject["npc"] = true;
                    }
                    else
                    {
                        Client * client = ((PlayerCity*)tile[idfromxy].city)->client;
                        castleobject["prestige"] = client->Prestige();
                        castleobject["honor"] = client->honor;
                        castleobject["userName"] = client->playername.c_str();
                        castleobject["flag"] = client->flag.c_str();
                        castleobject["changeface"] = client->changeface;

                        if (client->allianceid > 0)
                            castleobject["allianceName"] = client->alliancename.c_str();

                        int relation = 0;// spitfire::GetSingleton().alliances->GetRelation(clientid, tile[idfromxy].ownerid);
                        switch (relation)
                        {
                        case DEF_SELFRELATION:
                        case DEF_ALLY:
                        case DEF_ALLIANCE:
                            castleobject["canLoot"] = false;
                            castleobject["canOccupy"] = false;
                            castleobject["canScout"] = false;
                            castleobject["canSend"] = true;
                            castleobject["canTrans"] = true;
                            break;
                        case DEF_ENEMY:
                            castleobject["canLoot"] = true;
                            castleobject["canOccupy"] = true;
                            castleobject["canScout"] = true;
                            castleobject["canSend"] = true;
                            castleobject["canTrans"] = true;
                            break;
                        case DEF_NEUTRAL:
                        case DEF_NORELATION:
                        default:
                            castleobject["canLoot"] = true;
                            castleobject["canOccupy"] = true;
                            castleobject["canScout"] = true;
                            castleobject["canSend"] = false;
                            castleobject["canTrans"] = false;
                            break;
                        }

                        castleobject["playerLogoUrl"] = client->faceurl;

                        castleobject["relation"] = relation;
                        castleobject["state"] = client->status;
                        castleobject["userName"] = client->playername;
                        castleobject["zoneName"] = states[GetStateFromID(idfromxy)];
                        castleobject["furlough"] = client->beginner;
                        castleobject["npc"] = false;
                    }
                    castles.Add(castleobject);

                    mapStr += (char)Utils::itoh(tile[idfromxy].type);
                    mapStr += (char)Utils::itoh(tile[idfromxy].city->level);
                }
                else
                {
                    mapStr += (char)Utils::itoh(tile[idfromxy].type);
                    mapStr += (char)Utils::itoh(tile[idfromxy].level);
                }
            }
        }
    }
    else
    {

        if ((x2 - x1) > 20 || (y2 - y1) > 20)
        {
            data["ok"] = -1;
            data["errorMsg"] = "Out of range.";
            data["packageId"] = 0.0;

            return data;
        }


        for (int y = y1; y <= y2; ++y)
        {
            for (int x = x1; x <= x2; ++x)
            {
                GETIDFROMXY(x, y);
                if (tile[idfromxy].type > 10)
                {
                    amf3object castleobject = amf3object();
                    castleobject["id"] = tile[idfromxy].id;
                    castleobject["name"] = tile[idfromxy].city->cityname.c_str();
                    castleobject["state"] = tile[idfromxy].city->status;
                    if (tile[idfromxy].npc)
                    {
                        castleobject["npc"] = true;
                    }
                    else
                    {
                        Client * client = ((PlayerCity*)tile[idfromxy].city)->client;
                        castleobject["prestige"] = client->Prestige();
                        castleobject["honor"] = client->honor;
                        castleobject["userName"] = client->playername.c_str();
                        castleobject["flag"] = client->flag.c_str();

                        if (client->allianceid > 0)
                            castleobject["allianceName"] = client->alliancename.c_str();

                        int relation = 0;// spitfire::GetSingleton().alliances->GetRelation(clientid, tile[idfromxy].ownerid);
                        switch (relation)
                        {
                        case DEF_SELFRELATION:
                        case DEF_ALLY:
                        case DEF_ALLIANCE:
                            castleobject["canLoot"] = false;
                            castleobject["canOccupy"] = false;
                            castleobject["canScout"] = false;
                            castleobject["canSend"] = true;
                            castleobject["canTrans"] = true;
                            break;
                        case DEF_ENEMY:
                            castleobject["canLoot"] = true;
                            castleobject["canOccupy"] = true;
                            castleobject["canScout"] = true;
                            castleobject["canSend"] = true;
                            castleobject["canTrans"] = true;
                            break;
                        case DEF_NEUTRAL:
                        case DEF_NORELATION:
                        default:
                            castleobject["canLoot"] = true;
                            castleobject["canOccupy"] = true;
                            castleobject["canScout"] = true;
                            castleobject["canSend"] = false;
                            castleobject["canTrans"] = false;
                            break;
                        }

                        castleobject["playerLogoUrl"] = client->faceurl;

                        castleobject["relation"] = relation;
                        castleobject["state"] = client->status;
                        castleobject["userName"] = client->playername;
                        castleobject["zoneName"] = states[GetStateFromID(idfromxy)];
                        castleobject["furlough"] = client->beginner;
                        castleobject["npc"] = false;
                    }
                    castles.Add(castleobject);

                    mapStr += (char)Utils::itoh(tile[idfromxy].type);
                    mapStr += (char)Utils::itoh(tile[idfromxy].city->level);
                }
                else
                {
                    mapStr += (char)Utils::itoh(tile[idfromxy].type);
                    mapStr += (char)Utils::itoh(tile[idfromxy].level);
                }
            }
        }
    }


    data["castles"] = castles;


    data["mapStr"] = mapStr.c_str();

    data["ok"] = 1;
    data["packageId"] = 0.0;

    return data;
}

amf3object Map::GetMapCastle(int32_t fieldid, int32_t clientid)
{
    amf3object field;

    Tile * tile = &this->tile[fieldid];

    if (tile->ownerid > 0)
    {
        Client * client = spitfire::GetSingleton().GetClient(tile->ownerid);

        if (!client)
        {
            // field "had" a client.. but no longer does? should only trigger due to some sort of data loss - mostly test purposes
            // (also deleting a city from db without resetting tiles table row causes this)
            tile->type = FLAT;

            field["allianceName"] = "";
            field["npc"] = false;
            field["canLoot"] = true;
            field["canOccupy"] = true;
            field["canScout"] = true;
            field["canSend"] = false;
            field["canTrans"] = false;
            field["zoneName"] = states[GetStateFromID(fieldid)];
            field["id"] = fieldid;
            return field;
        }

        if (tile->type < 11)
        {
            field["allianceName"] = client->alliancename;
            field["flag"] = client->flag;
            field["honor"] = client->honor;
            field["id"] = fieldid;
            //field["name"] = tile->city->cityname;
            field["prestige"] = client->Prestige();
            int relation = 0;// spitfire::GetSingleton().alliances->GetRelation(clientid, client->accountid);
            switch (relation)
            {
            case DEF_SELFRELATION:
            case DEF_ALLY:
                field["canLoot"] = false;
                field["canOccupy"] = false;
                field["canScout"] = false;
                field["canSend"] = true;
                field["canTrans"] = true;
                break;
            case DEF_ENEMY:
                field["canLoot"] = true;
                field["canOccupy"] = true;
                field["canScout"] = true;
                field["canSend"] = true;
                field["canTrans"] = true;
                break;
            case DEF_NEUTRAL:
            case DEF_NORELATION:
            default:
                field["canLoot"] = true;
                field["canOccupy"] = true;
                field["canScout"] = true;
                field["canSend"] = false;
                field["canTrans"] = false;
                break;
            }
            field["relation"] = relation;
            field["state"] = client->status;
            field["userName"] = client->playername;
            field["zoneName"] = states[GetStateFromID(fieldid)];
            field["furlough"] = client->beginner;
            field["npc"] = false;
        }
        else
        {
            field["allianceName"] = client->alliancename;
            field["flag"] = client->flag;
            field["honor"] = client->honor;
            field["id"] = fieldid;
            field["name"] = tile->city->cityname;
            field["playerLogoUrl"] = client->faceurl;
            field["prestige"] = client->Prestige();
            int relation = 0;// spitfire::GetSingleton().alliances->GetRelation(clientid, client->accountid);
            switch (relation)
            {
            case DEF_SELFRELATION:
            case DEF_ALLY:
                field["canLoot"] = false;
                field["canOccupy"] = false;
                field["canScout"] = false;
                field["canSend"] = true;
                field["canTrans"] = true;
                break;
            case DEF_ENEMY:
                field["canLoot"] = true;
                field["canOccupy"] = true;
                field["canScout"] = true;
                field["canSend"] = true;
                field["canTrans"] = true;
                break;
            case DEF_NEUTRAL:
            case DEF_NORELATION:
            default:
                field["canLoot"] = true;
                field["canOccupy"] = true;
                field["canScout"] = true;
                field["canSend"] = false;
                field["canTrans"] = false;
                break;
            }
            field["relation"] = relation;
            field["state"] = client->status;
            field["userName"] = client->playername;
            field["zoneName"] = states[GetStateFromID(fieldid)];
            field["furlough"] = client->beginner;
            field["npc"] = false;
        }
    }
    else if (tile->npc)
    {
        field["allianceName"] = "";
        field["npc"] = true;
        field["canLoot"] = true;
        field["canOccupy"] = true;
        field["canScout"] = true;
        field["canSend"] = false;
        field["canTrans"] = false;
        field["zoneName"] = states[GetStateFromID(fieldid)];
        field["id"] = fieldid;
        field["name"] = tile->city->cityname;
    }
    else
    {
        field["allianceName"] = "";
        field["npc"] = false;
        field["canLoot"] = true;
        field["canOccupy"] = true;
        field["canScout"] = true;
        field["canSend"] = false;
        field["canTrans"] = false;
        field["zoneName"] = states[GetStateFromID(fieldid)];
        field["id"] = fieldid;
    }

    return field;
}
