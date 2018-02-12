/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "structs.h"


class Client;


class City
{
public:
	City();
	~City();

	bool SetBuilding(int16_t type, int8_t level, int16_t position, int16_t status = 0, double starttime = 0, double endtime = 0);
	void SetResources(double gold, double food, double wood, double stone, double iron);
	void SetMaxResources(double gold, double food, double wood, double stone, double iron);
	void SetForts(int32_t traps, int32_t abatis, int32_t towers, int32_t logs, int32_t trebs);
	int32_t GetForts(int32_t type) const;

	std::string m_cityname;
	int8_t m_status;
	int32_t m_tileid;
	int8_t m_type;
	int8_t m_level;

	int8_t m_loyalty;
	int8_t m_grievance;

	stResources m_resources;
	stResources m_maxresources;

	stForts m_forts;
	
	stBuilding m_innerbuildings[35]; // 735 bytes
	stBuilding m_outerbuildings[41]; // 840 bytes
									 // 
									 // 	public bool m_npc = true;
									 // 	public Client m_client = null;
									 // 	public int m_herocount = 0;
									 // 	public ArrayList m_heroes = new ArrayList();
									 // 	public bool m_allowAlliance = false;
									 // 	//public ArrayList m_buildings = new ArrayList();
									 // 	public Building[] m_buildings;
};
