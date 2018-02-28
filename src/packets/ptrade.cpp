/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "ptrade.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"


ptrade::ptrade(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

ptrade::~ptrade()
{

}

void ptrade::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if ((command == "searchTrades"))
    {
        int restype = data["resType"];

        if (restype < 0 || restype > 3)
        {
            gserver.SendObject(client, gserver.CreateError("trade.searchTrades", -99, "Not a valid resource type."));
            return;
        }


        obj2["cmd"] = "trade.searchTrades";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        amf3array sellers;
        amf3array buyers;


        int count = 0;

        if (restype == 0)
        {
            for (auto buy : *gserver.market.getFoodBuy())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                buyers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }

            for (auto buy : *gserver.market.getFoodSell())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                sellers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }
        }
        else if (restype == 1)
        {
            for (auto buy : *gserver.market.getWoodBuy())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                buyers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }

            for (auto buy : *gserver.market.getWoodSell())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                sellers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }
        }
        else if (restype == 2)
        {
            for (auto buy : *gserver.market.getStoneBuy())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                buyers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }

            for (auto buy : *gserver.market.getStoneSell())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                sellers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }
        }
        else if (restype == 3)
        {
            for (auto buy : *gserver.market.getIronBuy())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                buyers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }

            for (auto buy : *gserver.market.getIronSell())
            {
                amf3object obj;
                obj["amount"] = buy.amount;
                obj["price"] = buy.price;
                sellers.Add(obj);
                ++count;
                if (count >= 5)
                    break;
            }
        }
        data2["sellers"] = sellers;
        data2["buyers"] = buyers;

        gserver.SendObject(client, obj2);
        return;
    }
    else if (command == "newTrade")
    {
        int restype = data["resType"];

        if (restype < 0 || restype > 3)
        {
            gserver.SendObject(client, gserver.CreateError("trade.newTrade", -99, "Not a valid resource type."));
            return;
        }

        obj2["cmd"] = "trade.newTrade";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        stMarketEntry entry;
        entry.amount = data["amount"];
        entry.city = client->GetFocusCity();
        entry.client = client;
        entry.price = atoi((data["price"]).c_str());

        if ((int)data["tradeType"] != 1 && client->GetFocusCity()->resources.gold < entry.amount*entry.price)
        {
            gserver.SendObject(client, gserver.CreateError("trade.newTrade", -99, "Not enough gold."));
            return;
        }

        uint64_t t1 = Utils::time();
        switch (int(data["resType"]))
        {
        case 0:
            //food
            if ((int)data["tradeType"] == 1)//sell
                gserver.market.addFoodSell(entry);//1
            else
                gserver.market.addFoodBuy(entry);//0
            break;
        case 1:
            //wood
            if ((int)data["tradeType"] == 1)//sell
                gserver.market.addWoodSell(entry);
            else
                gserver.market.addWoodBuy(entry);
            break;
        case 2:
            //stone
            if ((int)data["tradeType"] == 1)//sell
                gserver.market.addStoneSell(entry);
            else
                gserver.market.addStoneBuy(entry);
            break;
        case 3:
            //iron
            if ((int)data["tradeType"] == 1)//sell
                gserver.market.addIronSell(entry);
            else
                gserver.market.addIronBuy(entry);
            break;
        }
        gserver.SendObject(client, obj2);

        uint64_t t2 = Utils::time();
        spitfire::GetSingleton().log->error("Market packet time: {}", t2 - t1);
/*
        //test stuff
        {
            amf3object obj;
            amf3object data;

            obj["cmd"] = "server.TransingTradeUpdate";
            data["castleId"] = client->currentcityid;
            amf3object bean;
            bean["amount"] = 5;
            bean["total"] = 100;
            bean["id"] = rand();
            bean["price"] = rand();
            bean["endTime"] = unixtime() + 1000*60*30;
            bean["resourceName"] = "Custom names!";
            bean["resType"] = 0;
            data["bean"] = bean;

            obj["data"] = data;
            gserver.SendObject(client, obj);
        }*/
    }
}
