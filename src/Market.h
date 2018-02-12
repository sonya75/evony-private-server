/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "structs.h"
#include <mutex>


class spitfire;

class Market
{
public:
	Market();
	~Market();
	bool init();
	std::list<stMarketEntry> * getFoodBuy();
	std::list<stMarketEntry> * getFoodSell();
	std::list<stMarketEntry> * getWoodBuy();
	std::list<stMarketEntry> * getWoodSell();
	std::list<stMarketEntry> * getStoneBuy();
	std::list<stMarketEntry> * getStoneSell();
	std::list<stMarketEntry> * getIronBuy();
	std::list<stMarketEntry> * getIronSell();
	void recalculateCache();
	void Process();

	static bool compareprice(stMarketEntry & first, stMarketEntry & second)
	{
		if (first.price > second.price)
		{
			if (first.time < second.time)
				return true;
			else
				return false;
		}
		else
			return false;
	}

	static bool rcompareprice(stMarketEntry & first, stMarketEntry & second)
	{
		if (first.price < second.price)
		{
			if (first.time > second.time)
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool addFoodBuy(stMarketEntry entry);
	bool addFoodSell(stMarketEntry entry);
	bool addWoodBuy(stMarketEntry entry);
	bool addWoodSell(stMarketEntry entry);
	bool addStoneBuy(stMarketEntry entry);
	bool addStoneSell(stMarketEntry entry);
	bool addIronBuy(stMarketEntry entry);
	bool addIronSell(stMarketEntry entry);

private:

	std::mutex m;
	std::mutex mcache;

	uint64_t tradeid;

	struct EntryList
	{
		std::list<stMarketEntry> foodbuy;
		std::list<stMarketEntry> foodsell;

		std::list<stMarketEntry> woodbuy;
		std::list<stMarketEntry> woodsell;

		std::list<stMarketEntry> ironbuy;
		std::list<stMarketEntry> ironsell;

		std::list<stMarketEntry> stonebuy;
		std::list<stMarketEntry> stonesell;
    };

    EntryList live;
    EntryList queue_;

public:
	struct
	{
		std::list<stMarketEntry> foodbuy;
		std::list<stMarketEntry> foodsell;

		std::list<stMarketEntry> woodbuy;
		std::list<stMarketEntry> woodsell;

		std::list<stMarketEntry> ironbuy;
		std::list<stMarketEntry> ironsell;

		std::list<stMarketEntry> stonebuy;
		std::list<stMarketEntry> stonesell;

		uint64_t buytime;
		uint64_t selltime;
	} cache;
};
