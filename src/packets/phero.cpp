/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "phero.h"
#include "../spitfire.h"
#include "../Hero.h"
#include "../Client.h"
#include "../City.h"

phero::phero(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

phero::~phero()
{

}

void phero::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    VERIFYCASTLEID();
    CHECKCASTLEID();

    if (command == "awardGold")
    {
        Hero * hero = city->GetHero((uint64_t)data["heroId"]);
        if (hero == nullptr)
        {
            obj2["cmd"] = "hero.awardGold";
            data2["ok"] = -99;
            data2["errorMsg"] = "Invalid hero";
            data2["packageId"] = 0.0;

            gserver.SendObject(client, obj2);
            return;
        }
        else
        {
            uint32_t goldcost = hero->m_level * 100;

            if (city->m_resources.gold < goldcost)
            {
                obj2["cmd"] = "hero.awardGold";
                data2["ok"] = -99;
                data2["errorMsg"] = "Not enough gold";
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);
                return;
            }

            hero->m_loyalty += 5;
            if (hero->m_loyalty > 100)
                hero->m_loyalty = 100;

            city->m_resources.gold -= goldcost;

            city->ResourceUpdate();
            city->HeroUpdate(hero, 2);

            obj2["cmd"] = "hero.awardGold";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;

            gserver.SendObject(client, obj2);

            client->SaveToDB();
            city->SaveToDB();
            hero->SaveToDB();

            return;
        }
    }
    if (command == "useItem")
    {
        Hero * hero = city->GetHero((uint64_t)data["heroId"]);
        if (hero == nullptr)
        {
            obj2["cmd"] = "hero.useItem";
            data2["ok"] = -99;
            data2["errorMsg"] = "Invalid hero";
            data2["packageId"] = 0.0;

            gserver.SendObject(client, obj2);
            return;
        }
        else
        {
            uint64_t addexp = 0;
            uint32_t cents = 0;
            std::string item = data["itemId"];
            std::string _substr = item.substr(0, item.length()-2);

            if (_substr == "player.experience.1")
            {
                if (item == "player.experience.1.a")
                {
                    //exp 1000 or 8%
                    cents = 8;
                    if ((hero->m_upgradeexp*0.08) > 1000)
                        addexp = hero->m_upgradeexp*0.08;
                    else
                        addexp = 1000;
                }
                else if (item == "player.experience.1.b")
                {
                    //exp 10000 or 30%
                    cents = 30;
                    if ((hero->m_upgradeexp*0.3) > 10000)
                        addexp = hero->m_upgradeexp*0.3;
                    else
                        addexp = 10000;
                }
                else if (item == "player.experience.1.c")
                {
                    //exp 100000 or 100%
                    cents = 100;
                    if (hero->m_upgradeexp > 100000)
                        addexp = hero->m_upgradeexp;
                    else
                        addexp = 100000;
                }

                if (client->GetItemCount(item) <= 0)
                {
                    if (client->cents < cents)
                    {
                        // TODO find error value -- hero.useItem
                        gserver.SendObject(client, gserver.CreateError("hero.useItem", -99, "Not enough cents."));// not enough item and not enough cents
                        return;
                    }
                    //not enough item, but can buy with cents
                    client->cents -= cents;
                    client->PlayerUpdate();
                }
                else
                { //has item
                    client->AddItem(item, -1);
                }

                hero->m_experience += addexp;


                city->HeroUpdate(hero, 2);

                obj2["cmd"] = "hero.useItem";
                data2["ok"] = 1;
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);

                client->SaveToDB();
                city->SaveToDB();
                hero->SaveToDB();
                return;
            }
            else if (_substr == "hero.loyalty")
            {
                uint8_t addloyalty = 0;
                if (item == "hero.loyalty.1")//cross - 5 loyalty
                    addloyalty = 5;
                else if (item == "hero.loyalty.2")//rose - 10 loyalty
                    addloyalty = 10;
                else if (item == "hero.loyalty.3")//lion - 15 loyalty
                    addloyalty = 15;
                else if (item == "hero.loyalty.4")//honor - 20 loyalty
                    addloyalty = 20;
                else if (item == "hero.loyalty.5")//courage - 25 loyalty
                    addloyalty = 25;
                else if (item == "hero.loyalty.6")//wisdom - 30 loyalty
                    addloyalty = 30;
                else if (item == "hero.loyalty.7")//freedom - 40 loyalty
                    addloyalty = 40;
                else if (item == "hero.loyalty.8")//justice - 50 loyalty
                    addloyalty = 50;
                else if (item == "hero.loyalty.9")//nation - 60 loyalty
                    addloyalty = 60;

                if (client->GetItemCount(item) <= 0)
                {
                    gserver.SendObject(client, gserver.CreateError("hero.useItem", -99, "No Item."));// not enough item
                    return;
                }
                //has item

                client->AddItem((std::string)item, -1);

                hero->m_loyalty += addloyalty;
                if (hero->m_loyalty > 100) hero->m_loyalty=100;

                city->HeroUpdate(hero, 2);

                obj2["cmd"] = "hero.useItem";
                data2["ok"] = 1;
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);

                client->SaveToDB();
                city->SaveToDB();
                hero->SaveToDB();
                return;
            }
            gserver.SendObject(client, gserver.CreateError("hero.useItem", -99, "Invalid Item."));
            return;
        }
    }
    if (command == "changeName")
    {
        int64_t heroid = data["heroId"];
        std::string newname = data["newName"];

        Hero * hero = city->GetHero(heroid);

        if (!hero)
        {
            gserver.SendObject(client, gserver.CreateError("hero.changeName", -99, "Hero not found."));
            return;
        }

        hero->m_name = newname;

        city->HeroUpdate(hero, 2);

        obj2["cmd"] = "hero.changeName";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;
        gserver.SendObject(client, obj2);

        hero->SaveToDB();

        return;
    }
    if ((command == "getHerosListFromTavern"))
    {
        int innlevel = city->GetBuildingLevel(B_INN);

        if (innlevel > 0)
        {
            obj2["cmd"] = "hero.getHerosListFromTavern";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;
            int tempnum = 0;
            for (int i = 0; i < 10; ++i)
                if (city->m_heroes[i])
                    tempnum++;
            data2["posCount"] = city->GetBuildingLevel(B_FEASTINGHALL) - tempnum;

            amf3array heroes = amf3array();

            for (int i = 0; i < innlevel; ++i)
            {
                if (!city->m_innheroes[i])
                {
                    city->m_innheroes[i] = gserver.CreateRandomHero(innlevel);
                }

                amf3object temphero = city->m_innheroes[i]->ToObject();
                heroes.Add(temphero);
            }

            data2["heros"] = heroes;

            gserver.SendObject(client, obj2);
        }
        else
        {
            obj2["cmd"] = "hero.getHerosListFromTavern";
            data2["ok"] = -1; // TODO find error (no inn exists) - hero.getHerosListFromTavern
            data2["packageId"] = 0.0;

            gserver.SendObject(client, obj2);
        }

        return;
    }
    if ((command == "hireHero"))
    {
        std::string heroname = data["heroName"];
        int blevel = city->GetBuildingLevel(B_FEASTINGHALL);

        if (blevel <= city->HeroCount())
        {
            gserver.SendObject(client, gserver.CreateError("hero.hireHero", -99, "Insufficient vacancies in Feasting Hall."));
            return;
        }

        for (int i = 0; i < 10; ++i)
        {
            if (city->m_innheroes[i]->m_name == heroname)
            {
                int32_t hirecost = city->m_innheroes[i]->m_level * 1000;
                if (hirecost > city->m_resources.gold)
                {
                    // TODO Get proper not enough gold to hire error code - hero.hireHero
                    gserver.SendObject(client, gserver.CreateError("hero.hireHero", -99, "Not enough gold!"));
                    return;
                }

                for (int x = 0; x < 10; ++x)
                {
                    if (!city->m_heroes[x])
                    {
                        city->m_resources.gold -= hirecost;
                        city->CalculateResourceStats();
                        city->CalculateStats();
                        city->CalculateResources();
                        {
                            city->m_innheroes[i]->m_id = gserver.m_heroid++;
                        }
                        {
                            city->HeroUpdate(city->m_innheroes[i], 0);
                            city->m_heroes[x] = city->m_innheroes[i];
                            city->m_innheroes[i] = nullptr;
                        }
                        city->ResourceUpdate();
                        city->m_heroes[x]->m_client = client;
                        city->m_heroes[x]->m_ownerid = client->accountid;
                        city->m_heroes[x]->m_castleid = city->m_castleid;
                        city->m_heroes[x]->InsertToDB();
                        break;
                    }
                }

                obj2["cmd"] = "hero.hireHero";
                data2["ok"] = 1;
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);

                client->SaveToDB();
                city->SaveToDB();

                return;
            }
        }
        gserver.SendObject(client, gserver.CreateError("hero.hireHero", -99, "Hero does not exist!"));
        return;
    }
    if ((command == "fireHero"))
    {
        int heroid = data["heroId"];

        for (int i = 0; i < 10; ++i)
        {
            if (city->m_heroes[i] && city->m_heroes[i]->m_id == heroid)
            {
                if (city->m_heroes[i]->m_status != 0)
                {
                    gserver.SendObject(client, gserver.CreateError("hero.fireHero", -80, "Status of this hero is not Idle!"));
                    return;
                }
                city->CalculateResourceStats();
                city->CalculateStats();
                city->CalculateResources();
                city->ResourceUpdate();
                city->HeroUpdate(city->m_heroes[i], 1);

                gserver.m_deletedhero.push_back(city->m_heroes[i]->m_id);

                city->m_heroes[i]->DeleteFromDB();

                delete city->m_heroes[i];
                city->m_heroes[i] = 0;

                obj2["cmd"] = "hero.fireHero";
                data2["ok"] = 1;
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);
                return;
            }
        }

        gserver.SendObject(client, gserver.CreateError("hero.fireHero", -99, "Hero does not exist!"));
        return;
    }
    if (command == "promoteToChief")
    {// BUG : Correct hero not being promoted
        int heroid = data["heroId"];

        for (int i = 0; i < 10; ++i)
        {
            if (city->m_heroes[i] && city->m_heroes[i]->m_id == heroid)
            {
                city->CalculateResourceStats();
                city->CalculateStats();
                city->CalculateResources();
                Hero * oldhero = 0;
                if (city->m_mayor)
                {
                    oldhero = city->m_mayor;
                    city->m_mayor->m_status = DEF_HEROIDLE;
                    city->HeroUpdate(city->m_mayor, 2);
                }
                city->m_mayor = city->m_heroes[i];
                city->m_heroes[i]->m_status = DEF_HEROMAYOR;

                city->HeroUpdate(city->m_mayor, 2);
                city->CalculateResourceStats();
                city->CalculateResources();
                city->ResourceUpdate();

                obj2["cmd"] = "hero.promoteToChief";
                data2["ok"] = 1;
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);

                if (oldhero)
                    oldhero->SaveToDB();
                city->m_mayor->SaveToDB();

                city->SaveToDB();

                return;
            }
        }
        gserver.SendObject(client, gserver.CreateError("hero.promoteToChief", -99, "TODO error message - hero.promoteToChief"));
        return;
        // TODO needs an error message - hero.promoteToChief
    }
    if ((command == "dischargeChief"))
    {
        if (!city->m_mayor)
        {
            gserver.SendObject(client, gserver.CreateError("hero.dischargeChief", -99, "Castellan is not appointed yet."));
            return;
        }

        Hero * oldhero = city->m_mayor;

        city->CalculateResourceStats();
        city->CalculateStats();
        city->CalculateResources();
        city->m_mayor->m_status = 0;
        city->HeroUpdate(city->m_mayor, 2);
        city->m_mayor = 0;
        city->CalculateResourceStats();
        city->CalculateResources();
        city->ResourceUpdate();

        obj2["cmd"] = "hero.dischargeChief";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);

        oldhero->SaveToDB();
        city->SaveToDB();

        return;
    }
    if ((command == "resetPoint"))
    {
        int heroid = data["heroId"];

        // TODO require holy water? - hero.resetPoint

        for (int i = 0; i < 10; ++i)
        {
            if (city->m_heroes[i] && city->m_heroes[i]->m_id == heroid)
            {
                if (city->m_heroes[i]->m_status > 1)
                {
                    gserver.SendObject(client, gserver.CreateError("hero.resetPoint", -80, "Status of this hero is not Idle!"));
                    return;
                }
                city->CalculateResourceStats();
                city->CalculateStats();
                city->CalculateResources();
                city->ResourceUpdate();

                Hero * hero = city->m_heroes[i];
                hero->m_power = hero->m_basepower;
                hero->m_management = hero->m_basemanagement;
                hero->m_stratagem = hero->m_basestratagem;
                hero->m_remainpoint = hero->m_level;

                city->HeroUpdate(city->m_heroes[i], 2);
                city->CalculateResourceStats();
                city->CalculateResources();

                obj2["cmd"] = "hero.resetPoint";
                data2["ok"] = 1;
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);

                city->m_heroes[i]->SaveToDB();

                return;
            }
        }

        gserver.SendObject(client, gserver.CreateError("hero.resetPoint", -99, "Hero does not exist!"));
        return;
    }
    if ((command == "addPoint"))
    {
        int heroid = data["heroId"];
        int stratagem = data["stratagem"];
        int power = data["power"];
        int management = data["management"];

        // TODO require holy water? - hero.resetPoint

        for (int i = 0; i < 10; ++i)
        {
            if (city->m_heroes[i] && city->m_heroes[i]->m_id == heroid)
            {
                city->CalculateResourceStats();
                city->CalculateStats();
                city->CalculateResources();
                city->ResourceUpdate();

                Hero * hero = city->m_heroes[i];
                if (((stratagem + power + management) >(hero->m_basemanagement + hero->m_basepower + hero->m_basestratagem + hero->m_level))
                    || (stratagem < hero->m_stratagem) || (power < hero->m_power) || (management < hero->m_management))
                {
                    gserver.SendObject(client, gserver.CreateError("hero.addPoint", -99, "Invalid action."));
                    return;
                }
                hero->m_power = power;
                hero->m_management = management;
                hero->m_stratagem = stratagem;
                hero->m_remainpoint = (hero->m_basemanagement + hero->m_basepower + hero->m_basestratagem + hero->m_level) - (stratagem + power + management);

                city->HeroUpdate(city->m_heroes[i], 2);
                city->CalculateResourceStats();
                city->CalculateResources();
                city->ResourceUpdate();

                obj2["cmd"] = "hero.resetPoint";
                data2["ok"] = 1;
                data2["packageId"] = 0.0;

                gserver.SendObject(client, obj2);

                city->m_heroes[i]->SaveToDB();

                return;
            }
        }

        gserver.SendObject(client, gserver.CreateError("hero.addPoint", -99, "Hero does not exist!"));
        return;
    }
    if ((command == "levelUp"))
    {
        int heroid = data["heroId"];

        obj2["cmd"] = "hero.levelUp";
        data2["packageId"] = 0.0;

        for (int i = 0; i < 10; ++i)
        {
            if (city->m_heroes[i] && city->m_heroes[i]->m_id == heroid)
            {
                Hero * hero = city->m_heroes[i];
                if (hero->m_experience < hero->m_upgradeexp)
                {
                    gserver.SendObject(client, gserver.CreateError("hero.levelUp", -99, "Not enough experience."));
                    return;
                }

                hero->m_level++;
                hero->m_remainpoint++;

                hero->m_experience -= hero->m_upgradeexp;
                hero->m_upgradeexp = hero->m_level * hero->m_level * 100;

                city->HeroUpdate(hero, 2);

                obj2["cmd"] = "hero.levelUp";
                data2["ok"] = 1;

                gserver.SendObject(client, obj2);

                city->m_heroes[i]->SaveToDB();

                return;
            }
        }

        gserver.SendObject(client, gserver.CreateError("hero.levelUp", -99, "Hero does not exist!"));
        return;
    }
    if ((command == "refreshHerosListFromTavern"))
    {
        int innlevel = city->GetBuildingLevel(B_INN);

        if (innlevel > 0)
        {
            //check for hero hunting item
            if (client->GetItemCount("consume.refreshtavern.1") <= 0)
            {
                gserver.SendObject(client, gserver.CreateError("hero.refreshHerosListFromTavern", -99, " 0 Refresh items"));
                return;
            }

            client->AddItem("consume.refreshtavern.1", -1);
            client->ItemUpdate("consume.refreshtavern.1");

            obj2["cmd"] = "hero.getHerosListFromTavern";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;
            int tempnum = 0;
            for (int i = 0; i < 10; ++i)
                if (city->m_heroes[i])
                    tempnum++;
            data2["posCount"] = city->GetBuildingLevel(B_FEASTINGHALL) - tempnum;

            amf3array heroes = amf3array();

            for (int i = 0; i < innlevel; ++i)
            {
                if (city->m_innheroes[i])
                {
                    delete city->m_innheroes[i];
                }

                city->m_innheroes[i] = gserver.CreateRandomHero(innlevel);
                amf3object temphero = city->m_innheroes[i]->ToObject();
                heroes.Add(temphero);
            }

            data2["heros"] = heroes;

            gserver.SendObject(client, obj2);
        }
        else
        {
            // TODO find error (not enough cents) - hero.refreshHerosListFromTavern
            gserver.SendObject(client, gserver.CreateError("hero.refreshHerosListFromTavern", -99, "Not enough cents."));
        }

        return;
    }
}

