/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "palliance.h"
#include "../Client.h"
#include "../City.h"
#include "../spitfire.h"
#include "../Alliance.h"
#include "../AllianceMgr.h"

palliance::palliance(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

palliance::~palliance()
{

}

void palliance::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if ((command == "sayByetoAlliance"))
    {
        if (client->HasAlliance())
        {
            Alliance * alliance = client->GetAlliance();
            alliance->RemoveMember(client->accountid);
            alliance->SendAllianceMessage(client->playername + " has left the alliance", false, true);
            client->allianceid = -1;
            client->alliancename = "";
            client->alliancerank = 0;
            client->allianceapply = "";
            client->allianceapplytime = 0;

            obj2["cmd"] = "alliance.sayByetoAlliance";
            data2["ok"] = 1;
            data2["packageId"] = 0.0;
            gserver.SendObject(client, obj2);
            gserver.m_alliances->CheckAlliance(alliance);
        }
        return;
    }
    if ((command == "isHasAlliance"))
    {
        obj2["cmd"] = "alliance.isHasAlliance";


        if (client->HasAlliance())
        {
            data2["ok"] = 1;
            data2["packageId"] = 0.0;


            Alliance * alliance = client->GetAlliance();
            Alliance * tempalliance;

            data2["serialVersionUID"] = 1.0;

            amf3array middleList = amf3array();
            amf3array enemyList = amf3array();
            amf3array friendlyList = amf3array();

            if (alliance->m_neutral.size() > 0)
            {
                for (int64_t target : alliance->m_neutral)
                {
                    tempalliance = gserver.m_alliances->AllianceById(target);
                    if (tempalliance != (Alliance*)-1)
                    {
                        amf3object ta = amf3object();
                        ta["rank"] = tempalliance->m_prestigerank;
                        ta["honor"] = tempalliance->m_honor;// HACK: might be 0 -- alliance.isHasAlliance
                        ta["allianceName"] = tempalliance->m_name.c_str();
                        ta["memberCount"] = tempalliance->m_currentmembers;
                        ta["aPrestigeCount"] = tempalliance->m_prestige;// HACK: might be 0 -- alliance.isHasAlliance
                        middleList.Add(ta);
                    }
                }
            }
            if (alliance->m_enemies.size() > 0)
            {
                for (int64_t target : alliance->m_enemies)
                {
                    tempalliance = gserver.m_alliances->AllianceById(target);
                    if (tempalliance != (Alliance*)-1)
                    {
                        amf3object ta = amf3object();
                        ta["rank"] = tempalliance->m_prestigerank;
                        ta["honor"] = tempalliance->m_honor;// HACK: might be 0 -- alliance.isHasAlliance
                        ta["allianceName"] = tempalliance->m_name.c_str();
                        ta["memberCount"] = tempalliance->m_currentmembers;
                        ta["aPrestigeCount"] = tempalliance->m_prestige;// HACK: might be 0 -- alliance.isHasAlliance
                        enemyList.Add(ta);
                    }
                }
            }
            if (alliance->m_allies.size() > 0)
            {
                for (int64_t target : alliance->m_allies)
                {
                    tempalliance = gserver.m_alliances->AllianceById(target);
                    if (tempalliance != (Alliance*)-1)
                    {
                        amf3object ta = amf3object();
                        ta["rank"] = tempalliance->m_prestigerank;
                        ta["honor"] = tempalliance->m_honor;// HACK: might be 0 -- alliance.isHasAlliance
                        ta["allianceName"] = tempalliance->m_name.c_str();
                        ta["memberCount"] = tempalliance->m_currentmembers;
                        ta["aPrestigeCount"] = tempalliance->m_prestige;// HACK: might be 0 -- alliance.isHasAlliance
                        friendlyList.Add(ta);
                    }
                }
            }
            data2["middleList"] = middleList;
            data2["enemyList"] = enemyList;
            data2["friendlyList"] = friendlyList;


            data2["indexAllianceInfoBean"] = alliance->indexAllianceInfoBean();
        }
        else
        {
            data2["ok"] = -99;
            data2["packageId"] = 0.0;
            data2["serialVersionUID"] = 0.0;
            data2["errorMsg"] = "You are not in any alliance.";
        }

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "leaderWantUserInAllianceList"))
    {
        obj2["cmd"] = "alliance.leaderWantUserInAllianceList";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        data2["allianceAddPlayerByLeaderInfoBeanList"] = amf3array();

        //TODO: this is your list of alliances that have invited you?-- alliance.leaderWantUserInAllianceList

        amf3array & allianceinfo = *(amf3array*)data2["allianceAddPlayerByLeaderInfoBeanList"];

        for (int i = 0; i < DEF_MAXALLIANCES; ++i)
        {
            if (gserver.m_alliances->m_alliances[i])
            {
                Alliance * alliance = gserver.m_alliances->m_alliances[i];
                if (alliance->m_invites.size())
                {
                    for (Alliance::stInviteList & invite : alliance->m_invites)
                    {
                        if ((invite.client->accountid == client->accountid) && (invite.inviteperson.length() > 0))
                        {
                            amf3object invite = amf3object();
                            invite["prestige"] = alliance->m_prestige;
                            invite["rank"] = alliance->m_prestigerank;
                            invite["allianceName"] = alliance->m_name;
                            invite["memberCount"] = alliance->m_currentmembers;
                            allianceinfo.Add(invite);
                        }
                    }
                }
            }
        }

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "rejectComeinAlliance"))
    {
        obj2["cmd"] = "alliance.rejectComeinAlliance";
        data2["packageId"] = 0.0;

        std::string alliancename = data["allianceName"];

        Alliance * alliance = gserver.m_alliances->AllianceByName(alliancename);
        if (alliance == (Alliance*)-1)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.rejectComeinAlliance", -99, "Alliance no longer exists."));
            return;
        }

        if (alliance->m_invites.size())
        {
            for (Alliance::stInviteList & invite : alliance->m_invites)
            {
                if (invite.client->accountid == client->accountid)
                {
                    alliance->m_invites.remove(invite);
                    data2["ok"] = 1;

                    gserver.SendObject(client, obj2);
                    return;
                }
            }
        }
        else
        {
            gserver.SendObject(client, gserver.CreateError("alliance.rejectComeinAlliance", -99, "Invite no longer exists."));
            return;
        }
        gserver.SendObject(client, gserver.CreateError("alliance.rejectComeinAlliance", -99, "An unknown error occurred."));
        return;
    }
    if ((command == "agreeComeinAllianceList"))
    {
        obj2["cmd"] = "alliance.agreeComeinAllianceList";
        data2["packageId"] = 0.0;

        if ((client->allianceid == 0) || (client->alliancerank > DEF_ALLIANCEPRES))
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceList", -99, "Not enough rank."));
            return;
        }

        Alliance * alliance = gserver.m_alliances->AllianceById(client->allianceid);

        if (alliance == (Alliance*)-1)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceList", -99, "Alliance does not exist."));
            return;
        }

        amf3array allianceinfo = amf3array();

        for (Alliance::stInviteList & invite : alliance->m_invites)
        {
            if (invite.inviteperson.length() == 0)
            {
                Client * pcl = invite.client;
                amf3object iobj = amf3object();
                iobj["prestige"] = pcl->Prestige();
                iobj["rank"] = pcl->prestigerank;
                iobj["userName"] = pcl->playername;
                iobj["inviteTime"] = invite.invitetime;
                iobj["castleCount"] = pcl->citycount;
                allianceinfo.Add(iobj);
            }
        }
        data2["ok"] = 1;

        data2["allianceAddPlayerByUserInfoBeanList"] = allianceinfo;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "resignForAlliance"))
    {
        obj2["cmd"] = "alliance.resignForAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.resignForAlliance", -99, "You are not in an alliance."));
            return;
        }
        Alliance * alliance = client->GetAlliance();
        if ((client->alliancerank == DEF_ALLIANCEHOST) && (alliance->m_currentmembers > 1))
        {
            gserver.SendObject(client, gserver.CreateError("alliance.resignForAlliance", -99, "Resignation refused. Please transfer your host title to other before you resign."));
            return;
        }
        else
        {
            //Only person left is host - disband alliance
            if (!gserver.m_alliances->RemoveFromAlliance(client->allianceid, client))
            {
                gserver.SendObject(client, gserver.CreateError("alliance.resignForAlliance", -99, "Unable to leave alliance. Please contact support."));
                return;
            }
            gserver.m_alliances->DeleteAlliance(alliance);
            return;
        }

        if (!gserver.m_alliances->RemoveFromAlliance(client->allianceid, client))
        {
            gserver.SendObject(client, gserver.CreateError("alliance.resignForAlliance", -99, "Unable to leave alliance. Please contact support."));
            return;
        }

        data2["ok"] = 1;

        client->SaveToDB();

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getAllianceMembers"))
    {
        obj2["cmd"] = "alliance.getAllianceMembers";
        data2["packageId"] = 0.0;

        if (client->allianceid < 0)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.getAllianceMembers", -99, "Not a member of an alliance."));
            return;
        }
        Alliance * alliance = gserver.m_alliances->AllianceById(client->allianceid);

        if (alliance == (Alliance*)-1)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.getAllianceMembers", -99, "Invalid Alliance."));
            return;
        }

        amf3array members = amf3array();

        for (Alliance::stMember & member : alliance->m_members)
        {
            Client * client = gserver.GetClient(member.clientid);
            if (client)
            {
                amf3object temp = amf3object();
                temp["createrTime"] = 0;
                temp["alliance"] = alliance->m_name;
                temp["office"] = client->office;
                temp["allianceLevel"] = AllianceMgr::GetAllianceRank(client->alliancerank);
                temp["sex"] = client->sex;
                temp["levelId"] = client->alliancerank;
                temp["honor"] = client->honor;
                temp["bdenyotherplayer"] = client->m_bdenyotherplayer;
                temp["id"] = client->accountid;
                temp["accountName"] = "";
                temp["prestige"] = client->Prestige();
                temp["faceUrl"] = client->faceurl;
                temp["flag"] = client->flag;
                temp["userId"] = client->masteraccountid;
                temp["userName"] = client->playername;
                temp["castleCount"] = client->citycount;
                temp["titleId"] = client->title;
                temp["medal"] = 0;
                temp["ranking"] = client->prestigerank;
                temp["lastLoginTime"] = client->lastlogin;
                temp["population"] = client->population;
                members.Add(temp);
            }
        }

        data2["ok"] = 1;
        data2["members"] = members;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getAllianceWanted"))
    {
        obj2["cmd"] = "alliance.getAllianceWanted";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        if (client->allianceapply.length() > 0)
            data2["allianceName"] = client->allianceapply;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getAllianceInfo"))
    {
        obj2["cmd"] = "alliance.getAllianceInfo";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        std::string alliancename = data["allianceName"];

        Alliance * alliance = gserver.m_alliances->AllianceByName(alliancename);

        if (alliance == (Alliance*)-1)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.getAllianceInfo", -99, "Alliance does not exist."));
            return;
        }

        data2["leader"] = alliance->m_owner;
        data2["prestigeCount"] = alliance->m_prestige;
        data2["ranking"] = alliance->m_prestigerank;
        data2["memberCount"] = alliance->m_currentmembers;
        data2["allinaceInfo"] = alliance->m_intro;
        data2["creator"] = alliance->m_founder;


        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "userWantInAlliance"))
    {
        obj2["cmd"] = "alliance.userWantInAlliance";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        if (client->GetFocusCity()->GetBuildingLevel(B_EMBASSY) < 1)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.userWantInAlliance", -99, "You need to build an embassy first."));
            return;
        }

        std::string alliancename = data["allianceName"];

        Alliance * alliance = gserver.m_alliances->AllianceByName(alliancename);

        if (alliance == (Alliance*)-1)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.userWantInAlliance", -99, "Alliance does not exist."));
            return;
        }

        if (client->allianceapply.length() != 0)
        {
            gserver.m_alliances->AllianceByName(client->allianceapply)->UnRequestJoin(client);
        }

        alliance->RequestJoin(client, timestamp);


        client->allianceapply = alliancename;
        client->allianceapplytime = timestamp;

        alliance->SendAllianceMessage(client->playername + " has applied to join your alliance", false, false);

        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        client->SaveToDB();

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "cancelUserWantInAlliance"))
    {
        obj2["cmd"] = "alliance.cancelUserWantInAlliance";
        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        std::string alliancename = data["allianceName"];

        if (client->allianceapply.length() == 0)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.cancelUserWantInAlliance", -99, "Not applied."));
            return;
        }

        Alliance * alliance = gserver.m_alliances->AllianceByName(alliancename);

        if (alliance == (Alliance*)-1)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.cancelUserWantInAlliance", -99, "Alliance does not exist."));
            return;
        }

        alliance->UnRequestJoin(client);


        client->allianceapply = "";
        client->allianceapplytime = 0;

        client->SaveToDB();

        data2["ok"] = 1;
        data2["packageId"] = 0.0;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "getMilitarySituationList"))
    {
        int pagesize = data["pageSize"];
        int pageno = data["pageNo"];

        obj2["cmd"] = "alliance.getMilitarySituationList";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "addUsertoAllianceList"))
    {
        obj2["cmd"] = "alliance.addUsertoAllianceList";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.addUsertoAllianceList", -99, "Not a member of an alliance."));
            return;
        }

        data2["ok"] = 1;

        amf3array listarray = amf3array();

        Alliance * alliance = client->GetAlliance();

        for (Alliance::stInviteList & invite : alliance->m_invites)
        {
            if (invite.inviteperson.length() != 0)
            {
                Client * pcl = invite.client;
                amf3object iobj = amf3object();
                iobj["prestige"] = pcl->prestige;
                iobj["rank"] = pcl->prestigerank;
                iobj["invitePerson"] = invite.inviteperson;
                iobj["userName"] = pcl->playername;
                iobj["inviteTime"] = invite.invitetime;
                listarray.Add(iobj);
            }
        }

        data2["allianceAddPlayerInfoBeanList"] = listarray;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "addUsertoAlliance"))
    {
        //invite player
        obj2["cmd"] = "alliance.addUsertoAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.addUsertoAlliance", -99, "Not a member of an alliance."));
            return;
        }
        if (client->alliancerank > DEF_ALLIANCEPRES)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.addUsertoAlliance", -99, "Not enough rank."));
            return;
        }

        //TODO: copy 24h alliance join cooldown?
        /*
        ["data"] Type: Object - Value: Object
        ["packageId"] Type: Number - Value: 0.000000
        ["ok"] Type: Integer - Value: -301
        ["errorMsg"] Type: String - Value: Can not join the same Alliance again in 23 hours.
        */

        std::string username = data["userName"];
        Client * invitee = gserver.GetClientByName(username);

        if (invitee == 0)
        {
            std::string msg;
            msg = "Player ";
            msg += username;
            msg += " doesn't exist.";
            gserver.SendObject(client, gserver.CreateError("alliance.addUsertoAlliance", -41, msg));
            return;
        }
        if (invitee->HasAlliance())
        {
            std::string msg;
            msg = username;
            msg += "is already a member of other alliance.";
            gserver.SendObject(client, gserver.CreateError("alliance.addUsertoAlliance", -99, msg));
            return;
        }
        data2["ok"] = 1;

        Alliance * alliance = client->GetAlliance();

        Alliance::stInviteList invite;
        invite.inviteperson = client->playername;
        invite.invitetime = timestamp;
        invite.client = invitee;

        alliance->m_invites.push_back(invite);

        gserver.SendMessage(invitee, "You have been invited to the alliance " + alliance->m_name);

        gserver.SendObject(client, obj2);

        alliance->SaveToDB();

        return;
    }
    if ((command == "canceladdUsertoAlliance"))
    {
        //TODO: permission check
        obj2["cmd"] = "alliance.canceladdUsertoAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.canceladdUsertoAlliance", -99, "Not a member of an alliance."));
            return;
        }

        std::string username = data["userName"];

        data2["ok"] = 1;

        Alliance * alliance = client->GetAlliance();

        alliance->UnRequestJoin(username);

        gserver.SendObject(client, obj2);

        alliance->SaveToDB();

        return;
    }
    if ((command == "cancelagreeComeinAlliance"))
    {
        obj2["cmd"] = "alliance.cancelagreeComeinAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.cancelagreeComeinAlliance", -99, "Not a member of an alliance."));
            return;
        }

        std::string username = data["userName"];

        data2["ok"] = 1;

        Alliance * alliance = client->GetAlliance();

        alliance->UnRequestJoin(username);

        gserver.SendObject(client, obj2);

        alliance->SaveToDB();

        return;
    }
    if ((command == "setAllianceFriendship"))
    {
        //TODO: permission check
        obj2["cmd"] = "alliance.setAllianceFriendship";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.setAllianceFriendship", -99, "Not a member of an alliance."));
            return;
        }

        //1 friendly
        //2 neutral
        //3 enemy

        int alliancetype = data["type"];
        std::string otheralliancename = data["targetAllianceName"];

        Alliance * otheralliance = gserver.m_alliances->AllianceByName(otheralliancename);
        if (otheralliance == (Alliance*)-1)
        {
            //doesn't exist
            gserver.SendObject(client, gserver.CreateError("alliance.setAllianceFriendship", -99, "Alliance does not exist."));
            return;
        }

        Alliance * alliance = client->GetAlliance();

        if (alliancetype == 1)
        {
            if (alliance->IsAlly(otheralliance->m_allianceid))
            {
                //already allied
                gserver.SendObject(client, gserver.CreateError("alliance.setAllianceFriendship", -99, "Alliance is already an ally."));
                return;
            }
            alliance->Ally(otheralliance->m_allianceid);
        }
        else if (alliancetype == 2)
        {
            if (alliance->IsNeutral(otheralliance->m_allianceid))
            {
                //already neutral
                gserver.SendObject(client, gserver.CreateError("alliance.setAllianceFriendship", -99, "Alliance is already neutral."));
                return;
            }
            alliance->Neutral(otheralliance->m_allianceid);
        }
        else //alliancetype = 3
        {
            if (alliance->IsEnemy(otheralliance->m_allianceid))
            {
                //already enemy
                gserver.SendObject(client, gserver.CreateError("alliance.setAllianceFriendship", -99, "Alliance is already an enemy."));
                return;
            }
            // TODO copy declare war cooldown?
            if (Utils::time() < alliance->enemyactioncooldown)
            {
                //Declared war too soon
                gserver.SendObject(client, gserver.CreateError("alliance.setAllianceFriendship", -99, "You have already declared war recently."));
                return;
            }

            alliance->Enemy(otheralliance->m_allianceid);
        }
        data2["ok"] = 1;
        gserver.SendObject(client, obj2);

        alliance->SaveToDB();
        return;
    }
    if ((command == "getAllianceFriendshipList"))
    {
        // TODO War reports -- alliance.getMilitarySituationList
        obj2["cmd"] = "alliance.getAllianceFriendshipList";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.getAllianceFriendshipList", -99, "Not a member of an alliance."));
            return;
        }

        data2["ok"] = 1;

        Alliance * tempalliance = (Alliance*)-1;
        Alliance * alliance = client->GetAlliance();

        amf3array middleList = amf3array();
        amf3array enemyList = amf3array();
        amf3array friendlyList = amf3array();

        std::vector<int64_t>::iterator iter;
        if (alliance->m_neutral.size() > 0)
        {
            for (int64_t target : alliance->m_neutral)
            {
                tempalliance = gserver.m_alliances->AllianceById(target);
                if (tempalliance != (Alliance*)-1)
                {
                    amf3object ta = amf3object();
                    ta["rank"] = tempalliance->m_prestigerank;
                    ta["honor"] = tempalliance->m_honor;// HACK might be 0 -- alliance.isHasAlliance
                    ta["allianceName"] = tempalliance->m_name.c_str();
                    ta["memberCount"] = tempalliance->m_currentmembers;
                    ta["aPrestigeCount"] = tempalliance->m_prestige;// HACK might be 0 -- alliance.isHasAlliance
                    middleList.Add(ta);
                }
            }
        }
        if (alliance->m_enemies.size() > 0)
        {
            for (int64_t target : alliance->m_enemies)
            {
                tempalliance = gserver.m_alliances->AllianceById(target);
                if (tempalliance != (Alliance*)-1)
                {
                    amf3object ta = amf3object();
                    ta["rank"] = tempalliance->m_prestigerank;
                    ta["honor"] = tempalliance->m_honor;// HACK might be 0 -- alliance.isHasAlliance
                    ta["allianceName"] = tempalliance->m_name.c_str();
                    ta["memberCount"] = tempalliance->m_currentmembers;
                    ta["aPrestigeCount"] = tempalliance->m_prestige;// HACK might be 0 -- alliance.isHasAlliance
                    enemyList.Add(ta);
                }
            }
        }
        if (alliance->m_allies.size() > 0)
        {
            for (int64_t target : alliance->m_allies)
            {
                tempalliance = gserver.m_alliances->AllianceById(target);
                if (tempalliance != (Alliance*)-1)
                {
                    amf3object ta = amf3object();
                    ta["rank"] = tempalliance->m_prestigerank;
                    ta["honor"] = tempalliance->m_honor;// HACK might be 0 -- alliance.isHasAlliance
                    ta["allianceName"] = tempalliance->m_name.c_str();
                    ta["memberCount"] = tempalliance->m_currentmembers;
                    ta["aPrestigeCount"] = tempalliance->m_prestige;// HACK might be 0 -- alliance.isHasAlliance
                    friendlyList.Add(ta);
                }
            }
        }
        data2["middleList"] = middleList;
        data2["enemyList"] = enemyList;
        data2["friendlyList"] = friendlyList;

        gserver.SendObject(client, obj2);
        return;
    }
    if ((command == "createAlliance"))
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        obj2["cmd"] = "alliance.createAlliance";
        data2["packageId"] = 0.0;

        std::string alliancename = data["allianceName"];

        if (city->m_resources.gold < 10000)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.createAlliance", -99, "Not enough gold."));
            return;
        }

        if (!gserver.m_alliances->CheckName(alliancename) || alliancename.size() < 2)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.createAlliance", -99, "Illegal naming, please choose another name."));
            return;
        }

        if (gserver.m_alliances->AllianceByName(alliancename) != (Alliance*)-1)
        {
            std::string error = "Alliance already existed: ";
            error += alliancename;
            error += ".";

            gserver.SendObject(client, gserver.CreateError("alliance.createAlliance", -12, error));
            return;
        }
        Alliance * alliance = 0;
        if ((alliance = gserver.m_alliances->CreateAlliance(alliancename, client->playername)))
        {
            data2["ok"] = 1;

            alliance->m_owner = client->playername;
            alliance->m_ownerid = client->accountid;

            std::string error = "Establish alliance ";
            error += alliancename;
            error += " successfully.";
            data2["msg"] = error;

            city->m_resources.gold -= 10000;

            if (!gserver.m_alliances->JoinAlliance(alliance->m_allianceid, client))
            {
                client->PlayerUpdate();
                city->ResourceUpdate();

                gserver.SendObject(client, gserver.CreateError("alliance.createAlliance", -99, "Alliance created but cannot join. Please contact support."));
                return;
            }
            if (!gserver.m_alliances->SetRank(alliance->m_allianceid, client, DEF_ALLIANCEHOST))
            {
                client->PlayerUpdate();
                city->ResourceUpdate();

                gserver.SendObject(client, gserver.CreateError("alliance.createAlliance", -99, "Alliance created but cannot set rank to host. Please contact support."));
                return;
            }

            client->PlayerUpdate();
            city->ResourceUpdate();

            gserver.SendObject(client, obj2);
            client->SaveToDB();
            alliance->SaveToDB();

            gserver.m_alliances->SortAlliances();

            return;
        }
        else
        {
            gserver.SendObject(client, gserver.CreateError("alliance.createAlliance", -99, "Cannot create alliance. Please contact support."));
            return;
        }
    }
    if ((command == "setAllInfoForAlliance"))
    {
        //TODO: permission check
        std::string notetext = data["noteText"];
        std::string infotext = data["infoText"];
        std::string alliancename = data["allianceName"];

        obj2["cmd"] = "alliance.setAllInfoForAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.setAllInfoForAlliance", -99, "Not a member of an alliance."));
            return;
        }
        Alliance * alliance = client->GetAlliance();
        if (alliance->m_name != alliancename)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.setAllInfoForAlliance", -99, "Error."));
            return;
        }

        alliance->m_note = Utils::makesafe(notetext);
        alliance->m_intro = Utils::makesafe(infotext);


        data2["ok"] = 1;

        gserver.SendObject(client, obj2);

        alliance->SaveToDB();

        return;
    }
    if (command == "getPowerFromAlliance")
    {
        obj2["cmd"] = "alliance.getPowerFromAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.getPowerFromAlliance", -99, "Not a member of an alliance."));
            return;
        }
        data2["ok"] = 1;
        data2["level"] = client->alliancerank;

        gserver.SendObject(client, obj2);
        return;
    }
    if (command == "resetTopPowerForAlliance")
    {
        obj2["cmd"] = "alliance.resetTopPowerForAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.resetTopPowerForAlliance", -99, "Not a member of an alliance."));
            return;
        }

        if (client->alliancerank != DEF_ALLIANCEHOST)
        {
            //you're not the host.. what are you doing?
            gserver.SendObject(client, gserver.CreateError("alliance.resetTopPowerForAlliance", -42, "You are not entitled to operate."));
            return;
        }

        std::string passtoname = data["userName"];

        Alliance * alliance = client->GetAlliance();
        if (alliance->HasMember(passtoname))
        {
            //member found
            Client * tclient = gserver.GetClientByName(passtoname);
            if (tclient->alliancerank != DEF_ALLIANCEVICEHOST)
            {
                gserver.SendObject(client, gserver.CreateError("alliance.resetTopPowerForAlliance", -88, "The Host title of the Alliance can only be transferred to Vice Host. You need to promote this player first."));
                return;
            }
            //everything checks out. target is vice host and you are host
            tclient->alliancerank = DEF_ALLIANCEHOST;
            client->alliancerank = DEF_ALLIANCEVICEHOST;
            client->PlayerUpdate();
            tclient->PlayerUpdate();
            alliance->m_ownerid = tclient->accountid;
            alliance->m_owner = tclient->playername;

            data2["ok"] = 1;

            gserver.SendObject(client, obj2);

            alliance->SaveToDB();
            client->SaveToDB();
            tclient->SaveToDB();

            return;
        }
        else
        {
            gserver.SendObject(client, gserver.CreateError("alliance.resetTopPowerForAlliance", -41, "Player " + passtoname + " doesn't exist."));
            return;
        }
    }
    if ((command == "agreeComeinAllianceByUser"))//TODO: alliance invites player, player accepts via embassy
    {
        VERIFYCASTLEID();
        CHECKCASTLEID();

        if (client->GetFocusCity()->GetBuildingLevel(B_EMBASSY) == 0)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByUser", -99, "You do not have an embassy."));
            return;
        }

        if (client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByUser", -99, "Already a member of an alliance."));
            return;
        }

        Alliance * alliance = gserver.m_alliances->AllianceByName(data["allianceName"]);
        if (!alliance)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByUser", -99, "Alliance no longer exists."));
            return;
        }

        for (Alliance::stInviteList & invite : alliance->m_invites)
        {
            if ((invite.client->playername == client->playername) && (invite.inviteperson.length() == 0))
            {
                //trying to accept self invite
                gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByUser", -99, "You cannot accept your own application."));
                return;
            }
        }

        alliance->UnRequestJoin(client->playername);
        gserver.m_alliances->JoinAlliance(alliance->m_allianceid, client);

        obj2["cmd"] = "alliance.agreeComeinAllianceByUser";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;
        
        gserver.SendObject(client, obj2);

        client->PlayerUpdate();

        alliance->SaveToDB();
        client->SaveToDB();

        return;
    }
    if ((command == "agreeComeinAllianceByLeader"))//player applies to alliance, leader accepts
    {
        obj2["cmd"] = "alliance.agreeComeinAllianceByLeader";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByLeader", -99, "Not a member of an alliance."));
            return;
        }

        if (client->alliancerank < 3)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByLeader", -99, "You do not have the proper permissions."));
            return;
        }

        std::string username = data["userName"];
        Client * invitee = gserver.GetClientByName(username);

        if (invitee == 0)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByLeader", -41, "Player " + username + " doesn't exist."));
            return;
        }
        if (invitee->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.agreeComeinAllianceByLeader", -99, username + " is already a member of other alliance."));
            return;
        }
        data2["ok"] = 1;

        Alliance * alliance = client->GetAlliance();

        alliance->UnRequestJoin(username);
        gserver.m_alliances->JoinAlliance(alliance->m_allianceid, invitee);

        gserver.SendObject(client, obj2);

        alliance->SaveToDB();
        invitee->SaveToDB();

        return;
    }
    if ((command == "setPowerForUserByAlliance"))
    {
        obj2["cmd"] = "alliance.setPowerForUserByAlliance";
        data2["packageId"] = 0.0;
        data2["ok"] = 1;

        int8_t type = data["typeId"];
        std::string username = data["userName"];

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.setPowerForUserByAlliance", -99, "Not a member of an alliance."));
            return;
        }

        Alliance * alliance = client->GetAlliance();

        Client * tar = gserver.GetClientByName(username);

        if (!tar || !alliance->HasMember(username))
        {
            gserver.SendObject(client, gserver.CreateError("alliance.setPowerForUserByAlliance", -99, "Member does not exist."));
            return;
        }
        //TODO: Set limits to rank counts? (aka, only x amount of vice hosts, etc)
        gserver.m_alliances->SetRank(client->allianceid, tar, type);

        gserver.SendObject(client, obj2);

        //send alliance message
        std::string msg = client->playername + " promotes " + tar->playername + " to " + AllianceMgr::GetAllianceRank(type) + ".";
        alliance->SendAllianceMessage(msg, false, false);

        alliance->SaveToDB();
        client->SaveToDB();

        return;
    }
    if ((command == "kickOutMemberfromAlliance"))
    {
        //TODO: permission check
        obj2["cmd"] = "alliance.kickOutMemberfromAlliance";
        data2["packageId"] = 0.0;

        if (!client->HasAlliance())
        {
            gserver.SendObject(client, gserver.CreateError("alliance.kickOutMemberfromAlliance", -99, "Not a member of an alliance."));
            return;
        }

        std::string username = data["userName"];

        Alliance * alliance = client->GetAlliance();

        Client * tar = gserver.GetClientByName(username);

        if (!tar || !alliance->HasMember(username))
        {
            gserver.SendObject(client, gserver.CreateError("alliance.kickOutMemberfromAlliance", -99, "Member does not exist."));
            return;
        }

        if (username == client->playername)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.kickOutMemberfromAlliance", -99, "Cannot kick yourself."));
            return;
        }

        if (client->alliancerank >= tar->alliancerank)
        {
            gserver.SendObject(client, gserver.CreateError("alliance.kickOutMemberfromAlliance", -99, "Cannot kick someone higher ranking than you."));
            return;
        }

        if (!gserver.m_alliances->RemoveFromAlliance(client->allianceid, tar))
        {
            gserver.SendObject(client, gserver.CreateError("alliance.kickOutMemberfromAlliance", -99, "Could not kick out member."));
            return;
        }

        gserver.SendObject(client, obj2);

        alliance->SaveToDB();
        tar->SaveToDB();

        return;
    }
}

