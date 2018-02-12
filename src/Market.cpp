/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "Market.h"
#include "spitfire.h"
#include "Client.h"
#include "PlayerCity.h"

Market::Market()
{
	cache.buytime = 0;
	cache.selltime = 0;
	tradeid = 0;
}

Market::~Market()
{

}

bool Market::init()
{
	//load saved market values
	return true;
}

bool Market::addFoodBuy(stMarketEntry entry)
{
	std::lock_guard<std::mutex> l(mcache);
	entry.time = Utils::time();

	if (entry.client == nullptr)
	{
        spitfire::GetSingleton().log->error("addFoodBuy client error");
		return false;
	}
	if (entry.city == nullptr)
	{
        spitfire::GetSingleton().log->error("addFoodBuy city error");
		return false;
	}
	stTrade trade = { entry.amount, entry.price, ++this->tradeid, 0/*trade type 0=buy 1=sell*/, "Bid", 0, "Food", 1, entry.client, entry.city };
    entry.tradeid = trade.id;
	trade.entry = entry;

	queue_.foodbuy.push_back(entry);

	entry.city->m_trades.push_back(trade);
	return true;
}

bool Market::addFoodSell(stMarketEntry entry)
{
    std::lock_guard<std::mutex> l(mcache);
    entry.time = Utils::time();
	if (entry.client == nullptr)
	{
		spitfire::GetSingleton().log->error("addFoodSell client error");
		return false;
	}
	if (entry.city == nullptr)
	{
        spitfire::GetSingleton().log->error("addFoodSell city error");
		return false;
	}
	stTrade trade = { entry.amount, entry.price, ++this->tradeid, 1, "Offer", 0, "Food", 1, entry.client, entry.city };
    entry.tradeid = trade.id;
	trade.entry = entry;

	queue_.foodsell.push_back(entry);

	entry.city->m_trades.push_back(trade);
	return true;
}

bool Market::addWoodBuy(stMarketEntry entry)
{
    std::lock_guard<std::mutex> l(mcache);
    entry.time = Utils::time();
	return true;
}

bool Market::addWoodSell(stMarketEntry entry)
{
    std::lock_guard<std::mutex> l(mcache);
    entry.time = Utils::time();
	return true;
}

bool Market::addStoneBuy(stMarketEntry entry)
{
    std::lock_guard<std::mutex> l(mcache);
    entry.time = Utils::time();
	return true;
}

bool Market::addStoneSell(stMarketEntry entry)
{
    std::lock_guard<std::mutex> l(mcache);
    entry.time = Utils::time();
	return true;
}

bool Market::addIronBuy(stMarketEntry entry)
{
	std::lock_guard<std::mutex> l(mcache);
	entry.time = Utils::time();
	return true;
}

bool Market::addIronSell(stMarketEntry entry)
{
	std::lock_guard<std::mutex> l(mcache);
	entry.time = Utils::time();
	return true;
}

std::list<stMarketEntry> * Market::getFoodBuy()
{
	auto time = Utils::time();
	if (time > cache.buytime + (5 * 1000))
		recalculateCache();
	return &cache.foodbuy;
}

std::list<stMarketEntry> * Market::getFoodSell()
{
	auto time = Utils::time();
	if (time > cache.selltime + (5 * 1000))
		recalculateCache();
	return &cache.foodsell;
}

std::list<stMarketEntry> * Market::getWoodBuy()
{
	auto time = Utils::time();
	if (time > cache.buytime + (5 * 1000))
		recalculateCache();
	return &cache.woodbuy;
}

std::list<stMarketEntry> * Market::getWoodSell()
{
	auto time = Utils::time();
	if (time > cache.selltime + (5 * 1000))
		recalculateCache();
	return &cache.woodsell;
}

std::list<stMarketEntry> * Market::getStoneBuy()
{
	auto time = Utils::time();
	if (time > cache.buytime + (5 * 1000))
		recalculateCache();
	return &cache.stonebuy;
}

std::list<stMarketEntry> * Market::getStoneSell()
{
	auto time = Utils::time();
	if (time > cache.selltime + (5 * 1000))
		recalculateCache();
	return &cache.stonesell;
}

std::list<stMarketEntry> * Market::getIronBuy()
{
	auto time = Utils::time();
	if (time > cache.buytime + (5 * 1000))
		recalculateCache();
	return &cache.ironbuy;
}

std::list<stMarketEntry> * Market::getIronSell()
{
	auto time = Utils::time();
	if (time > cache.selltime + (5 * 1000))
		recalculateCache();
	return &cache.ironsell;
}

void Market::recalculateCache()
{
	std::lock_guard<std::mutex> l(m);
	std::lock_guard<std::mutex> l2(mcache);

	cache.foodbuy.clear();  cache.foodsell.clear();//  live.foodbuy.clear();  live.foodsell.clear();
	cache.woodbuy.clear();  cache.woodsell.clear();//  live.woodbuy.clear();  live.woodsell.clear();
	cache.stonebuy.clear(); cache.stonesell.clear();// live.stonebuy.clear(); live.stonesell.clear();
	cache.ironbuy.clear();  cache.ironsell.clear();//  live.ironbuy.clear();  live.ironsell.clear();

	int foodbuyentries = 0, foodsellentries = 0;
	int woodbuyentries = 0, woodsellentries = 0;
	int stonebuyentries = 0, stonesellentries = 0;
	int ironbuyentries = 0, ironsellentries = 0;

	for (auto e : queue_.foodbuy)
		live.foodbuy.push_front(e);
	for (auto e : queue_.foodsell)
		live.foodsell.push_front(e);
	for (auto e : queue_.woodbuy)
		live.woodbuy.push_front(e);
	for (auto e : queue_.woodsell)
		live.woodsell.push_front(e);
	for (auto e : queue_.stonebuy)
		live.stonebuy.push_front(e);
	for (auto e : queue_.stonesell)
		live.stonesell.push_front(e);
	for (auto e : queue_.ironbuy)
		live.ironbuy.push_front(e);
	for (auto e : queue_.ironsell)
		live.ironsell.push_front(e);

	queue_.foodbuy.clear();
	queue_.foodsell.clear();
	queue_.woodbuy.clear();
	queue_.woodsell.clear();
	queue_.stonebuy.clear();
	queue_.stonesell.clear();
	queue_.ironbuy.clear();
	queue_.ironsell.clear();


	//access sell opposite of buy
	live.foodbuy.sort(compareprice);live.foodsell.sort(rcompareprice);
	live.woodbuy.sort(compareprice);live.woodsell.sort(rcompareprice);
	live.stonebuy.sort(compareprice);live.stonesell.sort(rcompareprice);
	live.ironbuy.sort(compareprice);live.ironsell.sort(rcompareprice);


	bool test = false;
	//food
	for (auto entry : live.foodbuy)
	{
		for (auto & cacheentry : cache.foodbuy)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.foodbuy.push_back(newentry);
			++foodbuyentries;
            test = false;
        }
		if (foodbuyentries > 5)
			break;
	}

    test = false;
    for (auto entry : live.foodsell)
	{
		for (auto & cacheentry : cache.foodsell)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.foodsell.push_back(newentry);
			++foodsellentries;
            test = false;
        }
		if (foodsellentries > 5)
			break;
	}

	//wood
	for (auto entry : live.woodbuy)
	{
		for (auto & cacheentry : cache.woodbuy)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.foodbuy.push_back(newentry);
			++woodbuyentries;
		}
		if (woodbuyentries > 5)
			break;
	}

	for (auto entry : live.woodsell)
	{
		for (auto & cacheentry : cache.woodsell)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.woodsell.push_back(newentry);
			++woodsellentries;
		}
		if (woodsellentries > 5)
			break;
	}

	//stone
	for (auto entry : live.stonebuy)
	{
		for (auto & cacheentry : cache.stonebuy)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.foodbuy.push_back(newentry);
			++stonebuyentries;
		}
		if (stonebuyentries > 5)
			break;
	}

	for (auto entry : live.stonesell)
	{
		for (auto & cacheentry : cache.stonesell)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.stonesell.push_back(newentry);
			++stonesellentries;
		}
		if (stonesellentries > 5)
			break;
	}

	//iron
	for (auto entry : live.ironbuy)
	{
		for (auto & cacheentry : cache.ironbuy)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.ironbuy.push_back(newentry);
			++ironbuyentries;
		}
		if (ironbuyentries > 5)
			break;
	}

	for (auto entry : live.ironsell)
	{
		for (auto & cacheentry : cache.ironsell)
		{
			if (entry.price == cacheentry.price)
			{
				cacheentry.amount += entry.amount;
				test = true;
				break;
			}
		}
		if (!test)
		{
			stMarketEntry newentry;
			newentry.amount = entry.amount;
			newentry.price = entry.price;
			cache.ironsell.push_back(newentry);
			++ironsellentries;
		}
		if (ironsellentries > 5)
			break;
	}

	auto time = Utils::time();
	cache.buytime = time;
	cache.selltime = time;
}

void Market::Process()
{
	std::lock_guard<std::mutex> l(m);
	//process food entries for successful transfers
	for (auto & entry : live.foodsell)
	{
		for (auto & buy : live.foodbuy)
		{
			if (buy.price >= entry.price)
			{
				//trade found
				if (buy.amount >= entry.amount)
				{
					//buying more than amount available
					buy.amount -= entry.amount;

					//send entry->city the gold
					entry.city->m_resources.gold += entry.amount*entry.price;
					entry.city->ResourceUpdate();
					entry.amount = 0;
					for (auto & trade : entry.city->m_trades)
					{
						if (trade.id == entry.tradeid)
						{
							entry.city->m_trades.remove(trade);
							break;
						}
					}
					//entry->city->TradesUpdate();

					//send buy->city the resources

					buy.client;
					buy.city;

					//city->m_trades
				}
				else
				{
					//buying less than amount available
					entry.amount -= buy.amount;
					buy.amount = 0;

					buy.client;
					buy.city;
				}
			}
		}
	}
}

