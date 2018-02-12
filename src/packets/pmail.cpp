
#include "pmail.h"
#include "../spitfire.h"
#include "../Client.h"

#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/RecordSet.h>

using Poco::Data::RecordSet;
using namespace Poco::Data;
using namespace Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

pmail::pmail(spitfire & server, request & req, amf3object & obj)
	: packet(server, req, obj)
{

}

pmail::~pmail()
{

}

void pmail::process()
{
	obj2["data"] = amf3object();
	amf3object & data2 = obj2["data"];

	if (command == "receiveMailList")
	{
		int pagesize = data["pageSize"];
		int type = data["type"];
		int pageno = data["pageNo"];

		if (pageno == 0)
			pageno++;


		std::list<stMail> * maillist = &client->maillist;

		int32_t count = 0;
		for (stMail mail : *maillist)
		{
			if (mail.type_id == type)
				count++;
		}

		obj2["cmd"] = "mail.receiveMailList";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;
		amf3array mails = amf3array();
		
		std::list<stMail>::reverse_iterator iter;
		if (pagesize <= 0 || pagesize > 20 || pageno < 0 || pageno > 100000)
		{
			gserver.SendObject(client, gserver.CreateError("mail.receiveMailList", -99, "Invalid data."));
			return;
		}

		if ((pageno - 1)*pagesize > maillist->size())
		{
			gserver.SendObject(client, gserver.CreateError("mail.receiveMailList", -99, "Invalid page."));
			return;
		}
		iter = maillist->rbegin();
		for (int i = 0; i < (pageno - 1)*pagesize; ++i)
		{
			while (iter != maillist->rend())
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
		for (; iter != maillist->rend() && pagesize != 0; ++iter)
		{
			if (iter->type_id == type)
			{
				pagesize--;
				amf3object temp = amf3object();
				if (iter->playerid == 0)
				{
					temp["sender"] = "System";
					temp["playerId"] = client->accountid;
				}
				else
				{
					Client * sender = gserver.GetClient(iter->playerid);
					temp["sender"] = sender->playername;
					temp["playerId"] = sender->accountid;
				}
				temp["mailid"] = iter->mailid;
				temp["selected"] = false;
				temp["title"] = iter->title;
				temp["receiver"] = client->playername;
				temp["receiveTime"] = iter->senttime;
				temp["targetId"] = client->accountid;
				temp["type_id"] = iter->type_id;
				temp["isRead"] = iter->isread();
				mails.Add(temp);
			}
		}
		
		data2["mails"] = mails;
		gserver.SendObject(client, obj2);
		return;
	}
	/*if (command == "receiveMailList")
	{
		int32_t pageno = obj["pageNo"];
		int32_t pagesize = obj["pageSize"];
		int32_t type = obj["type"];

		//type 1 = normal inbox
		//type 2 = system
		//type 3 = sent

		amf3object obj3;
		obj3["cmd"] = "mail.receiveMailList";
		obj3["data"] = amf3object();
		amf3object & data3 = obj3["data"];

		amf3array mails;
		amf3object mail;
		mail["sender"] = "Daisy1";
		mail["mailid"] = 1;
		mail["selected"] = false;
		mail["title"] = "title";
		mail["receiver"] = "Daisy";
		mail["playerId"] = client->m_playerid;
		mail["receiveTime"] = unixtime();
		mail["targetId"] = client->m_playerid;
		mail["type_id"] = 1;//??
		mail["isRead"] = 0;//0 = unread, 1 = read?
		mails.Add(mail);

		data3["reports"] = mails;
		data3["pageNo"] = pageno;
		data3["packageId"] = 0.0;
		data3["ok"] = 1;
		data3["totalPage"] = 0;
		gserver.SendObject(client, obj3);
		return;
	}*/
	if (command == "readMail")
	{
		uint32_t mailid = data["mailId"];

		for (stMail & mail : client->maillist)
		{
			if (mail.mailid == mailid)
			{
				if (mail.playerid != 0)
				{
					Client * sender = nullptr;
					sender = gserver.GetClient(mail.playerid);
					data2["sender"] = sender->playername;
					data2["targetId"] = sender->accountid;//player who sent the mail - if System, set to reader's id

				}
				else
				{
					data2["sender"] = "System";
					data2["targetId"] = client->accountid;
				}
				mail.readtime = Utils::time();
				data2["content"] = mail.content;
				data2["mailid"] = mailid;
				data2["title"] = mail.title;
				data2["receiver"] = client->playername;
				data2["receiveTime"] = mail.senttime;
				data2["playerId"] = client->accountid;//player reading the mail
				data2["ok"] = 1;
				data2["type_id"] = mail.type_id;
				data2["isRead"] = mail.isread();

				obj2["cmd"] = "mail.readMail";

				client->MailUpdate();
				gserver.SendObject(client, obj2);

				try
				{
					Session ses(gserver.serverpool->get());
					Statement stmt = (ses << "UPDATE `mail` SET `readtime`=? WHERE `pid`=? AND `receiverid`=? LIMIT 1;",
									  use(mail.readtime), use(mailid), use(client->accountid));
					stmt.execute();
					if (!stmt.done())
					{
						gserver.log->error("Unable to update read mail.");
						return;
					}
				}
				SQLCATCH3(return;, gserver);
				return;
			}
		}
	}
	if (command == "sendMail")
	{
        std::string username = data["username"];
        std::string title = data["title"];
        std::string content = data["content"];

/*
		if (client->m_playername == username)
		{
			gserver.SendObject(client, gserver.CreateError("mail.sendMail", -21, "invalid mailing operation."));
			return;
		}*/

		Client * tclient = gserver.GetClientByName(username);
		if (!tclient)
		{
			gserver.SendObject(client, gserver.CreateError("mail.sendMail", -41, "Player " + username + " doesn't exist."));
			return;
		}

		if (!gserver.CreateMail(client->playername, username, title, content, 1))
		{
			gserver.SendObject(client, gserver.CreateError("mail.sendMail", -42, "Error sending mail"));
			return;
		}

		obj2["cmd"] = "mail.sendMail";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;
		gserver.SendObject(client, obj2);

		client->MailUpdate();
		tclient->MailUpdate();
		return;
	}
	if (command == "reportBug")
	{
        std::string subject = data["subject"];
        std::string content = data["content"];
		int64_t accountid = client->accountid;
        std::string sender = client->playername;

		obj2["cmd"] = "mail.reportBug";
		data2["errorMsg"] = "success";
        data2["packageId"] = 0.0;
		data2["ok"] = 1;
		gserver.SendObject(client, obj2);
	}
}
