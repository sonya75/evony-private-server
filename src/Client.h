/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/


#include "amf3.h"
#include "PlayerCity.h"
#include "connection.h"
#include "structs.h"
#include "defines.h"

class spitfire;

#pragma once
class Client
{
public:
    Client();
    ~Client(void);

    connection * socket;

    amf3array Packages();
    amf3object ToObject();
    amf3array Items();
    amf3array SaleTypeItems();
    amf3array SaleItems();
    amf3array CastleArray();
    amf3array BuffsArray();
    amf3array CastleSignArray();
    amf3object PlayerInfo();

    bool SaveToDB();

    double Prestige() const
    {
        return prestige;
    }
    void Prestige(double pres)
    {
        prestige += pres;
        if (prestige < 0)
            prestige = 0;
        if (prestige > 2100000000)
            prestige = 2100000000;
    }

    void ParseBuffs(std::string str);
    void ParseResearch(std::string str);
    void ParseItems(std::string str);
    void ParseMisc(std::string str);

    void ParseCastleSign(std::string str);

    std::string DBBuffs();
    std::string DBResearch();
    std::string DBItems();
    std::string DBMisc();
    std::string DBCastleSign();

    void SetBuff(std::string type, std::string desc, int64_t endtime, int8_t param = 0);
    void RemoveBuff(std::string type);
    void AddItem(std::string type, int64_t dir);
    void SetItem(std::string type, int64_t amount);
    void SetResearch(uint16_t id, int16_t level, int32_t castleid, double starttime, double endtime);

    int16_t GetResearchLevel(int16_t id);
    int64_t GetItemCount(std::string type);
    int64_t GetItemCount(int16_t type);

    uint32_t mailpid;

    int64_t accountid;
    int64_t masteraccountid;

    bool accountexists;

    int32_t PACKETSIZE;// = 1024 * 150;
    uint64_t socknum;
    bool loggedin;// = false;
    uint64_t internalid;
    std::string playername;// = "Testname";
    std::string flag;// = "Evony";
    std::string faceurl;
    std::string alliancename;
    int16_t alliancerank;
    std::string office;
    std::string ipaddress;
    int64_t allianceid;
    std::string email;
    std::string password;
    double prestige;
    double honor;
    bool beginner;
    int32_t status;
    int32_t rank;
    int32_t title;
    double connected;
    double lastlogin;
    double creation;
    //char * readytoprocess = new char[PACKETSIZE];
    int32_t processoffset;
    int32_t citycount;
    int32_t sex;
    int32_t population;
    uint32_t prestigerank;
    uint32_t honorrank;
    std::vector<PlayerCity*> citylist;// = new ArrayList();
    int m_bdenyotherplayer;
    int tempvar;
    std::string allianceapply;
    int64_t allianceapplytime;
    bool haschangedface;
    uint16_t changeface;//new castle designs
    uint32_t currentreportid;

    int8_t icon;

    uint64_t cents;

    uint32_t currentcityindex;
    uint32_t currentcityid;

    double clientdelay;

    bool debugmode;


    std::vector<stPackage> packagelist;
    std::vector<stBuff> bufflist;
    std::list<stReport> reportlist;
    std::list<stMail> maillist;
    std::vector<stItem> itemlist;
    stResearch research[25];

    std::vector<stCastleSign> castlesignlist;

    stBuff * GetBuff(std::string type)
    {
        for (auto i = 0; i < bufflist.size(); ++i)
        {
            if (bufflist[i].id == type)
                return &bufflist[i];
        }
        return nullptr;
    }

    std::list<stArmyMovement*> armymovement;
    std::list<stArmyMovement*> friendarmymovement;
    std::list<stArmyMovement*> enemyarmymovement;

    void CalculateResources();
    void PlayerInfoUpdate();
    void ItemUpdate(std::string itemname);
    void BuffUpdate(std::string name, std::string desc, int64_t endtime, int8_t type = 0);
    void HeroUpdate(int heroid, int castleid);
    void MailUpdate();
    void ReportUpdate();
    void EnemyArmyUpdate();
    void FriendArmyUpdate();
    void SelfArmyUpdate();
    void CastleSignUpdate();
    bool HasAlliance() const
    {
        if (allianceid > 0)
            return true;
        return false;
    }
    Alliance * GetAlliance();

    PlayerCity * GetCity(int64_t castleid);
    PlayerCity * GetFocusCity();
    bool Beginner() const { return beginner; }
    void Beginner(bool set, bool update = true)
    {
        if (beginner != set)
        {
            beginner = set;
            if (update)
                PlayerInfoUpdate();
        }
    }
    inline void CheckBeginner(bool update = true)
    {
        if (Beginner())
        {
            if ((Utils::time() - creation) > 1000 * 60 * 60 * 24 * 7)
            {
                Beginner(false, update);
                return;
            }
            for (auto i = 0; i < this->citylist.size(); ++i)
            {
                if (citylist[i]->GetBuildingLevel(B_TOWNHALL) >= 5)
                {
                    Beginner(false, update);
                    return;
                }
            }
        }
    }
};
