/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "City.h"
#include "structs.h"


class Client;


class PlayerCity : public City
{
public:
	PlayerCity();
	~PlayerCity(void);

	Client * m_client;
	uint64_t m_castleid;
	int64_t m_accountid;
	std::string m_logurl;
	int32_t m_population;
	uint32_t m_availablepopulation;
	uint32_t m_maxpopulation;
	bool m_allowalliance;
	bool m_gooutforbattle;
	bool m_hasenemy;
	double m_creation;
	bool m_researching;

	double m_lastcomfort;
	double m_lastlevy;

	stTroops m_troops, m_injuredtroops; // size 44 bytes

	stResources m_production;
	stResources m_workpopulation;
	stResources m_workrate;
	stResources m_storepercent;

	double m_productionefficiency;
	int32_t m_resourcebaseproduction;
	double m_resourcemanagement;
	int32_t m_resourcetech;
	int32_t m_resourcevalley;

	double m_troopconsume;

	struct stTimers
	{
		double updateresources;
	} m_timers;

	Hero * m_heroes[10]; // 75 bytes * 10
	Hero * m_innheroes[10]; // 75 bytes * 10

	Hero * m_mayor;

    std::string DBFortifications() const;
    std::string DBBuildings() const;
    std::string DBTransingtrades() const;
    std::string DBTrades() const;
    std::string DBTroops() const;
    std::string DBMisc() const;
	std::string DBTroopQueues() const;
	bool SaveToDB();

	amf3object ToObject();
	amf3array Buildings() const;
	amf3object Troops() const;
	amf3object InjuredTroops() const;
	amf3object Resources();
	amf3array HeroArray();
	amf3object Fortifications() const;

	void SetTroops(int8_t type, int64_t amount);
	void SetForts(int32_t type, int32_t count);
	int8_t SetTroopQueue(int32_t position, int32_t troopid, int32_t count, int64_t costtime, int64_t endtime=0);
	int64_t GetTroops(int8_t type) const;
	Hero * GetHero(uint64_t id);
	bool HasTroops(stTroops & troops) const;

	void ParseBuildings(std::string str);
	void ParseTroops(std::string str);
	void ParseTroopQueues(std::string str);
	void ParseFortifications(std::string str);
	void ParseMisc(std::string str);

	bool CheckBuildingPrereqs(int16_t type, int16_t level);


	void CalculateStats();
	void CalculateResources();
	void RecalculateCityStats();
	void CalculateResourceStats();


	void CastleUpdate();
	void ResourceUpdate();
	void HeroUpdate(Hero * hero, int16_t updatetype) const;
	void TroopUpdate() const;
	void FortUpdate() const;
	void TradesUpdate(uint64_t amount, uint64_t id, int32_t tradetype, double price, std::string tradetypename, uint64_t dealedtotal, std::string resourcename, int32_t restype, int32_t updatetype) const;

	int16_t GetReliefMultiplier();


	amf3array ResourceProduceData() const;



	std::vector<stTroopQueue> m_troopqueue;

	std::list<stArmyMovement*> armymovement;

	int16_t HeroCount();
	stTroopQueue * GetBarracksQueue(int16_t position);
	int8_t AddToBarracksQueue(int8_t position, int16_t troopid, int32_t count, bool isshare, bool isidle);


	int16_t GetBuildingLevel(int16_t id);
	stBuilding * GetBuilding(int16_t position);
	int16_t GetEffectiveTechLevel(int16_t id);
	int16_t GetBuildingCount(int16_t id);

	stTrade * GetTrade(int64_t id);
	stTransingTrade * GetTransingTrade(int64_t id);

	std::list<stTrade> m_trades;
	std::list<stTransingTrade> m_transingtrades;

}; // 3,448 + base
