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
        std::string reportid = data["idStr"];

        if (reportid.length() > 0) {
            char* l = new char[reportid.length() + 1];
            char* ll = l;
            l[reportid.length()] = 0;
            strcpy(l, reportid.c_str());
            while (true) {
                int32_t x = strtol(ll, &ll, 10);
                if (x == 0) {
                    gserver.SendObject(client,gserver.CreateError("report.deleteReport", -99, "Invalid report id."));
                    return;
                }
                std::list<stReport>::iterator iter;
                for (iter = client->reportlist.begin(); iter != client->reportlist.end(); ++iter) {
                    if (iter->reportid == x) {
                        client->reportlist.erase(iter);
                        break;
                    }
                }
                if (*ll == 0) break;
                ll++;
            }
            delete[] l;
        }
        obj2["cmd"] = "report.deleteReport";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        gserver.SendObject(client, obj2);
        client->ReportUpdate();
        return;
    }
    if (command == "markAsRead")
    {
        int reportid = data["reportId"];
        std::list<stReport>::iterator iter;
        stReport* r = nullptr;
        for (iter = client->reportlist.begin(); iter != client->reportlist.end(); ++iter) {
            if (iter->reportid == reportid) {
                r = &*iter;
            }
        }
        if (r==nullptr) {
            gserver.SendObject(client, gserver.CreateError("report.markAsRead", -99, "Invalid report id."));
            return;
        }
        obj2["cmd"] = "report.markAsRead";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        amf3object report = amf3object();
        report["id"] = reportid;
        report["selected"] = false;
        report["title"] = r->title;
        report["startPos"] = r->startpos;
        report["back"] = r->back;
        report["attack"] = r->attack;
        report["targetPos"] = r->targetpos;
        report["eventTime"] = (double)r->eventtime;
        report["type"] = r->type_id;
        report["armyType"] = r->armytype;
        report["isRead"] = r->isread;
        report["content"] = gserver.readreport(r->guid);
        data2["report"] = report;

        gserver.SendObject(client, obj2);

        r->isread = true;
        client->ReportUpdate();
        return;
    }
    if (command == "readOverReport")
    {
        std::string reportid = data["reportIds"];

        if (reportid.length() > 0) {
            char* l = new char[reportid.length() + 1];
            char* ll = l;
            l[reportid.length()] = 0;
            strcpy(l, reportid.c_str());
            while (true) {
                int32_t x = strtol(ll, &ll, 10);
                if (x == 0) {
                    gserver.SendObject(client, gserver.CreateError("report.readOverReport", -99, "Invalid report id."));
                    return;
                }
                std::list<stReport>::iterator iter;
                for (iter = client->reportlist.begin(); iter != client->reportlist.end(); ++iter) {
                    if (iter->reportid == x) {
                        iter->isread = true;
                        break;
                    }
                }
                if (*ll == 0) break;
                ll++;
            }
            delete[] l;
        }
        obj2["cmd"] = "report.readOverReport";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        gserver.SendObject(client, obj2);
        client->ReportUpdate();
        return;
    }
    if (command == "receiveReportList")
    {
        int pagesize = data["pageSize"];
        int type = data["reportType"];
        int pageno = data["pageNo"];

        if (pageno == 0)
            pageno++;


        std::list<stReport> * reportlist = &client->reportlist;

        int32_t count = 0;
        for (stReport& report : * reportlist)
        {
            if (report.type_id == type)
                count++;
        }

        obj2["cmd"] = "report.receiveReportList";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        amf3array reports = amf3array();

        if (pagesize <= 0 || pagesize > 1000 || pageno < 0 || pageno > 100000)
        {
            gserver.SendObject(client, gserver.CreateError("report.receiveReportList", -99, "Invalid data."));
            return;
        }

        if ((pageno - 1)*pagesize > count)
        {
            gserver.SendObject(client, gserver.CreateError("report.receiveReportList", -99, "Invalid page."));
            return;
        }
        std::list<stReport>::iterator iter;
        iter = reportlist->begin();
        for (int i = 0; i < (pageno - 1)*pagesize; ++i)
        {
            while (iter != reportlist->end())
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
        for (; iter != reportlist->end() && pagesize != 0; ++iter)
        {
            if (iter->type_id == type)
            {
                pagesize--;

                amf3object report;
                report["id"] = iter->reportid;
                report["selected"] = false;
                report["title"] = iter->title;
                report["startPos"] = iter->startpos;
                report["back"] = iter->back;
                report["attack"] = iter->attack;
                report["targetPos"] = iter->targetpos;
                report["eventTime"] = (double)iter->eventtime;
                report["type"] = iter->type_id;
                report["armyType"] = iter->armytype;
                report["isRead"] = iter->isread;
                report["content"] = amf3object();
                reports.Add(report);
            }
        }
        data2["reports"] = reports;
        gserver.SendObject(client, obj2);
        return;
    }
}
