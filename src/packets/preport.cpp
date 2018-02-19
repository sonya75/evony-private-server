/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "preport.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"
#include "../Hero.h"

preport::preport(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

preport::~preport()
{

}

void preport::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if (command == "deleteReport")
    {
        std::string reportid = data["idStr"];//always string? always a stringed number?

        amf3object obj3;
        obj3["cmd"] = "report.deleteReport";
        obj3["data"] = amf3object();
        amf3object & data3 = obj3["data"];

        data3["packageId"] = 0.0;
        data3["ok"] = 1;
        gserver.SendObject(client, obj3);
        return;
    }
    if (command == "markAsRead")
    {

        amf3object obj3;
        obj3["cmd"] = "report.receiveReportList";
        obj3["data"] = amf3object();
        amf3object & data3 = obj3["data"];

        amf3object report;
        report["id"] = 0;//report id (based on current reports you have, or ?)
        report["selected"] = false;
        report["title"] = "Plunder Reports";
        report["startPos"] = "City Name(317,374)";
        report["back"] = false;//??
        report["attack"] = false;//??
        report["targetPos"] = "Daisy(317,371)";
        report["eventTime"] = Utils::time();
        report["type"] = 1;//1 = plunder?
        report["armyType"] = 5;//5 = attack?
        report["isRead"] = 0;//0 = unread, 1 = read?
        report["content"] = "<reportData reportUrl=\"http://battleceshi3.evony.com/default.html?20140613/46/f1/46f16df3fb6ca832bc7ac1a182c99060.xml\">\
<battleReport isAttack=\"true\" isAttackSuccess=\"false\" round=\"100\">\
<attackTroop king=\"Daisy\" heroName=\"shitatt\" heroLevel=\"1\" heroUrl=\"images/icon/player/faceA21s.png\">\
<troopUnit typeId=\"2\" count=\"1\" lose=\"0\"/>\
</attackTroop>\
<defendTroop king=\"Daisy1\"/>\
<backTroop>\
<troops heroLevel=\"1\" heroName=\"shitatt\" heroUrl=\"images/icon/player/faceA21s.png\" isHeroBeSeized=\"false\">\
<troopInfo typeId=\"2\" remain=\"1\"/>\
</troops>\
</backTroop>\
</battleReport>\
</reportData>";
        data3["report"] = report;

        gserver.SendObject(client, obj3);

        client->ReportUpdate();
        return;
    }
    if (command == "readOverReport")
    {

    }
    if (command == "receiveReportList")
    {
        int pagesize = data["pageSize"];
        int type = data["type"];
        int pageno = data["pageNo"];

        if (pageno == 0)
            pageno++;


        std::list<stReport> * reportlist = &client->reportlist;

        int32_t count = 0;
        for (stReport report : * reportlist)
        {
            if (report.type_id == type)
                count++;
        }

        obj2["cmd"] = "report.receiveReportList";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        amf3array reports = amf3array();

        std::list<stReport>::reverse_iterator iter;
        if (pagesize <= 0 || pagesize > 20 || pageno < 0 || pageno > 100000)
        {
            gserver.SendObject(client, gserver.CreateError("report.receiveReportList", -99, "Invalid data."));
            return;
        }

        if ((pageno - 1)*pagesize > reportlist->size())
        {
            gserver.SendObject(client, gserver.CreateError("report.receiveReportList", -99, "Invalid page."));
            return;
        }
        iter = reportlist->rbegin();
        for (int i = 0; i < (pageno - 1)*pagesize; ++i)
        {
            while (iter != reportlist->rend())
            {
                if (iter->type_id == type)
                {
                    iter++;
                    break;
                }
                iter++;
            }
        }

        data2["pageNo"] = pageno;
        data2["pageSize"] = pagesize;
        if ((count % pagesize) == 0)
            data2["totalPage"] = (count / pagesize);
        else
            data2["totalPage"] = (count / pagesize) + 1;
        for (; iter != reportlist->rend() && pagesize != 0; ++iter)
        {
            if (iter->type_id == type)
            {
                pagesize--;

                amf3object report;
                report["id"] = iter->reportid;//report id (based on current reports you have, or ?)
                report["selected"] = false;
                report["title"] = iter->title;
                report["startPos"] = iter->startpos;
                report["back"] = iter->back;//??
                report["attack"] = iter->attack;//??
                report["targetPos"] = iter->targetpos;
                report["eventTime"] = iter->eventtime;
                report["type"] = iter->type_id;//1 = plunder?
                report["armyType"] = iter->armytype;//5 = attack?
                report["isRead"] = iter->isread();//0 = unread, 1 = read?
                reports.Add(report);
            }
        }
        data2["reports"] = reports;
        gserver.SendObject(client, obj2);
        return;
    }
}
