/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "request_handler.h"
#include <string>
#include <string.h>

#include "packets/pcommon.h"
#include "packets/pcity.h"
#include "packets/preport.h"
#include "packets/pserver.h"
#include "packets/pcastle.h"
#include "packets/pfield.h"
#include "packets/pquest.h"
#include "packets/palliance.h"
#include "packets/ptech.h"
#include "packets/plogin.h"
#include "packets/prank.h"
#include "packets/ptrade.h"
#include "packets/pshop.h"
#include "packets/pmail.h"
#include "packets/pfortifications.h"
#include "packets/ptroop.h"
#include "packets/pinterior.h"
#include "packets/phero.h"
#include "packets/pfriend.h"
#include "packets/pcity.h"
#include "packets/pfurlough.h"
#include "packets/parmy.h"
#include "packets/pgameclient.h"
#include "packets/punknown.h"

#include <spdlog/fmt/bundled/ostream.h>
//#include "../lib/fmt/fmt/ostream.h"
#include <string.h>

#include "Client.h"
#include "spitfire.h"
#include "Alliance.h"
#include "AllianceMgr.h"
#include "Map.h"
#include "City.h"
#include "Hero.h"
#include "Tile.h"
#include "spitfire.h"
#include <Poco/Data/MySQL/MySQLException.h>


request_handler::request_handler()
{
}

void request_handler::handle_request(spitfire & server, request& req) const
{
    amf3object & obj = req.object;
    std::string cmd = obj["cmd"];

    amf3object obj2 = amf3object();
    obj2["cmd"] = "";
    amf3object & data2 = obj2["data"];
    data2 = amf3object();

    server.log->trace("packet: size: {0:5d} - Command: {1}", req.size, cmd);

    std::vector<std::string> tokens;
    my_split(tokens, cmd, ".");
    //boost::split(tokens, cmd, boost::is_any_of("."));

    std::string cmdtype, command;

    cmdtype = tokens[0];
    if (tokens.size() > 1)
        command = tokens[1];

    connection & c = *req.conn;
    Client * client = c.client_;

    if (cmdtype != "login")
    {
        if (cmdtype == "" || command == "")
        {
            if (c.client_)
                server.log->error("0 length command sent clientnumber: {}", c.client_->internalid);
            else
                server.log->error("0 length command sent from nonexistent client");
            return;
        }
    }

    if (client != nullptr && client->debugmode)
    {
        server.SendMessage(client, cmd);
    }

    try
    {
        if (cmdtype == "common")
        {
            pcommon pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "report")
        {
            preport pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "server")
        {
            pserver pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "castle")
        {
            pcastle pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "field")
        {
            pfield pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "quest")
        {
            pquest pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "alliance")
        {
            palliance pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "tech")
        {
            ptech pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "login")
        {
            plogin pkt(server, req, obj);
            pkt.process();
        }
        else if (cmdtype == "rank")
        {
            prank pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "trade")
        {
            ptrade pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "shop")
        {
            pshop pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "mail")
        {
            pmail pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "fortifications")
        {
            pfortifications pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "troop")
        {
            ptroop pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "interior")
        {
            pinterior pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "hero")
        {
            phero pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "friend")
        {
            pfriend pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "city")
        {
            pcity pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "furlough")
        {
            pfurlough pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "army")
        {
            parmy pkt(server, req, obj);
            pkt.check();
            pkt.process();
        }
        else if (cmdtype == "gameClient")
        {
            pgameclient pkt(server, req, obj);
            pkt.process();
        }
        else
        {
            punknown pkt(server, req, obj);
            pkt.process();
        }
    }
    catch (Poco::Data::MySQL::ConnectionException& e)\
    {
        std::string file = __FILE__;
        server.log->error("ConnectionException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::Data::MySQL::StatementException& e)
    {
        std::string file = __FILE__;
        server.log->error("StatementException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::Data::MySQL::MySQLException& e)
    {
        std::string file = __FILE__;
        server.log->error("MySQLException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::InvalidArgumentException& e)
    {
        std::string file = __FILE__;
        server.log->error("InvalidArgumentException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (Poco::Data::ConnectionFailedException& e)
    {
        std::string file = __FILE__;
        server.log->error("ConnectionFailedException: {} {} {}", file, __LINE__, e.displayText());
        return;
    }
    catch (std::exception & e)
    {
        std::cerr << "uncaught handle_request() exception: " << e.what();
    }
    catch (int er)
    {
        if (er == 0)
        {
            //CHECKCASTLEID() failure (castle id not matching)
        }
        else if (er == 1)
        {
            //VERIFYCASTLEID() failure (castle id not provided)
        }
    }
}

/*
Possible messages:

respMap["alliance.getAllianceArmyReport"] = _resp_alliance_getAllianceArmyReport;
respMap["alliance.getAllianceEventList"] = _resp_alliance_getAllianceEventList;
respMap["alliance.getAllianceInfo"] = _resp_alliance_getAllianceInfo;
respMap["alliance.getAllianceList"] = _resp_alliance_getAllianceList;
respMap["alliance.getAllianceMembers"] = _resp_alliance_getAllianceMembers;
respMap["alliance.getMilitarySituationList"] = _resp_alliance_getMilitarySituationList;
respMap["alliance.addUsertoAlliance"] = _resp_alliance_addUsertoAlliance;
respMap["alliance.addUsertoAllianceList"] = _resp_alliance_addUsertoAllianceList;
respMap["alliance.agreeComeinAllianceByLeader"] = _resp_alliance_agreeComeinAllianceByLeader;
respMap["alliance.agreeComeinAllianceByUser"] = _resp_alliance_agreeComeinAllianceByUser;
respMap["alliance.agreeComeinAllianceList"] = _resp_alliance_agreeComeinAllianceList;
respMap["alliance.cancelUserWantInAlliance"] = _resp_alliance_cancelUserWantInAlliance;
respMap["alliance.canceladdUsertoAlliance"] = _resp_alliance_canceladdUsertoAlliance;
respMap["alliance.cancelagreeComeinAlliance"] = _resp_alliance_cancelagreeComeinAlliance;
respMap["alliance.createAlliance"] = _resp_alliance_createAlliance;
respMap["alliance.dropAllianceFriendshipRelation"] = _resp_alliance_dropAllianceFriendshipRelation;
respMap["alliance.getAllianceFriendshipList"] = _resp_alliance_getAllianceFriendshipList;
respMap["alliance.getAllianceWanted"] = _resp_alliance_getAllianceWanted;
respMap["alliance.getPowerFromAlliance"] = _resp_alliance_getPowerFromAlliance;
respMap["alliance.isHasAlliance"] = _resp_alliance_isHasAlliance;
respMap["alliance.kickOutMemberfromAlliance"] = _resp_alliance_kickOutMemberfromAlliance;
respMap["alliance.leaderWantUserInAllianceList"] = _resp_alliance_leaderWantUserInAllianceList;
respMap["alliance.messagesForAllianceMember"] = _resp_alliance_messagesForAllianceMember;
respMap["alliance.rejectComeinAlliance"] = _resp_alliance_rejectComeinAlliance;
respMap["alliance.resetTopPowerForAlliance"] = _resp_alliance_resetTopPowerForAlliance;
respMap["alliance.resignForAlliance"] = _resp_alliance_resignForAlliance;
respMap["alliance.sayByetoAlliance"] = _resp_alliance_sayByetoAlliance;
respMap["alliance.setAllInfoForAlliance"] = _resp_alliance_setAllInfoForAlliance;
respMap["alliance.setAllianceFriendship"] = _resp_alliance_setAllianceFriendship;
respMap["alliance.setPowerForUserByAlliance"] = _resp_alliance_setPowerForUserByAlliance;
respMap["alliance.userWantInAlliance"] = _resp_alliance_userWantInAlliance;
respMap["army.callBackArmy"] = _resp_army_callBackArmy;
respMap["army.cureInjuredTroop"] = _resp_army_cureInjuredTroop;
respMap["army.disbandInjuredTroop"] = _resp_army_disbandInjuredTroop;
respMap["army.exerciseArmy"] = _resp_army_exerciseArmy;
respMap["army.getInjuredTroop"] = _resp_army_getInjuredTroop;
respMap["army.getStayAllianceArmys"] = _resp_army_getStayAllianceArmys;
respMap["army.getTroopParam"] = _resp_army_getTroopParam;
respMap["army.newArmy"] = _resp_army_newArmy;
respMap["army.setAllowAllianceArmy"] = _resp_army_setAllowAllianceArmy;
respMap["army.setArmyGoOut"] = _resp_army_setArmyGoOut;
respMap["castle.cancelBuildingQueue"] = _resp_castle_cancelBuildingQueue;
respMap["castle.cancleBuildCommand"] = _resp_castle_cancleBuildCommand;
respMap["castle.checkOutUpgrade"] = _resp_castle_checkOutUpgrade;
respMap["castle.demolishBuildingQueue"] = _resp_castle_demolishBuildingQueue;
respMap["castle.destructBuilding"] = _resp_castle_destructBuilding;
respMap["castle.getAvailableBuildingBean"] = _resp_castle_getAvailableBuildingBean;
respMap["castle.getAvailableBuildingListInside"] = _resp_castle_getAvailableBuildingListInside;
respMap["castle.getAvailableBuildingListOutside"] = _resp_castle_getAvailableBuildingListOutside;
respMap["castle.getBuildingQueueCoinsNeed"] = _resp_castle_getBuildingQueueCoinsNeed;
respMap["castle.getCoinsNeed"] = _resp_castle_getCoinsNeed;
respMap["castle.getDestructBuildBean"] = _resp_castle_getDestructBuildBean;
respMap["castle.newBuilding"] = _resp_castle_newBuilding;
respMap["castle.newBuildingQueue"] = _resp_castle_newBuildingQueue;
respMap["castle.speedUpBuildCommand"] = _resp_castle_speedUpBuildCommand;
respMap["castle.speedupBuildingQueue"] = _resp_castle_speedupBuildingQueue;
respMap["castle.upgradeBuilding"] = _resp_castle_upgradeBuilding;
respMap["castle.deleteCastleSign"] = _resp_castle_deleteCastleSign;
respMap["castle.saveCastleSignList"] = _resp_castle_saveCastleSignList;
respMap["city.advMoveCastle"] = _resp_city_advMoveCastle;
respMap["city.constructCastle"] = _resp_city_constructCastle;
respMap["city.getStoreList"] = _resp_city_getStoreList;
respMap["city.giveupCastle"] = _resp_city_giveupCastle;
respMap["city.modifyCastleName"] = _resp_city_modifyCastleName;
respMap["city.modifyFlag"] = _resp_city_modifyFlag;
respMap["city.modifyStorePercent"] = _resp_city_modifyStorePercent;
respMap["city.modifyUserName"] = _resp_city_modifyUserName;
respMap["city.moveCastle"] = _resp_city_moveCastle;
respMap["city.setStopWarState"] = _resp_city_setStopWarState;
respMap["city.uniteAdvMoveCastle"] = _resp_city_uniteAdvMoveCastle;
respMap["common.CbUD"] = _resp_common_CbUD;
respMap["common.allianceChat"] = _resp_common_allianceChat;
respMap["common.cancelOnlineBonus"] = _resp_common_cancelOnlineBonus;
respMap["common.changeName"] = _resp_common_changeName;
respMap["common.changeUserFace"] = _resp_common_changeUserFace;
respMap["common.channelChat"] = _resp_common_channelChat;
respMap["common.createNewPlayer"] = _resp_common_createNewPlayer;
respMap["common.delUniteServerPeaceStatus"] = _resp_common_delUniteServerPeaceStatus;
respMap["common.deleteUserAndRestart"] = _resp_common_deleteUserAndRestart;
respMap["common.denyPlayerSpeak"] = _resp_common_denyPlayerSpeak;
respMap["common.getItemDefXml"] = _resp_common_getItemDefXml;
respMap["common.getOnlineBonus"] = _resp_common_getOnlineBonus;
respMap["common.getPackage"] = _resp_common_getPackage;
respMap["common.getPackageList"] = _resp_common_getPackageList;
respMap["common.getPackageNumber"] = _resp_common_getPackageNumber;
respMap["common.getPlayerInfoByName"] = _resp_common_getPlayerInfoByName;
respMap["common.mapCastle"] = _resp_common_mapCastle;
respMap["common.mapInfo"] = _resp_common_mapInfo;
respMap["common.mapInfoSimple"] = _resp_common_mapInfoSimple;
respMap["common.privateChat"] = _resp_common_privateChat;
respMap["common.refreshCaptcha"] = _resp_common_refreshCaptcha;
respMap["common.step"] = _resp_common_step;
respMap["common.worldChat"] = _resp_common_worldChat;
respMap["common.zoneInfo"] = _resp_common_zoneInfo;
respMap["common.authSecurityCode"] = _resp_common_authSecurityCode;
respMap["common.cancelRemovingSecurityCodeProcess"] = _resp_common_cancelRemovingSecurityCodeProcess;
respMap["common.changeSecurityCode"] = _resp_common_changeSecurityCode;
respMap["common.getIsSecurityCodeSetted"] = _resp_common_getIsSecurityCodeSetted;
respMap["common.getProtectOption"] = _resp_common_getProtectOption;
respMap["common.removeSecurityCode"] = _resp_common_removeSecurityCode;
respMap["common.setProtectOption"] = _resp_common_setProtectOption;
respMap["common.setSecurityCode"] = _resp_common_setSecurityCode;
respMap["common.setUnlockOption"] = _resp_common_setUnlockOption;
respMap["field.getCastleFieldInfo"] = _resp_field_getCastleFieldInfo;
respMap["field.getOtherFieldInfo"] = _resp_field_getOtherFieldInfo;
respMap["field.giveUpField"] = _resp_field_giveUpField;
respMap["fortifications.accTroopProduce"] = _resp_fortifications_accTroopProduce;
respMap["fortifications.cancelFortificationProduce"] = _resp_fortifications_cancelFortificationProduce;
respMap["fortifications.destructWallProtect"] = _resp_fortifications_destructWallProtect;
respMap["fortifications.getFortificationsProduceList"] = _resp_fortifications_getFortificationsProduceList;
respMap["fortifications.getProduceQueue"] = _resp_fortifications_getProduceQueue;
respMap["fortifications.produceWallProtect"] = _resp_fortifications_produceWallProtect;
respMap["friend.addBlock"] = _resp_friend_addBlock;
respMap["friend.addFriend"] = _resp_friend_addFriend;
respMap["friend.deleteBlock"] = _resp_friend_deleteBlock;
respMap["friend.deleteFriend"] = _resp_friend_deleteFriend;
respMap["friend.isBlockMailPlayer"] = _resp_friend_isBlockMailPlayer;
respMap["furlough.cancelFurlought"] = _resp_furlough_cancelFurlought;
respMap["furlough.isFurlought"] = _resp_furlough_isFurlought;
respMap["gamemaster.addBuilding"] = _resp_gamemaster_addBuilding;
respMap["gamemaster.addHero"] = _resp_gamemaster_addHero;
respMap["gamemaster.addItems"] = _resp_gamemaster_addItems;
respMap["gamemaster.removeBuilding"] = _resp_gamemaster_removeBuilding;
respMap["gamemaster.removeHero"] = _resp_gamemaster_removeHero;
respMap["gamemaster.removeItem"] = _resp_gamemaster_removeItem;
respMap["gamemaster.setResources"] = _resp_gamemaster_setResources;
respMap["gamemaster.setTechnology"] = _resp_gamemaster_setTechnology;
respMap["hero.addPoint"] = _resp_hero_addPoint;
respMap["hero.awardGold"] = _resp_hero_awardGold;
respMap["hero.callBackHero"] = _resp_hero_callBackHero;
respMap["hero.changeName"] = _resp_hero_changeName;
respMap["hero.dischargeChief"] = _resp_hero_dischargeChief;
respMap["hero.fireHero"] = _resp_hero_fireHero;
respMap["hero.getHerosListFromTavern"] = _resp_hero_getHerosListFromTavern;
respMap["hero.hireHero"] = _resp_hero_hireHero;
respMap["hero.levelUp"] = _resp_hero_levelUp;
respMap["hero.promoteToChief"] = _resp_hero_promoteToChief;
respMap["hero.refreshHerosListFromTavern"] = _resp_hero_refreshHerosListFromTavern;
respMap["hero.releaseHero"] = _resp_hero_releaseHero;
respMap["hero.resetPoint"] = _resp_hero_resetPoint;
respMap["hero.tryGetSeizedHero"] = _resp_hero_tryGetSeizedHero;
respMap["hero.useItem"] = _resp_hero_useItem;
respMap["interior.getResourceProduceData"] = _resp_interior_getResourceProduceData;
respMap["interior.modifyCommenceRate"] = _resp_interior_modifyCommenceRate;
respMap["interior.modifyTaxRate"] = _resp_interior_modifyTaxRate;
respMap["interior.pacifyPeople"] = _resp_interior_pacifyPeople;
respMap["interior.taxation"] = _resp_interior_taxation;
respMap["mail.deleteMail"] = _resp_mail_deleteMail;
respMap["mail.getAllTVMsg"] = _resp_mail_getAllTVMsg;
respMap["mail.readMail"] = _resp_mail_readMail;
respMap["mail.readOverMailList"] = _resp_mail_readOverMailList;
respMap["mail.receiveMailList"] = _resp_mail_receiveMailList;
respMap["mail.reportBug"] = _resp_mail_reportBug;
respMap["mail.reportPlayer"] = _resp_mail_reportPlayer;
respMap["mail.sendMail"] = _resp_mail_sendMail;
respMap["quest.award"] = _resp_quest_award;
respMap["quest.awardPacket"] = _resp_quest_awardPacket;
respMap["quest.getAwardItems"] = _resp_quest_getAwardItems;
respMap["quest.getQuestList"] = _resp_quest_getQuestList;
respMap["quest.getQuestType"] = _resp_quest_getQuestType;
respMap["rank.getAllianceRank"] = _resp_rank_getAllianceRank;
respMap["rank.getCastleRank"] = _resp_rank_getCastleRank;
respMap["rank.getHeroRank"] = _resp_rank_getHeroRank;
respMap["rank.getPlayerRank"] = _resp_rank_getPlayerRank;
respMap["report.deleteReport"] = _resp_report_deleteReport;
respMap["report.markAsRead"] = _resp_report_markAsRead;
respMap["report.readOverReport"] = _resp_report_readOverReport;
respMap["report.receiveReportList"] = _resp_report_receiveReportList;
respMap["shop.buy"] = _resp_shop_buy;
respMap["shop.buyResource"] = _resp_shop_buyResource;
respMap["shop.getBuyResourceInfo"] = _resp_shop_getBuyResourceInfo;
respMap["shop.useCastleGoods"] = _resp_shop_useCastleGoods;
respMap["shop.useGoods"] = _resp_shop_useGoods;
respMap["stratagem.useStratagem"] = _resp_stratagem_useStratagem;
respMap["tech.cancelResearch"] = _resp_tech_cancelResearch;
respMap["tech.getCoinsNeed"] = _resp_tech_getCoinsNeed;
respMap["tech.getResearchList"] = _resp_tech_getResearchList;
respMap["tech.research"] = _resp_tech_research;
respMap["tech.speedUpResearch"] = _resp_tech_speedUpResearch;
respMap["trade.cancelTrade"] = _resp_trade_cancelTrade;
respMap["trade.getMyTradeList"] = _resp_trade_getMyTradeList;
respMap["trade.getTransingTradeList"] = _resp_trade_getTransingTradeList;
respMap["trade.newTrade"] = _resp_trade_newTrade;
respMap["trade.searchTrades"] = _resp_trade_searchTrades;
respMap["trade.speedUpTrans"] = _resp_trade_speedUpTrans;
respMap["troop.accTroopProduce"] = _resp_troop_accTroopProduce;
respMap["troop.cancelTroopProduce"] = _resp_troop_cancelTroopProduce;
respMap["troop.checkIdleBarrack"] = _resp_troop_checkIdleBarrack;
respMap["troop.disbandTroop"] = _resp_troop_disbandTroop;
respMap["troop.getProduceQueue"] = _resp_troop_getProduceQueue;
respMap["troop.getTroopProduceList"] = _resp_troop_getTroopProduceList;
respMap["troop.produceTroop"] = _resp_troop_produceTroop;
respMap["truce.cancelDreamTruce"] = _resp_truce_cancelDreamTruce;
respMap["truce.changeDreamTruceTime"] = _resp_truce_changeDreamTruceTime;
respMap["truce.setDreamTruce"] = _resp_truce_setDreamTruce;
respMap["server.AllianceChatMsg"] = _resp_AllianceChatMsg;
respMap["server.BuildComplate"] = _resp_BuildComplate;
respMap["server.BuildingQueueUpdate"] = _resp_BuildingQueueUpdate;
respMap["server.CastleFieldUpdate"] = _resp_CastleFieldUpdate;
respMap["server.CastleUpdate"] = _resp_CastleUpdate;
respMap["server.CbUpdate"] = _resp_CbUpdate;
respMap["server.ChangeName"] = _resp_ChangeName;
respMap["server.ChannelChatMsg"] = _resp_ChannelChatMsg;
respMap["server.ConnectionLost"] = _resp_ConnectionLost;
respMap["server.EnemyArmysUpdate"] = _resp_EnemyArmysUpdate;
respMap["server.FortificationsUpdate"] = _resp_FortificationsUpdate;
respMap["server.FriendArmysUpdate"] = _resp_FriendArmysUpdate;
respMap["server.HeroUpdate"] = _resp_HeroUpdate;
respMap["server.InjuredTroopUpdate"] = _resp_InjuredTroopUpdate;
respMap["server.ItemBuff"] = _resp_ItemBuff;
respMap["server.ItemUpdate"] = _resp_ItemUpdate;
respMap["server.KickedOut"] = _resp_KickedOut;
respMap["server.LoginResponse"] = _resp_LoginResponse;
respMap["server.NewFinishedQuest"] = _resp_NewFinishedQuest;
respMap["server.NewMail"] = _resp_NewMail;
respMap["server.NewReport"] = _resp_NewReport;
respMap["server.OnlineBonus"] = _resp_OnlineBonus;
respMap["server.PackageList"] = _resp_PackageList;
respMap["server.PlayerBuffUpdate"] = _resp_PlayerBuffUpdate;
respMap["server.PlayerInfoUpdate"] = _resp_PlayerInfoUpdate;
respMap["server.PrivateChatMessage"] = _resp_PrivateChatMessage;
respMap["server.QuestFinished"] = _resp_QuestFinished;
respMap["server.RegisterResponse"] = _resp_RegisterResponse;
respMap["server.ResearchCompleteUpdate"] = _resp_ResearchCompleteUpdate;
respMap["server.ResourceUpdate"] = _resp_ResourceUpdate;
respMap["server.SecurityCodeRemovedEvent"] = _resp_SecurityCodeRemovedEvent;
respMap["server.SelfArmysUpdate"] = _resp_SelfArmysUpdate;
respMap["server.StrategyBuff"] = _resp_StrategyBuff;
respMap["server.SystemInfoMsg"] = _resp_SystemInfoMsg;
respMap["server.TradesUpdate"] = _resp_TradesUpdate;
respMap["server.TransingTradeUpdate"] = _resp_TransingTradeUpdate;
respMap["server.TroopUpdate"] = _resp_TroopUpdate;
respMap["server.WorldChatMsg"] = _resp_WorldChatMsg;
*/

