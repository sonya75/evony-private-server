/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pinterior.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"


pinterior::pinterior(spitfire & server, request & req, amf3object & obj)
	: packet(server, req, obj)
{

}

pinterior::~pinterior()
{

}

void pinterior::process()
{
	obj2["data"] = amf3object();
	amf3object & data2 = obj2["data"];

	if ((command == "modifyTaxRate"))
	{
		VERIFYCASTLEID();
		CHECKCASTLEID();

		uint32_t castleid = data["castleId"];

		int temp = data["tax"];
		if (temp < 0 || temp > 100)
		{
			city->m_workrate.gold = 0;
			// TODO error reporting - interior.modifyTaxRate
		}
		else
		{
			city->m_workrate.gold = data["tax"];
		}

		obj2["cmd"] = "interior.modifyTaxRate";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;

		gserver.SendObject(client, obj2);

		city->CalculateStats();
		city->CalculateResources();
		city->ResourceUpdate();

		client->SaveToDB();
		city->SaveToDB();

		return;
	}
	if ((command == "pacifyPeople"))
	{
		VERIFYCASTLEID();
		CHECKCASTLEID();

		uint32_t castleid = data["castleId"];

		if (timestamp - city->m_lastcomfort < 15 * 60 * 1000)
		{
			{
				std::stringstream ss;
				double timediff = city->m_lastcomfort + 15 * 60 * 1000 - timestamp;
				int min = timediff / 1000 / 60;
				int sec = int(((((timediff) / 1000) / 60) - min) * 60);
				ss << min << "m ";
				ss << sec << "s ";
				ss << "interval needed for next comforting.";
				gserver.SendObject(client, gserver.CreateError("interior.pacifyPeople", -34, ss.str()));
			}
			return;
		}

		int itypeid = data["typeId"];
		city->m_lastcomfort = timestamp;

		obj2["cmd"] = "interior.pacifyPeople";
		data2["packageId"] = 0.0;
		data2["ok"] = -99;

		switch (itypeid)
		{
			case 1://Disaster Relief 100% pop limit in food for cost, increases loyalty by 5 reduces grievance by 15
				if (city->m_resources.food < city->m_maxpopulation)
				{
					gserver.SendObject(client, gserver.CreateError("interior.pacifyPeople", -99, "Not enough food."));
					return;
				}
				city->m_resources.food -= city->m_maxpopulation;
				city->m_loyalty += 5;
				city->m_grievance -= 15;
				if (city->m_loyalty > 100)
					city->m_loyalty = 100;
				if (city->m_grievance < 0)
					city->m_grievance = 0;
				city->ResourceUpdate();
				client->PlayerUpdate();
				break;
			case 2://Praying 100% pop limit in food for cost, increases loyalty by 25 reduces grievance by 5
				if (city->m_resources.food < city->m_maxpopulation)
				{
					gserver.SendObject(client, gserver.CreateError("interior.pacifyPeople", -99, "Not enough food."));
					return;
				}
				city->m_resources.food -= city->m_maxpopulation;
				city->m_loyalty += 25;
				city->m_grievance -= 5;
				if (city->m_loyalty > 100)
					city->m_loyalty = 100;
				if (city->m_grievance < 0)
					city->m_grievance = 0;
				city->ResourceUpdate();
				client->PlayerUpdate();
				break;
			case 3://Blessing 10% pop limit in gold for cost, increases food by 100% pop limit - chance for escaping disaster?
				if (city->m_resources.gold < (city->m_maxpopulation / 10))
				{
					gserver.SendObject(client, gserver.CreateError("interior.pacifyPeople", -99, "Not enough gold."));
					return;
				}
				city->m_resources.gold -= (city->m_maxpopulation / 10);
				city->m_resources.food += city->m_maxpopulation;
				if (rand() % 10 == 1)
				{
					city->m_resources.food += city->m_maxpopulation;
					gserver.SendObject(client, gserver.CreateError("interior.pacifyPeople", -99, "Free blessing!"));

					city->ResourceUpdate();
					client->PlayerUpdate();

					client->SaveToDB();
					city->SaveToDB();

					return;
				}
				city->ResourceUpdate();
				client->PlayerUpdate();
				break;
			case 4://Population Raising 500% pop limit in food for cost, increases population by 5%
				if (city->m_resources.food < (city->m_maxpopulation * 5))
				{
					gserver.SendObject(client, gserver.CreateError("interior.pacifyPeople", -99, "Not enough food."));
					return;
				}
				city->m_resources.food -= (city->m_maxpopulation * 5);
				city->m_population += double(city->m_maxpopulation) / 20;
				if (city->m_population >= city->m_maxpopulation)
					city->m_population = city->m_maxpopulation;
				city->ResourceUpdate();
				client->PlayerUpdate();
				break;
		}

		// TODO finish - interior.pacifyPeople
		obj2["cmd"] = "interior.pacifyPeople";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;

		gserver.SendObject(client, obj2);

		client->SaveToDB();
		city->SaveToDB();

		return;
	}
	if ((command == "taxation"))
	{
		VERIFYCASTLEID();
		CHECKCASTLEID();

		uint32_t castleid = data["castleId"];

		if (timestamp - city->m_lastlevy < 15 * 60 * 1000)
		{
			{
				std::stringstream ss;
				double timediff = city->m_lastlevy + 15 * 60 * 1000 - timestamp;
				int min = timediff / 1000 / 60;
				int sec = int(((((timediff) / 1000) / 60) - min) * 60);
				ss << min << "m ";
				ss << sec << "s ";
				ss << "interval needed for next levy.";
				gserver.SendObject(client, gserver.CreateError("interior.taxation", -34, ss.str()));
			}
			return;
		}

		int itypeid = data["typeId"];
		city->m_lastlevy = timestamp;

		obj2["cmd"] = "interior.taxation";
		data2["packageId"] = 0.0;
		data2["ok"] = -99;

		if (city->m_loyalty <= 20) //not enough loyalty to levy
		{
			gserver.SendObject(client, gserver.CreateError("interior.taxation", -99, "Loyalty too low. Please comfort first."));
			return;
		}
		city->m_loyalty -= 20;

		switch (itypeid)
		{
			case 1://Gold 10% current pop
				city->m_resources.gold += (city->m_population / 10);
				break;
			case 2://Food 100% current pop
				city->m_resources.food += city->m_population;
				break;
			case 3://Wood 100% current pop 
				city->m_resources.wood += city->m_population;
				break;
			case 4://Stone 50% current pop 
				city->m_resources.stone += (city->m_population / 2);
				break;
			case 5://Iron 40% current pop 
				city->m_resources.iron += (city->m_population*0.40);
				break;
		}
		city->ResourceUpdate();
		client->PlayerUpdate();

		// TODO finish - interior.taxation
		obj2["cmd"] = "interior.taxation";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;

		gserver.SendObject(client, obj2);

		client->SaveToDB();
		city->SaveToDB();

		return;
	}
	if ((command == "modifyCommenceRate"))
	{
		VERIFYCASTLEID();
		CHECKCASTLEID();

		uint32_t castleid = data["castleId"];

		int temp = data["foodrate"];

		city->CalculateStats();
		city->CalculateResources();

		if (temp < 0 || temp > 100)
		{
			city->m_workrate.food = 0;
			// TODO error reporting - interior.modifyCommenceRate
		}
		else
		{
			city->m_workrate.food = data["foodrate"];
		}
		temp = data["woodrate"];
		if (temp < 0 || temp > 100)
		{
			city->m_workrate.wood = 0;
			// TODO error reporting - interior.modifyCommenceRate
		}
		else
		{
			city->m_workrate.wood = data["woodrate"];
		}
		temp = data["ironrate"];
		if (temp < 0 || temp > 100)
		{
			city->m_workrate.iron = 0;
			// TODO error reporting - interior.modifyCommenceRate
		}
		else
		{
			city->m_workrate.iron = data["ironrate"];
		}
		temp = data["stonerate"];
		if (temp < 0 || temp > 100)
		{
			city->m_workrate.stone = 0;
			// TODO error reporting - interior.modifyCommenceRate
		}
		else
		{
			city->m_workrate.stone = data["stonerate"];
		}

		obj2["cmd"] = "interior.modifyCommenceRate";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;

		gserver.SendObject(client, obj2);

		city->CalculateStats();
		city->ResourceUpdate();

		client->SaveToDB();
		city->SaveToDB();

		return;
	}
	if ((command == "getResourceProduceData"))
	{
		VERIFYCASTLEID();
		CHECKCASTLEID();

		uint32_t castleid = data["castleId"];

		obj2["cmd"] = "interior.getResourceProduceData";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;
		data2["resourceProduceDataBean"] = city->ResourceProduceData();

		gserver.SendObject(client, obj2);
		return;
	}
}

