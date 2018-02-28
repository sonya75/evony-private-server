/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "prank.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"
#include "../Alliance.h"
#include "../AllianceMgr.h"

prank::prank(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

prank::~prank()
{

}

void prank::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if ((command == "getPlayerRank"))
    {
        int pagesize = data["pageSize"];
        std::string key = data["key"];
        int sorttype = data["sortType"];
        int pageno = data["pageNo"];

        obj2["cmd"] = "rank.getPlayerRank";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        std::list<stClientRank> * ranklist;
        amf3array beans = amf3array();
        switch (sorttype)
        {
            case 1:
                ranklist = &gserver.titlerank;
                break;
            case 2:
                ranklist = &gserver.prestigerank;
                break;
            case 3:
                ranklist = &gserver.honorrank;
                break;
            case 4:
                ranklist = &gserver.citiesrank;
                break;
            case 5:
                ranklist = &gserver.populationrank;
                break;
            default:
                ranklist = &gserver.prestigerank;
                break;
        }
        std::list<stClientRank>::iterator iter;
        if (pagesize <= 0 || pagesize > 20 || pageno < 0 || pageno > 100000)
        {
            gserver.SendObject(client, gserver.CreateError("rank.getPlayerRank", -99, "Invalid data."));
            return;
        }

        if (key.length() > 0)
        {
            //search term given
            ranklist = (std::list<stClientRank>*)gserver.DoRankSearch(key, 1, ranklist, pageno, pagesize);//1 = client
        }

        if ((pageno - 1)*pagesize > ranklist->size())
        {
            gserver.SendObject(client, gserver.CreateError("rank.getPlayerRank", -99, "Invalid page."));
            return;
        }
        iter = ranklist->begin();
        for (int i = 0; i < (pageno - 1)*pagesize; ++i)
            ++iter;

        int rank = (pageno - 1)*pagesize + 1;
        data2["pageNo"] = pageno;
        data2["pageSize"] = pagesize;
        if ((ranklist->size() % pagesize) == 0)
            data2["totalPage"] = (ranklist->size() / pagesize);
        else
            data2["totalPage"] = (ranklist->size() / pagesize) + 1;
        for (; iter != ranklist->end() && pagesize != 0; ++iter)
        {
            pagesize--;
            amf3object temp = amf3object();
            temp["createrTime"] = 0;
            if (iter->client->allianceid > 0)
            {
                temp["alliance"] = iter->client->GetAlliance()->name;
                temp["allianceLevel"] = AllianceMgr::GetAllianceRank(iter->client->alliancerank);
                temp["levelId"] = iter->client->alliancerank;
            }
            temp["office"] = iter->client->office;
            temp["sex"] = iter->client->sex;
            temp["honor"] = iter->client->honor;
            temp["bdenyotherplayer"] = iter->client->bdenyotherplayer;
            temp["id"] = iter->client->accountid;
            temp["accountName"] = "";
            temp["prestige"] = iter->client->prestige;
            temp["faceUrl"] = iter->client->faceurl;
            temp["flag"] = iter->client->flag;
            temp["userId"] = iter->client->masteraccountid;
            temp["userName"] = iter->client->playername;
            temp["castleCount"] = iter->client->citycount;
            temp["titleId"] = iter->client->title;
            temp["medal"] = 0;
            temp["ranking"] = iter->rank;
            temp["lastLoginTime"] = 0;
            temp["population"] = iter->client->population;
            beans.Add(temp);
        }
        data2["beans"] = beans;
        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getAllianceRank"))
    {
        obj2["cmd"] = "rank.getAllianceRank";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        data2["beans"] = amf3array();
        gserver.SendObject(client, obj2);
        try
        {
            int pagesize = data["pageSize"];
            std::string key = data["key"];
            int sorttype = data["sortType"];
            int pageno = data["pageNo"];

            obj2["cmd"] = "rank.getAllianceRank";
            data2["packageId"] = 0.0;
            data2["ok"] = 1;
            std::list<stAlliance> * ranklist;
            amf3array beans = amf3array();
            switch (sorttype)
            {
            case 1:
                ranklist = &gserver.alliances->membersrank;
                break;
            case 2:
                ranklist = &gserver.alliances->prestigerank;
                break;
            case 3:
                ranklist = &gserver.alliances->honorrank;
                break;
            default:
                ranklist = &gserver.alliances->membersrank;
                break;
            }
            std::list<stAlliance>::iterator iter;
            if (pagesize <= 0 || pagesize > 20 || pageno < 0 || pageno > 100000)
            {
                gserver.SendObject(client, gserver.CreateError("rank.getAllianceRank", -99, "Invalid data."));
                return;
            }

            if (key.length() > 0)
            {
                //search term given
                ranklist = (std::list<stAlliance>*)gserver.DoRankSearch(key, 4, ranklist, pageno, pagesize);//1 = client
            }

            if ((pageno - 1)*pagesize > ranklist->size())
            {
                gserver.SendObject(client, gserver.CreateError("rank.getAllianceRank", -99, "Invalid page."));
                return;
            }
            iter = ranklist->begin();
            for (int i = 0; i < (pageno - 1)*pagesize; ++i)
                iter++;

            int rank = (pageno - 1)*pagesize + 1;
            data2["pageNo"] = pageno;
            data2["pageSize"] = pagesize;
            if ((ranklist->size() % pagesize) == 0)
                data2["totalPage"] = (ranklist->size() / pagesize);
            else
                data2["totalPage"] = (ranklist->size() / pagesize) + 1;
            for (; iter != ranklist->end() && pagesize != 0; ++iter)
            {
                pagesize--;
                amf3object temp = amf3object();
                temp["member"] = iter->ref->currentmembers;
                temp["prestige"] = iter->ref->prestige;
                temp["rank"] = iter->rank;
                temp["playerName"] = iter->ref->owner;
                temp["honor"] = iter->ref->honor;
                temp["description"] = iter->ref->intro;
                temp["createrName"] = iter->ref->founder;
                temp["name"] = iter->ref->name;
                temp["city"] = iter->ref->citycount;
                beans.Add(temp);
            }
            data2["beans"] = beans;
            gserver.SendObject(client, obj2);
        }
        catch (std::exception & e)
        {
            gserver.log->error("Ranklist exception : {} {} {}", (std::string)__FILE__, __LINE__, e.what());
        }
        catch (...)
        {
            gserver.log->error("Ranklist exception : {} {}", (std::string)__FILE__, __LINE__);
        }
        return;
    }
    if ((command == "getHeroRank"))
    {
        int pagesize = data["pageSize"];
        std::string key = data["key"];
        int sorttype = data["sortType"];
        int pageno = data["pageNo"];

        obj2["cmd"] = "rank.getHeroRank";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        std::list<stHeroRank> * ranklist;
        amf3array beans = amf3array();
        switch (sorttype)
        {
            case 1:
                ranklist = &gserver.herorankgrade;
                break;
            case 2:
                ranklist = &gserver.herorankmanagement;
                break;
            case 3:
                ranklist = &gserver.herorankpower;
                break;
            case 4:
                ranklist = &gserver.herorankstratagem;
                break;
            default:
                ranklist = &gserver.herorankgrade;
                break;
        }
        std::list<stHeroRank>::iterator iter;
        if (pagesize <= 0 || pagesize > 20 || pageno < 0 || pageno > 100000)
        {
            gserver.SendObject(client, gserver.CreateError("rank.getHeroRank", -99, "Invalid data."));
            return;
        }

        if (key.length() > 0)
        {
            //search term given
            ranklist = (std::list<stHeroRank>*)gserver.DoRankSearch(key, 2, ranklist, pageno, pagesize);//1 = client
        }

        if ((pageno - 1)*pagesize > ranklist->size())
        {
            gserver.SendObject(client, gserver.CreateError("rank.getHeroRank", -99, "Invalid page."));
            return;
        }
        iter = ranklist->begin();
        for (int i = 0; i < (pageno - 1)*pagesize; ++i)
            iter++;

        int rank = (pageno - 1)*pagesize + 1;
        data2["pageNo"] = pageno;
        data2["pageSize"] = pagesize;
        if ((ranklist->size() % pagesize) == 0)
            data2["totalPage"] = (ranklist->size() / pagesize);
        else
            data2["totalPage"] = (ranklist->size() / pagesize) + 1;
        for (; iter != ranklist->end() && pagesize != 0; ++iter)
        {
            pagesize--;
            amf3object temp = amf3object();
            temp["rank"] = iter->rank;
            temp["stratagem"] = iter->stratagem;
            temp["name"] = iter->name;
            temp["power"] = iter->power;
            temp["grade"] = iter->grade;
            temp["management"] = iter->management;
            temp["kind"] = iter->kind;
            beans.Add(temp);
        }
        data2["beans"] = beans;
        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getCastleRank"))
    {
        int pagesize = data["pageSize"];
        std::string key = data["key"];
        int sorttype = data["sortType"];
        int pageno = data["pageNo"];

        obj2["cmd"] = "rank.getCastleRank";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        std::list<stCastleRank> * ranklist;
        amf3array beans = amf3array();
        switch (sorttype)
        {
            case 1:
                ranklist = &gserver.castlerankpopulation;
                break;
            case 2:
                ranklist = &gserver.castleranklevel;
                break;
            default:
                ranklist = &gserver.castlerankpopulation;
                break;
        }
        std::list<stCastleRank>::iterator iter;
        if (pagesize <= 0 || pagesize > 20 || pageno < 0 || pageno > 100000)
        {
            gserver.SendObject(client, gserver.CreateError("rank.getCastleRank", -99, "Invalid data."));
            return;
        }

        if (key.length() > 0)
        {
            //search term given
            ranklist = (std::list<stCastleRank>*)gserver.DoRankSearch(key, 3, ranklist, pageno, pagesize);//1 = client
        }

        if ((pageno - 1)*pagesize > ranklist->size())
        {
            gserver.SendObject(client, gserver.CreateError("rank.getCastleRank", -99, "Invalid page."));
            return;
        }
        iter = ranklist->begin();
        for (int i = 0; i < (pageno - 1)*pagesize; ++i)
            iter++;

        int rank = (pageno - 1)*pagesize + 1;
        data2["pageNo"] = pageno;
        data2["pageSize"] = pagesize;
        if ((ranklist->size() % pagesize) == 0)
            data2["totalPage"] = (ranklist->size() / pagesize);
        else
            data2["totalPage"] = (ranklist->size() / pagesize) + 1;
        for (; iter != ranklist->end() && pagesize != 0; ++iter)
        {
            pagesize--;
            amf3object temp = amf3object();
            temp["alliance"] = iter->alliance;
            temp["rank"] = iter->rank;
            temp["level"] = iter->level;
            temp["name"] = iter->name;
            temp["grade"] = iter->grade;
            temp["kind"] = iter->kind;
            temp["population"] = iter->population;
            beans.Add(temp);
        }
        data2["beans"] = beans;
        gserver.SendObject(client, obj2);
        return;
    }
}
