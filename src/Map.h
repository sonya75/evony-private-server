/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "amf3.h"
#include "structs.h"
#include "defines.h"
#include <string>

class Tile;
class spitfire;

class Map
{
public:
	Map(uint32_t size);
	~Map();

	uint32_t mapsize;
	std::string states[16] = { DEF_STATE1, DEF_STATE2, DEF_STATE3, DEF_STATE4, DEF_STATE5, DEF_STATE6, DEF_STATE7, DEF_STATE8, DEF_STATE9, DEF_STATE10, DEF_STATE11, DEF_STATE12, DEF_STATE13, DEF_STATE14, DEF_STATE15, DEF_STATE16 };

	void CalculateOpenTiles();
	int GetStateFromXY(int x, int y);
	int GetStateFromID(int id);
	int GetRandomOpenTile(int zone);
	amf3object GetTileRangeObject(int32_t clientid, int x1, int x2, int y1, int y2);
	amf3object GetMapCastle(int32_t fieldid, int32_t clientid);
	Tile * GetTileFromID(int id) const;

	bool AddCity(int id, City * city);

	Tile * m_tile;
	uint16_t m_totalflats[DEF_STATES];
    uint16_t m_openflats[DEF_STATES];
    uint16_t m_npcs[DEF_STATES];
    uint16_t m_cities[DEF_STATES];
    uint16_t m_occupiedtiles[DEF_STATES];
    uint16_t m_occupiabletiles[DEF_STATES];
	struct mapstats
	{
		int players;
		int numbercities;
		int playerrate;
	} m_stats[DEF_STATES];

	std::vector<int32_t> m_openflatlist[DEF_STATES];
};
