/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "plogin.h"
#include "../spitfire.h"
#include "../Client.h"

#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/RecordSet.h>

using Poco::Data::RecordSet;
using namespace Poco::Data;
using namespace Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

plogin::plogin(spitfire & server, request & req, amf3object & obj)
	: packet(server, req, obj)
{

}

plogin::~plogin()
{

}

void plogin::process()
{
	obj2["data"] = amf3object();
	amf3object & data2 = obj2["data"];

	//errors:
	//-5 = captcha
	//-99 = general error
	//-100 = holiday
    std::string username = data["user"];
    std::string password = data["pwd"];

	if (gserver.maxplayers <= gserver.currentplayersonline + 1)
	{
		gserver.SendObject(req.conn, gserver.CreateError("server.LoginResponse", -99, "Servers are currently overloaded. Please try again later."));
		return;
	}

    std::string newuser;
    std::string newpass;
	newuser = Utils::makesafe(username);
	newpass = Utils::makesafe(password);


	{
		Session ses(gserver.accountpool->get());
		Statement select(ses);
		select << "SELECT COUNT(*) AS a FROM `account` WHERE `email`=?;", use(newuser);
		select.execute();
		RecordSet rs(select);

		uint64_t ttime = Utils::time();

		if (rs.value("a").convert<int32_t>() == 0)
		{
			//account does not exist - insert new row
			try
			{
				Statement insert(ses);
				insert << "INSERT INTO `account` (`name`, `email`, `ip`, `lastlogin`, `creation`, `password`, `status`, `reason`) VALUES ('null', ?, '', ?, ?, ?, 0, '');", use(newuser), use(ttime), use(ttime), use(newpass), now;
			}
			catch (MySQL::StatementException * e)
			{
				gserver.log->error("Account Create Exception: {}", e->displayText());
				return;
			}
		}
	}

	{
		Session ses(gserver.accountpool->get());
		Statement select(ses);
		select << "SELECT * FROM `account` WHERE `email`=? AND `password`=?;", use(newuser), use(newpass);
		select.execute();
		RecordSet rs(select);

		if (rs.rowCount() == 0)
		{
			//account doesn't exist or password is wrong
			gserver.SendObject(req.conn, gserver.CreateError("server.LoginResponse", -2, "Incorrect account or password."));
			return;
		}
		else
		{
			int32_t masteraccountid = rs.value("id").convert<int32_t>();
			client = gserver.GetClientByParent(masteraccountid);

			bool banned = false;

			{
				//are they banned? if so, globally or for this server?
				Session ses2(gserver.serverpool->get());
				Statement select2(ses2);
				select2 << "SELECT * FROM `accounts` WHERE `parentid`=?;", use(masteraccountid);
				select2.execute();
				RecordSet rs2(select2);

				if (rs.value("status").convert<int32_t>() == -99)
					banned = true;

				if (rs2.rowCount() > 0 && rs2.value("status").convert<int32_t>() == -99)
					banned = true;

				if (banned)
				{
                    std::string errormsg = "You are banned. Reason: ";
					errormsg += rs.value("reason").convert<std::string>().length() > 0 ? rs.value("reason").convert<std::string>() : rs2.value("reason").convert<std::string>();

					gserver.SendObject(req.conn, gserver.CreateError("server.LoginResponse", -99, errormsg));

					return;
				}
			}

			//client = gserver.GetClientByParent(parentid);
			if (client == nullptr)
			{
				client = gserver.NewClient();
				client->masteraccountid = masteraccountid;
				client->socknum = req.conn->uid;
				client->socket = req.conn;
				req.conn->client_ = client;
				client->connected = true;
			}
			else
			{
				if (client->connected)
				{
					//player already logged on
					gserver.CloseClient(client, 3, "");//multiple people logging into the same account
				}
				//Login is valid
				client->connected = true;
				double logintime = Utils::time();
				if (logintime - client->lastlogin < 1000 * 5)
				{
					gserver.SendObject(req.conn, gserver.CreateError("server.LoginResponse", 6, "You have tried logging in too frequently. Please try again later."));
					req.conn->stop();
					return;
				}
				client->lastlogin = logintime;
				if (client->socket) gserver.CloseClient(client, 3, "");
				req.conn->client_ = client;
				client->socket = req.conn;
				client->socknum = req.conn->uid;
				client->ipaddress = req.conn->address;
				gserver.log->info("Already established client found # {}", (uint32_t)client->internalid);

				if (client->email == "Daisy")
				{
					client->m_bdenyotherplayer = true;
					client->icon = 7;
				}
			}

			if (client == nullptr)
			{
				//error creating client object
				gserver.log->error("Error creating client object @ {}:{}", (std::string)__FILE__, (uint32_t)__LINE__);
				gserver.SendObject(client, gserver.CreateError("server.LoginResponse", -99, "Error with connecting. Please contact support."));
				return;
			}


			//account exists
			Session ses2(gserver.serverpool->get());
			Statement select2(ses2);
			select2 << "SELECT * FROM `accounts` WHERE `parentid`=?;", use(masteraccountid);
			select2.execute();
			RecordSet rs2(select2);

			if (rs2.rowCount() == 0)
			{
				//does not have an account on server
				gserver.SendObject(client, gserver.CreateError("server.LoginResponse", -4, "need create player"));
				client->loggedin = true;

				return;
			}
			else
			{
				int accountid = rs2.value("accountid").convert<int32_t>();
				client->accountid = accountid;

				//has an account, what about cities?
				Session ses3(gserver.serverpool->get());
				Statement select3(ses3);
				select3 << "SELECT * FROM `cities` WHERE `accountid`=?;", use(accountid);
				select3.execute();
				RecordSet rs3(select3);

				if (rs3.rowCount() == 0)
				{
					//does not have any cities on server but did have an account - this only happens if you try to "restart" your account. it saves the account info while deleting your cities
					gserver.SendObject(client, gserver.CreateError("server.LoginResponse", -4, "need create player"));
					client->loggedin = true;
					return;
				}
				else
				{
					//has an account and cities. process the list and send account info

					amf3object obj;
					obj["cmd"] = "server.LoginResponse";
					obj["data"] = amf3object();
					amf3object & data = obj["data"];
					data["packageId"] = 0.0;

					double tslag = Utils::time();

					if (client->GetItemCount("consume.1.a") < 10000)
						client->SetItem("consume.1.a", 10000);
					client->cents = 5000;

					data["player"] = client->ToObject();
					//UNLOCK(M_CLIENTLIST);

					if (client->citylist.size() == 0)
					{
						//problem
						gserver.log->error("Error client has no cities @ {}:{}", (std::string)__FILE__, (uint32_t)__LINE__);
						gserver.SendObject(client, gserver.CreateError("server.LoginResponse", -99, "Error with connecting. Please contact support."));
						return;
					}
					client->currentcityid = ((PlayerCity*)client->citylist.at(0))->m_castleid;
					client->currentcityindex = 0;
					client->accountexists = true;


					//check for holiday status
					stBuff * holiday = client->GetBuff("FurloughBuff");
					if (holiday && holiday->endtime > tslag)
					{
						//is in holiday - send holiday info too

                        std::string s;
						{
							int32_t hours;
							int32_t mins;
							int32_t secs = (holiday->endtime - tslag) / 1000;

							hours = secs / 60 / 60;
							mins = secs / 60 - hours * 60;
							secs = secs - mins * 60 - hours * 60 * 60;

							std::stringstream ss;
							ss << hours << "," << mins << "," << secs;

							s = ss.str();
						}

						data["ok"] = -100;
						data["msg"] = s;
						data["errorMsg"] = s;
					}
					else
					{
						data["ok"] = 1;
						data["msg"] = "success";
					}

					gserver.SendObject(client, obj);
					//SendObject(*req.connection, obj);

					client->clientdelay = Utils::time() - tslag;

					client->loggedin = true;

					gserver.currentplayersonline++;
					client->SaveToDB();

					uint32_t tc = 0;
					for (Client * client : gserver.players)
					{
						if (client->socket)
						{
							tc++;
						}
					}

					gserver.log->info("Players online: {}", tc);

					return;
				}
			}
		}
	}
	return;
}
