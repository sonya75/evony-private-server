/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pshop.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../Hero.h"


pshop::pshop(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pshop::~pshop()
{

}

void pshop::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if ((command == "buy"))
    {
        int64_t amount = data["amount"];
        std::string itemid = static_cast<std::string>(data["itemId"]);

        if (amount <= 0)
        {
            //TODO: while I realize this should be caught as a cheat attempt, for now just error it
            gserver.SendObject(client, gserver.CreateError("shop.buy", -28, "Not going to happen."));
            return;
        }

        for (int i = 0; i < DEF_MAXITEMS; ++i)
        {
            if (itemid == gserver.items[i].name)
            {
                if (client->cents < gserver.items[i].cost*amount)
                {
                    gserver.SendObject(client, gserver.CreateError("shop.buy", -28, "Insufficient game coins."));
                    return;
                }
                client->cents -= gserver.items[i].cost*amount;

                client->AddItem(itemid, amount);

                client->PlayerUpdate();

                obj2["cmd"] = "shop.buy";
                data2["packageId"] = 0.0;
                data2["ok"] = 1;

                gserver.SendObject(client, obj2);

                client->SaveToDB();

                return;
            }
        }
        gserver.SendObject(client, gserver.CreateError("shop.buy", -99, "Item does not exist."));
        return;
    }
    if ((command == "getBuyResourceInfo"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();


        int amount = data["amount"];
        //string itemid = static_cast<string>(data["itemId"]);

        obj2["cmd"] = "shop.getBuyResourceInfo";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        amf3object buyresource = amf3object();
        buyresource["woodRemain"] = 10000000;
        buyresource["forWood"] = 100000;
        buyresource["stoneRemain"] = 10000000;
        buyresource["forStone"] = 50000;
        buyresource["ironRemain"] = 10000000;
        buyresource["forIron"] = 40000;
        buyresource["foodRemain"] = 10000000;
        buyresource["forFood"] = 100000;

        data2["buyResourceBean"] = buyresource;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "buyResource"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        int fooduse = data["foodUse"];
        int wooduse = data["woodUse"];
        int stoneuse = data["stoneUse"];
        int ironuse = data["ironUse"];

        if (fooduse + wooduse + stoneuse + ironuse > client->cents)
        {
            gserver.SendObject(client, gserver.CreateError("shop.buyResource", -24, "Insufficient game coins."));
            return;
        }

        client->cents -= fooduse + wooduse + stoneuse + ironuse;
        city->resources.food += fooduse * 100000;
        city->resources.wood += wooduse * 100000;
        city->resources.stone += stoneuse * 50000;
        city->resources.iron += ironuse * 40000;

        obj2["cmd"] = "shop.buyResource";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        client->PlayerUpdate();
        city->ResourceUpdate();

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "useGoods"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t num = data["num"];
        std::string itemid = static_cast<std::string>(data["itemId"]);

        if (client->GetItemCount(itemid) < num)
        {
            gserver.SendObject(client, gserver.CreateError("shop.useGoods", -24, "Insufficient items."));
            return;
        }

        //other errors?
        //timed items
        //{
        //    gserver.SendObject(client, gserver.CreateError("shop.useGoods", -132, "You can only use this item between Wed Feb 06 00:00:00 CST 2013 and Thu Feb 21 00:00:00 CST 2013"));
        //}
        ShopUseGoods(data, client);

        client->SaveToDB();

        return;
    }
    if ((command == "useCastleGoods"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        uint32_t num = data["num"];
        std::string itemid = static_cast<std::string>(data["itemId"]);

        if (client->GetItemCount(itemid) < num)
        {
            gserver.SendObject(client, gserver.CreateError("shop.useGoods", -24, "Insufficient items."));
            return;
        }

        //other errors?
        //timed items
        //{
        //    gserver.SendObject(client, gserver.CreateError("shop.useGoods", -132, "You can only use this item between Wed Feb 06 00:00:00 CST 2013 and Thu Feb 21 00:00:00 CST 2013"));
        //}
        ShopUseCastleGoods(data, client);

        client->SaveToDB();

        return;
    }
}

void pshop::ShopUseGoods(amf3object & data, Client * client)
{
    amf3object obj2 = amf3object();
    obj2["cmd"] = "shop.useGoods";
    amf3object & data2 = obj2["data"];
    data2["packageId"] = 0.0;
    data2["ok"] = 1;

    uint32_t num = data["num"];
    std::string itemid = static_cast<std::string>(data["itemId"]);

    gserver.log->info("usegoods: {}", itemid);

    //Maybe tokenize instead?
    //     char * cmdtype, * command, * ctx;
    //     cmdtype = strtok_s(cmd, ".", &ctx);
    //     command = strtok_s(NULL, ".", &ctx);
    // 
    //     if ((cmdtype == "gameClient"))
    //     {
    //         if ((command == "version"))
    //         {
    //         }
    //     }

    if (itemid == "player.level.hero.100")
    {
        Hero * hero = client->GetFocusCity()->mayor;
        if (hero == nullptr)
        {
            gserver.SendObject(client, gserver.CreateError("shop.useGoods", -99, "Mayor not set."));
            return;
        }
        hero->level += 100;
        hero->remainpoint += 100;


        city->HeroUpdate(hero, 2);

        client->AddItem(itemid, -1);

        gserver.SendObject(client, obj2);
    
        client->SaveToDB();
        city->SaveToDB();
        hero->SaveToDB();
        return;
    }
    if (itemid == "player.box.special.1")
    {
        client->AddItem(itemid, -1);
        uint8_t _item1 = (rand() % 3) + 2;
        uint8_t _item2 = (rand() % 3) + 2;
        uint8_t _item3 = (rand() % 3) + 2;
        client->AddItem("hero.loyalty.1", _item1);
        client->AddItem("hero.loyalty.2", _item2);
        client->AddItem("hero.loyalty.3", _item3);

        amf3array itemBeans = amf3array();
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.1";
            additem["count"] = _item1;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.2";
            additem["count"] = _item2;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.3";
            additem["count"] = _item3;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        data2["itemBeans"] = itemBeans;

        gserver.SendObject(client, obj2);

        client->SaveToDB();
        return;
    }
    if (itemid == "player.box.special.2")
    {
        client->AddItem(itemid, -1);
        uint8_t _item1 = (rand() % 3) + 2;
        uint8_t _item2 = (rand() % 3) + 2;
        uint8_t _item3 = (rand() % 3) + 2;
        client->AddItem("hero.loyalty.4", _item1);
        client->AddItem("hero.loyalty.5", _item2);
        client->AddItem("hero.loyalty.6", _item3);

        amf3array itemBeans = amf3array();
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.4";
            additem["count"] = _item1;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.5";
            additem["count"] = _item2;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.6";
            additem["count"] = _item3;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        data2["itemBeans"] = itemBeans;

        gserver.SendObject(client, obj2);

        client->SaveToDB();
        return;
    }
    if (itemid == "player.box.special.3")
    {
        client->AddItem(itemid, -1);
        uint8_t _item1 = (rand() % 3) + 2;
        uint8_t _item2 = (rand() % 3) + 2;
        uint8_t _item3 = (rand() % 3) + 2;
        client->AddItem("hero.loyalty.7", _item1);
        client->AddItem("hero.loyalty.8", _item2);
        client->AddItem("hero.loyalty.9", _item3);

        amf3array itemBeans = amf3array();
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.7";
            additem["count"] = _item1;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.8";
            additem["count"] = _item2;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        {
            amf3object additem = amf3object();
            additem["id"] = "hero.loyalty.9";
            additem["count"] = _item3;
            additem["minCount"] = 0; additem["maxCount"] = 0;
            itemBeans.Add(additem);
        }
        data2["itemBeans"] = itemBeans;

        gserver.SendObject(client, obj2);

        client->SaveToDB();
        return;
    }

//     if (itemid == "player.speak.bronze_publicity_ambassador.permanent")
//     {
// 
//     }
//     if (itemid == "player.speak.bronze_publicity_ambassador.permanent.15")
//     {
// 
//     }
//     if (itemid == "player.speak.gold_publicity_ambassador.15")
//     {
// 
//     }
//     if (itemid == "player.speak.gold_publicity_ambassador.permanent"){}
//     if (itemid == "player.speak.silver_publicity_ambassador.15"){}
//     if (itemid == "player.speak.silver_publicity_ambassador.permanent"){}
//     if (itemid == "alliance.ritual_of_pact.advanced"){}
//     if (itemid == "alliance.ritual_of_pact.premium"){}
//     if (itemid == "alliance.ritual_of_pact.ultimate"){}
//     if (itemid == "consume.blueprint.1"){}
//     if (itemid == "consume.changeflag.1"){}
//     if (itemid == "consume.hegemony.1"){}
//     if (itemid == "consume.key.1"){}
//     if (itemid == "consume.key.2"){}
//     if (itemid == "consume.key.3"){}
//     if (itemid == "consume.move.1"){}
//    if (itemid == "consume.refreshtavern.1"){}//Hero Hunting
//     if (itemid == "consume.transaction.1"){}
//     if (itemid == "hero.intelligence.1"){}
//     if (itemid == "hero.loyalty.1"){}//Cross Medal
//     if (itemid == "hero.loyalty.2"){}
//     if (itemid == "hero.loyalty.3"){}
//     if (itemid == "hero.loyalty.4"){}
//     if (itemid == "hero.loyalty.5"){}
//     if (itemid == "hero.loyalty.6"){}
//     if (itemid == "hero.loyalty.7"){}
//     if (itemid == "hero.loyalty.8"){}
//     if (itemid == "hero.loyalty.9"){}
//     if (itemid == "hero.management.1"){}
//     if (itemid == "hero.power.1"){}
//     if (itemid == "hero.reset.1"){}
//     if (itemid == "hero.reset.1.a"){}
//     if (itemid == "player.attackinc.1"){}
//     if (itemid == "player.attackinc.1.b"){}
//     if (itemid == "player.box.1"){}
//     if (itemid == "player.box.2"){}
//     if (itemid == "player.box.3"){}
//     if (itemid == "player.box.currently.1"){}
//     if (itemid == "player.box.gambling.1"){}
//     if (itemid == "player.box.gambling.10"){}
//     if (itemid == "player.box.gambling.11"){}
//     if (itemid == "player.box.gambling.12"){}
//     if (itemid == "player.box.gambling.2"){}
//     if (itemid == "player.box.gambling.3"){}
//     if (itemid == "player.box.gambling.4"){}
//     if (itemid == "player.box.gambling.5"){}
//     if (itemid == "player.box.gambling.6"){}
//     if (itemid == "player.box.gambling.7"){}
//     if (itemid == "player.box.gambling.8"){}
//     if (itemid == "player.box.gambling.9"){}
//     if (itemid == "player.box.gambling.food"){}
//     if (itemid == "player.box.gambling.gold"){}
//     if (itemid == "player.box.gambling.iron"){}
//     if (itemid == "player.box.gambling.medal.10"){}
//     if (itemid == "player.box.gambling.medal.300"){}
//     if (itemid == "player.box.gambling.stone"){}
//     if (itemid == "player.box.gambling.wood"){}
//     if (itemid == "player.box.hero.a"){}
//     if (itemid == "player.box.hero.b"){}
//     if (itemid == "player.box.hero.c"){}
//     if (itemid == "player.box.hero.d"){}
//     if (itemid == "player.box.present.1"){}
//     if (itemid == "player.box.present.10"){}
//     if (itemid == "player.box.present.11"){}
//     if (itemid == "player.box.present.2"){}
//     if (itemid == "player.box.present.3"){}
//     if (itemid == "player.box.present.4"){}
//     if (itemid == "player.box.present.5"){}
//     if (itemid == "player.box.present.6"){}
//     if (itemid == "player.box.present.7"){}
//     if (itemid == "player.box.present.8"){}
//     if (itemid == "player.box.present.9"){}
//     if (itemid == "player.box.resource.1"){}
//     if (itemid == "player.box.special.1"){}
//     if (itemid == "player.box.special.2"){}
//     if (itemid == "player.box.special.3"){}
//     if (itemid == "player.box.troop.1"){}
//     if (itemid == "player.box.troop.a"){}//Junior Battalion
//     if (itemid == "player.box.troop.b"){}//Senior Battalion
//     if (itemid == "player.box.troop.c"){}
//     if (itemid == "player.box.troop.d"){}
//     if (itemid == "player.box.wood.1"){}
//     if (itemid == "player.defendinc.1"){}
//     if (itemid == "player.defendinc.1.b"){}
//     if (itemid == "player.destroy.1.a"){}
//     if (itemid == "player.experience.1.a"){}//Anabasis
//     if (itemid == "player.experience.1.b"){}
//     if (itemid == "player.experience.1.c"){}
//     if (itemid == "player.fort.1.c"){}
//     if (itemid == "player.gold.1.a"){}
//     if (itemid == "player.gold.1.b"){}
//     if (itemid == "player.heart.1.a"){}
//     if (itemid == "player.more.castle.1.a"){}
//     if (itemid == "player.name.1.a"){}
//     if (itemid == "player.peace.1"){}
//     if (itemid == "player.pop.1.a"){}
//     if (itemid == "player.relive.1"){}
//     if (itemid == "player.resinc.1"){}
//     if (itemid == "player.resinc.1.b"){}
//     if (itemid == "player.resinc.2"){}
//     if (itemid == "player.resinc.2.b"){}
//     if (itemid == "player.resinc.3"){}
//     if (itemid == "player.resinc.3.b"){}
//     if (itemid == "player.resinc.4"){}
//     if (itemid == "player.resinc.4.b"){}
//     if (itemid == "player.troop.1.a"){}
//     if (itemid == "player.troop.1.b"){}
//     if (itemid == "player.box.present.medal.50"){}
//     if (itemid == "player.box.present.12"){}
//     if (itemid == "player.box.present.13"){}
//     if (itemid == "player.box.present.14"){}
//     if (itemid == "player.box.present.15"){}
//     if (itemid == "player.box.present.16"){}
//     if (itemid == "player.box.present.17"){}
//     if (itemid == "player.box.present.18"){}
//     if (itemid == "player.box.present.19"){}
//     if (itemid == "player.box.present.20"){}
//     if (itemid == "player.box.present.21"){}
//     if (itemid == "player.box.present.22"){}
//     if (itemid == "player.box.present.medal.3500"){}
//     if (itemid == "player.box.present.medal.500"){}
//     if (itemid == "player.box.present.23"){}
//     if (itemid == "player.box.present.24"){}
//     if (itemid == "player.box.present.25"){}
//     if (itemid == "player.box.present.26"){}
//     if (itemid == "player.box.present.27"){}
//     if (itemid == "player.box.present.28"){}
//     if (itemid == "player.box.present.29"){}
//     if (itemid == "player.box.present.3"){}
//     if (itemid == "player.box.present.30"){}
//     if (itemid == "player.box.present.31"){}
//     if (itemid == "player.box.present.32"){}
//     if (itemid == "player.box.present.33"){}
//     if (itemid == "player.box.present.34"){}
//     if (itemid == "player.box.present.35"){}
//     if (itemid == "player.box.present.36"){}
//     if (itemid == "player.box.present.37"){}
//     if (itemid == "player.box.present.38"){}
//     if (itemid == "player.box.present.recall.a"){}
//     if (itemid == "player.box.present.recall.b"){}
//     if (itemid == "player.box.present.recall.c"){}
//     if (itemid == "player.box.present.money.3"){}
//     if (itemid == "player.box.present.money.4"){}
//     if (itemid == "player.box.present.money.5"){}
//     if (itemid == "player.box.present.money.6"){}
//     if (itemid == "player.box.present.money.7"){}
//     if (itemid == "player.box.present.money.8"){}
//     if (itemid == "player.box.present.money.9"){}
//     if (itemid == "player.box.present.money.10"){}
//     if (itemid == "player.box.present.money.11"){}
//     if (itemid == "player.box.present.money.12"){}
//     if (itemid == "player.box.present.money.13"){}
//     if (itemid == "player.box.present.money.14"){}
//     if (itemid == "player.box.present.money.15"){}
//     if (itemid == "player.box.present.money.16"){}
//     if (itemid == "player.box.present.money.buff.17"){}
//     if (itemid == "player.box.present.money.buff.18"){}
//     if (itemid == "player.box.present.money.buff.19"){}
//     if (itemid == "player.box.present.money.20"){}
//     if (itemid == "player.box.present.money.21"){}
//     if (itemid == "player.box.present.money.22"){}
//     if (itemid == "player.box.present.money.23"){}
//     if (itemid == "player.key.santa"){}
//     if (itemid == "player.santa.stoptoopsupkeep"){}
//     if (itemid == "player.box.present.christmas.a"){}
//     if (itemid == "player.box.present.christmas.b"){}
//     if (itemid == "player.box.present.christmas.c"){}
//     if (itemid == "player.box.present.money.24"){}
//     if (itemid == "player.box.present.money.25"){}
//     if (itemid == "player.key.newyear"){}
//     if (itemid == "player.newyear.stoptoopsupkeep"){}
//     if (itemid == "player.truce.dream"){}
//     if (itemid == "player.box.present.money.27"){}
//     if (itemid == "player.move.castle.1.b"){}
//     if (itemid == "player.box.present.40"){}
//     if (itemid == "player.key.easter_package"){}
//     if (itemid == "player.box.present.money.28"){}
//     if (itemid == "player.box.present.money.29"){}
//     if (itemid == "player.reduce.troops.upkeep.1"){}
//     if (itemid == "player.key.special.chest"){}
//     if (itemid == "player.box.present.money.30"){}
//     if (itemid == "player.box.present.money.31"){}
//     if (itemid == "player.box.merger.compensation"){}
//     if (itemid == "player.box.present.money.32"){}
//     if (itemid == "player.box.present.money.33"){}
//     if (itemid == "player.key.halloween"){}
//     if (itemid == "player.halloween.candy"){}
//     if (itemid == "player.box.present.money.34"){}
//     if (itemid == "player.box.present.money.35"){}
//     if (itemid == "player.box.evony.birthday"){}
//     if (itemid == "player.box.present.money.36"){}
//     if (itemid == "player.box.present.money.37"){}
//     if (itemid == "player.box.evony.subscription"){}
//     if (itemid == "player.box.toolbarbonus1.greater"){}
//     if (itemid == "player.box.toolbarbonus1.lesser"){}
//     if (itemid == "player.box.toolbarbonus1.medium"){}
//     if (itemid == "player.box.toolbarbonus1.superior"){}
//     if (itemid == "player.box.present.money.38"){}
//     if (itemid == "player.box.present.money.39"){}
//     if (itemid == "player.key.valentine"){}
//     if (itemid == "player.cupid.chocolate"){}
//     if (itemid == "player.queue.building"){}
//     if (itemid == "player.key.patrick"){}
//     if (itemid == "player.box.present.money.40"){}
//     if (itemid == "player.box.present.money.41"){}
//     if (itemid == "player.irish.whiskey"){}
//     if (itemid == "player.box.present.money.42"){}
//     if (itemid == "player.box.present.money.43"){}
//     if (itemid == "player.key.easter"){}
//     if (itemid == "player.easter.egg"){}
//     if (itemid == "player.box.compensation.a"){}
//     if (itemid == "player.box.compensation.b"){}
//     if (itemid == "player.box.compensation.c"){}
//     if (itemid == "player.box.compensation.d"){}
//     if (itemid == "player.box.compensation.e"){}
//     if (itemid == "player.box.present.money.44"){}
//     if (itemid == "player.box.present.money.45"){}
//     if (itemid == "player.key.bull"){}
//     if (itemid == "player.running.shoes"){}

    if (itemid == "player.box.gambling.3")
    {
        obj2["cmd"] = "shop.useGoods";

        amf3array itembeans = amf3array();
        amf3array gamblingbeans = amf3array();

        stItemConfig * randitem = &gserver.items[rand() % gserver.itemcount];

        amf3object item = amf3object();
        item["id"] = randitem->name;
        item["count"] = 1;
        item["minCount"] = 0;
        item["maxCount"] = 0;

        itembeans.Add(item);

        randitem = &gserver.items[rand() % gserver.itemcount];

        obj2["data"] = GenerateGamble();

        amf3object & announceitem = obj2["data"];
        //announceitem["itemBeans"] = amf3array();
        amf3array & array = announceitem["itemBeans"];
        amf3object itemobj = array.Get(0);

        int worthcents = 0;
        std::string desc;

        stItemConfig * it = gserver.GetItem((std::string)itemobj["id"]);
        worthcents = it->cost * (int)itemobj["count"];
        desc = it->desc;

        client->AddItem("player.box.gambling.3", -1);
        if (it->name == "player.box.gambling.gold")
        {
            client->GetFocusCity()->resources += {(double)itemobj["count"], 0, 0, 0, 0};
        }
        else if (it->name == "player.box.gambling.food")
        {
            client->GetFocusCity()->resources += {0, (double)itemobj["count"], 0, 0, 0};
        }
        else if (it->name == "player.box.gambling.wood")
        {
            client->GetFocusCity()->resources += {0, 0, (double)itemobj["count"], 0, 0};
        }
        else if (it->name == "player.box.gambling.stone")
        {
            client->GetFocusCity()->resources += {0, 0, 0, (double)itemobj["count"], 0};
        }
        else if (it->name == "player.box.gambling.iron")
        {
            client->GetFocusCity()->resources += {0, 0, 0, 0, (double)itemobj["count"]};
        }
        else if (it->name == "player.box.gambling.medal.10")
        {
            client->cents += (int32_t)itemobj["count"];
            desc = "Cents";
        }
        else
            client->AddItem(it->name, itemobj["count"]);

        std::stringstream ss;
        if (client->sex == 1)
            ss << "Lady ";
        else
            ss << "Lord ";
        ss << "<font color='#FF0000'><b><u>" << client->playername << "</u></b></font> gained <b><font color='#00A2FF'>" << (std::string)itemobj["count"] << " " << desc << "</font></b> (worth <b><font color='#FF0000'>" << worthcents << "</font></b> Cents) from <b><font color='#FF0000'>Amulet</font></b>!";
        gserver.MassMessage(ss.str());

        gserver.SendObject(client, obj2);
        return;
    }
    else if (itemid == "player.box.test.1")
    {

    }
/*
    ["data"] Type: Object - Value : Object
        ["alliance"] Type : Boolean - Value : False
        ["tV"] Type : Boolean - Value : True
        ["msg"] Type : String - Value : Lord <font color = '#FF0000'><b><u>LEGION57< / u>< / b>< / font> gained <b> < font color = '#00A2FF'>1 Ivory Horn< / font>< / b>(worth <b> < font color = '#FF0000'>60 < / font > < / b> Cents) from <b><font color = '#FF0000'>Amulet< / font>< / b>!
        ["noSenderSystemInfo"] Type : Boolean - Value : False


    ["data"] Type : Object - Value : Object
        ["alliance"] Type : Boolean - Value : False
        ["tV"] Type : Boolean - Value : False
        ["msg"] Type : String - Value : Lord <font color = '#FF0000'><b><u>LEGION57< / u>< / b>< / font> gained <b> < font color = '#00A2FF'>1 Ivory Horn< / font>< / b>(worth <b> < font color = '#FF0000'>60 < / font > < / b> Cents) from <b><font color = '#FF0000'>Amulet< / font>< / b>!
        ["noSenderSystemInfo"] Type : Boolean - Value : True*/


}
void pshop::ShopUseCastleGoods(amf3object & data, Client * client)
{
    amf3object obj2 = amf3object();
    obj2["cmd"] = "shop.useGoods";
    amf3object & data2 = obj2["data"];
    data2["packageId"] = 0.0;
    data2["ok"] = 1;

    uint32_t num = data["num"];
    std::string itemid = static_cast<std::string>(data["itemId"]);

    gserver.log->info("usecastlegoods: {}", itemid);

    if (itemid == "player.pop.1.a")
    {
        int64_t cid = data["castleId"];
        for (PlayerCity * city : client->citylist)
        {
            if (city->castleid == cid)
            {
                client->AddItem(itemid, -1);
                if ((double)city->maxpopulation * 0.20 < 100)
                    city->population += 100;
                else
                    city->population += int32_t(city->maxpopulation * 0.20);
                if (city->population > city->maxpopulation)
                    city->population = city->maxpopulation;
                city->CalculateStats();
                city->CastleUpdate();
                city->ResourceUpdate();
                client->SaveToDB();
                city->SaveToDB();
                gserver.SendObject(client, obj2);
                return;
            }
        }
        //no city exists - error
        gserver.SendObject(client, gserver.CreateError("shop.useCastleGoods", -99, "Invalid castle."));
        return;
    }
    else if (itemid == "player.heart.1.a")
    {
        int64_t cid = data["castleId"];
        for (PlayerCity * city : client->citylist)
        {
            if (city->castleid == cid)
            {
                client->AddItem(itemid, -1);
                city->loyalty = 100;
                city->grievance = 0;
                city->CalculateStats();
                city->CastleUpdate();
                city->ResourceUpdate();
                client->SaveToDB();
                city->SaveToDB();
                gserver.SendObject(client, obj2);
                return;
            }
        }
        //no city exists - error
        gserver.SendObject(client, gserver.CreateError("shop.useCastleGoods", -99, "Invalid castle."));
        return;
    }
    else if (itemid == "player.box.test.1")
    {
        //test item
    }
}
int32_t pshop::GetGambleCount(std::string item)
{
    if (item == "consume.1.a")
        return (rand() % 5 == 1) ? 5 : 1;
    if (item == "consume.2.a")
        return (rand() % 5 == 1) ? 5 : 1;
    if (item == "consume.2.b")
        return (rand() % 5 == 1) ? 5 : 1;
    if (item == "consume.2.b.1")
        return (rand() % 5 == 1) ? 5 : 1;
    if (item == "consume.blueprint.1")
        return (rand() % 5 == 1) ? 5 : 1;
    if (item == "consume.refreshtavern.1")
        return (rand() % 5 == 1) ? 10 : 5;
    if (item == "consume.transaction.1")
        return (rand() % 5 == 1) ? 10 : 5;
    if (item == "hero.loyalty.1")
        return (rand() % 5 != 1) ? 1 : 5;
    if (item == "hero.loyalty.2")
        return (rand() % 5 != 1) ? 1 : 5;
    if (item == "hero.loyalty.3")
        return (rand() % 5 != 1) ? 1 : 5;
    if (item == "hero.loyalty.4")
        return (rand() % 5 != 1) ? 1 : 5;
    if (item == "hero.loyalty.5")
        return (rand() % 5 != 1) ? 1 : 5;
    if (item == "hero.loyalty.6")
        return (rand() % 5 != 1) ? 1 : 5;
    if (item == "hero.loyalty.7")
        return (rand() % 5 != 1) ? 1 : 4;
    if (item == "hero.loyalty.8")
        return (rand() % 5 != 1) ? 1 : 3;
    if (item == "hero.loyalty.9")
        return (rand() % 5 != 1) ? 1 : 2;
    if (item == "player.box.gambling.food")
        return (rand() % 2 == 1) ? 250000 : 500000;
    if (item == "player.box.gambling.wood")
        return (rand() % 2 == 1) ? 250000 : 500000;
    if (item == "player.box.gambling.stone")
        return (rand() % 2 == 1) ? 150000 : 300000;
    if (item == "player.box.gambling.iron")
        return (rand() % 2 == 1) ? 100000 : 200000;
    if (item == "player.box.gambling.gold")
        return (rand() % 2 == 1) ? 150000 : 300000;
    if (item == "player.heart.1.a")
        return (rand() % 5 == 1) ? 10 : 5;
    if (item == "player.queue.building")
        return (rand() % 5 == 1) ? 10 : 5;
    if (item == "player.gold.1.a")
        return (rand() % 5 == 1) ? 10 : 5;
    if (item == "player.gold.1.b")
        return (rand() % 5 == 1) ? 10 : 5;

    return 1;
}
amf3object pshop::GenerateGamble()
{
    amf3array itemBeans = amf3array();
    amf3array gamblingItemsBeans = amf3array();
    amf3object data = amf3object();

    //16 normals
    //4 corners
    //3 rares (left right bottom)
    //1 super rare (top middle)

    // 24 total


    // Item rarity:
    // 5 = ultra rare (chance within super rare to appear)
    // 4 = super rare
    // 3 = semi rare
    // 2 = special
    // 1 = common


    for (int i = 0; i < 16; ++i)
    {
        amf3object obj = amf3object();
        stItemConfig * item = gserver.gambleitems.common.at(rand() % gserver.gambleitems.common.size());
        obj["id"] = item->name;
        obj["count"] = GetGambleCount(item->name);
        obj["kind"] = item->rarity - 1;
        gamblingItemsBeans.Add(obj);
        //gserver.consoleLogger->information(Poco::format("Item: %s", item->name));
    }
    for (int i = 0; i < 4; ++i)
    {
        amf3object obj = amf3object();
        stItemConfig * item = gserver.gambleitems.special.at(rand() % gserver.gambleitems.special.size());
        obj["id"] = item->name;
        obj["count"] = GetGambleCount(item->name);
        obj["kind"] = item->rarity - 1;
        gamblingItemsBeans.Add(obj);
        //gserver.consoleLogger->information(Poco::format("Item: %s", item->name));
    }
    for (int i = 0; i < 3; ++i)
    {
        amf3object obj = amf3object();
        stItemConfig * item = gserver.gambleitems.rare.at(rand() % gserver.gambleitems.rare.size());
        obj["id"] = item->name;
        obj["count"] = GetGambleCount(item->name);
        obj["kind"] = item->rarity - 1;
        gamblingItemsBeans.Add(obj);
        //gserver.consoleLogger->information(Poco::format("Item: %s", item->name));
    }
    {
        amf3object obj = amf3object();
        stItemConfig * item;
        if (rand() % 100 < 95)
        {
            item = gserver.gambleitems.superrare.at(rand() % gserver.gambleitems.superrare.size());
            obj["kind"] = item->rarity - 1;
        }
        else
        {
            item = gserver.gambleitems.ultrarare.at(rand() % gserver.gambleitems.ultrarare.size());
            obj["kind"] = item->rarity;
        }

        obj["id"] = item->name;
        obj["count"] = GetGambleCount(item->name);
        gamblingItemsBeans.Add(obj);
        //gserver.consoleLogger->information(Poco::format("Item: %s", item->name));
    }
    int get = 0;
    int value = (rand() % 100000);
    if ((value >= 0) && (value < 60000))
    {
        get = rand() % 16;
    }
    else if ((value >= 60000) && (value < 85000))
    {
        get = rand() % 4 + 16;
    }
    else if ((value >= 85000) && (value < 95000))
    {
        get = rand() % 3 + 16 + 4;
    }
    else if ((value >= 95000) && (value < 100000))
    {
        get = 23;
    }

    itemBeans.Add(gamblingItemsBeans.Get(get));

    data["itemBeans"] = itemBeans;
    data["gamblingItemsBeans"] = gamblingItemsBeans;
    data["packageId"] = 0.0;
    data["ok"] = 1;

    return data;
}
