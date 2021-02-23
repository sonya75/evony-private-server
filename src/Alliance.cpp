/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "Alliance.h"
#include "AllianceMgr.h"
#include "spitfire.h"
#include "Client.h"
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/Session.h>

using namespace Poco::Data;
using namespace Poco::Data::Keywords;

Alliance::Alliance(std::string name, std::string founder)
{
    enemyactioncooldown = 0;
    m_name = name;
    m_members.clear();
    m_enemies.clear();
    m_allies.clear();
    m_neutral.clear();
    m_currentmembers = 0;
    m_maxmembers = 500;
    //m_members.push_back(ownerid);
    m_prestige = 0;
    m_honor = 0;
    m_prestigerank = 0;
    m_honorrank = 0;
    m_membersrank = 0;
    m_founder = founder;
    m_owner = "";
    //m_citycount = 0;
    m_allicitycount = 0;
}

Alliance::~Alliance()
{

}

bool Alliance::InsertToDB()
{
    typedef Poco::Tuple<std::string, std::string, std::string, int64_t> AllianceSave;
    //name, founder, leader

    AllianceSave savedata(m_name, m_founder, m_owner, Utils::time());

    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "INSERT INTO `alliances` (name,founder,leader,created,note,intro,motd,allies,neutrals,enemies,members) VALUES (?,?,?,?,'','','','','','','');", use(savedata), now;

        Statement lastinsert = (ses << "SELECT " + LAST_INSERT_ID);
        lastinsert.execute();
        RecordSet lsi(lastinsert);
        lsi.moveFirst();
        int64_t lsiv = lsi.value(LAST_INSERT_ID).convert<int64_t>();
        if (lsiv > 0)
        {
            m_allianceid = lsiv;
        }
        else
        {
            spitfire::GetSingleton().log->error("Unable to create alliance.");
            return false;
        }

        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

bool Alliance::DeleteFromDB()
{
    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "DELETE FROM `alliances` WHERE id=?;", use(m_allianceid), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

bool Alliance::SaveToDB()
{
    typedef Poco::Tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string> AllianceSave;
    //name, founder, leader, note, intro, motd, allies, neutrals, enemies, members


    std::vector<Poco::Any> args;
    std::stringstream ss;

    std::string allies = "";
    std::string neutrals = "";
    std::string enemies = "";
    std::string members = "";

    for (int64_t id : m_allies)
    {
        args.push_back(id);
        ss << "%?d|";
    }
    Poco::format(allies, ss.str(), args);

    for (int64_t id : m_neutral)
    {
        args.push_back(id);
        ss << "%?d|";
    }
    Poco::format(neutrals, ss.str(), args);

    for (int64_t id : m_enemies)
    {
        args.push_back(id);
        ss << "%?d|";
    }
    Poco::format(enemies, ss.str(), args);

    for (stMember & member : m_members)
    {
        args.push_back(member.clientid);
        args.push_back(member.rank);
        ss << "%?d,%?d|";
    }
    Poco::format(members, ss.str(), args);

    AllianceSave savedata(m_name, m_founder, m_owner, m_note, m_intro, m_motd, allies, neutrals, enemies, members);


    //name, founder, leader, note, intro, motd, allies, neutrals, enemies, members
    try
    {
        Poco::Data::Session ses(spitfire::GetSingleton().serverpool->get());
        ses << "UPDATE `alliances` SET name=?,founder=?,leader=?,note=?,intro=?,motd=?,allies=?,neutrals=?,enemies=?,members=? WHERE id=?;", use(savedata), use(m_allianceid), now;
        return true;
    }
    SQLCATCH3(0, spitfire::GetSingleton());

    return false;
}

amf3object Alliance::ToObject()
{
    amf3object obj = amf3object();

    return obj;
}

bool Alliance::IsEnemy(int64_t allianceid)
{
    if (allianceid <= 0) return false;
    for (int64_t _id : m_enemies)
    {
        if (allianceid == _id)
        {
            return true;
        }
    }
    return false;
}

bool Alliance::IsAlly(int64_t allianceid)
{
    if (allianceid <= 0) return false;
    for (int64_t _id : m_allies)
    {
        if (allianceid == _id)
        {
            return true;
        }
    }
    return false;
}

bool Alliance::IsNeutral(int64_t allianceid)
{
    if (allianceid <= 0) return false;
    for (int64_t _id : m_neutral)
    {
        if (allianceid == _id)
        {
            return true;
        }
    }
    return false;
}

bool Alliance::AddMember(uint64_t clientid, uint8_t rank)
{
    if (HasMember(clientid))
    {
        //bug point - should never trigger but is in certain unknown circumstances
        return true;
    }
    if (m_currentmembers >= m_maxmembers)
    {
        return false;
    }
    stMember member;
    member.clientid = clientid;
    member.rank = rank;
    m_members.push_back(member);
    m_currentmembers++;
    return true;
}

bool Alliance::HasMember(std::string username)
{
    Client * client = spitfire::GetSingleton().GetClientByName(username);
    if (!client)
        return false;
    return HasMember(client->accountid);
}

bool Alliance::HasMember(uint64_t clientid)
{
    std::list<Alliance::stMember>::iterator iter;
    for (iter = m_members.begin(); iter != m_members.end(); ++iter)
    {
        if (iter->clientid == clientid)
        {
            return true;
        }
    }
    return false;
}

bool Alliance::RemoveMember(uint64_t clientid)
{
    std::list<Alliance::stMember>::iterator iter;
    for (iter = m_members.begin(); iter != m_members.end(); )
    {
        if (iter->clientid == clientid)
        {
            m_members.erase(iter);
            m_currentmembers--;
            return true;
        }
        ++iter;
    }
    return true;
}

void Alliance::RequestJoin(Client * client, uint64_t timestamp)
{
    stInviteList invite;
    invite.invitetime = timestamp;
    invite.client = client;
    m_invites.push_back(invite);
}

void Alliance::UnRequestJoin(Client * client)
{
    std::list<stInviteList> tempinvites = m_invites;
    for (stInviteList & invite : tempinvites)
    {
        if (invite.client->playername == client->playername)
        {
            m_invites.remove(invite);
        }
    }
}

void Alliance::UnRequestJoin(std::string client)
{
    std::list<stInviteList> tempinvites = m_invites;
    for (stInviteList & invite : tempinvites)
    {
        if (invite.client->playername == client)
        {
            m_invites.remove(invite);
        }
    }
}

void Alliance::ParseMembers(std::string str)
{
    if (str.length() > 0)
    {
        uint32_t clientid;
        uint8_t rank;

        std::vector<std::string> outertokens;
        std::vector<std::string> tokens;
        //boost::split(outertokens, str, boost::is_any_of("|"));
        my_split(outertokens, str, "|");

        for (std::string token : outertokens)
        {
            my_split(tokens, token, ",");
            //boost::split(tokens, token, boost::is_any_of(","));
            if (tokens.size() == 2)
            {
                clientid = atoi(tokens[0].c_str());
                rank = atoi(tokens[1].c_str());

                Client * client = spitfire::GetSingleton().GetClient(clientid);
                if (client)
                {
                    AddMember(clientid, rank);
                    client->allianceid = this->m_allianceid;
                    client->alliancerank = rank;
                }
            }
        }
    }
}

void Alliance::ParseRelation(std::list<int64_t> * list, std::string str)
{
    if (str.length() > 0)
    {
        uint64_t allianceid;

        std::vector<std::string> tokens;
        my_split(tokens, str, "|");
        //boost::split(tokens, str, boost::is_any_of("|"));

        for (std::string token : tokens)
        {
            allianceid = atoi(token.c_str());
            list->push_back(allianceid);
        }
    }
}

amf3object Alliance::indexAllianceInfoBean()
{
    amf3object allianceinfo;
    allianceinfo["prestige"] = m_prestige;
    allianceinfo["rank"] = m_prestigerank;
    allianceinfo["creatorName"] = m_founder.c_str();
    allianceinfo["allianceNote"] = m_note.c_str();
    allianceinfo["allianceInfo"] = m_intro.c_str();
    allianceinfo["allianceName"] = m_name.c_str();
    allianceinfo["memberCount"] = m_currentmembers;
    allianceinfo["memberLimit"] = m_maxmembers;
    allianceinfo["leaderName"] = m_owner.c_str();
    return allianceinfo;
};

void Alliance::Ally(int64_t allianceid)
{
    if (IsAlly(allianceid))
        return;
    if (IsNeutral(allianceid))
        UnNeutral(allianceid);
    if (IsEnemy(allianceid))
        UnEnemy(allianceid);
    m_allies.push_back(allianceid);
    Alliance * temp = spitfire::GetSingleton().m_alliances->AllianceById(allianceid);
    temp->SendAllianceMessage("Alliance [" + temp->m_name + "] recognizes Diplomatic Relationship with us as Ally.", false, false);
}
void Alliance::Neutral(int64_t allianceid)
{
    if (IsNeutral(allianceid))
        return;
    if (IsAlly(allianceid))
        UnAlly(allianceid);
    if (IsEnemy(allianceid))
        UnEnemy(allianceid);
    m_neutral.push_back(allianceid);
    Alliance * temp = spitfire::GetSingleton().m_alliances->AllianceById(allianceid);
    temp->SendAllianceMessage("Alliance [" + temp->m_name + "] recognizes Diplomatic Relationship with us as Neutral.", false, false);
}
void Alliance::Enemy(int64_t allianceid, bool skip /* = false*/)
{
    if (IsEnemy(allianceid))
        return;
    enemyactioncooldown = Utils::time() + 1000 * 60 * 60 * 24;
    if (IsNeutral(allianceid))
        UnNeutral(allianceid);
    if (IsAlly(allianceid))
        UnAlly(allianceid);
    m_enemies.push_back(allianceid);
    if (skip)
        return;
    //send global message
    Alliance * temp = spitfire::GetSingleton().m_alliances->AllianceById(allianceid);
    spitfire::GetSingleton().MassMessage("Alliance " + this->m_name + " declares war against alliance " + temp->m_name + ". Diplomatic Relationship between each other alters to Hostile automatically.");
    temp->Enemy(m_allianceid, true);
    temp->SendAllianceMessage("Alliance [" + m_name + "] recognizes Diplomatic Relationship with us as Enemy.", false, false);
}
void Alliance::UnAlly(int64_t allianceid)
{
    m_allies.remove(allianceid);
}
void Alliance::UnNeutral(int64_t allianceid)
{
    m_neutral.remove(allianceid);
}
void Alliance::UnEnemy(int64_t allianceid)
{
    m_enemies.remove(allianceid);
}

void Alliance::SendAllianceMessage(std::string msg, bool tv, bool nosender)
{
    amf3object obj = amf3object();
    amf3object data = amf3object();

    obj["cmd"] = "server.SystemInfoMsg";
    data["alliance"] = true;
    data["sender"] = "Alliance Msg";
    data["tV"] = tv;
    data["msg"] = msg;
    data["noSenderSystemInfo"] = nosender;

    obj["data"] = data;

    for (Alliance::stMember & client : m_members)
    {
        spitfire::GetSingleton().SendObject(spitfire::GetSingleton().GetClient(client.clientid), obj);
    }
}
