/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pfurlough.h"
#include "../spitfire.h"
#include "../Client.h"

pfurlough::pfurlough(spitfire & server, request & req, amf3object & obj)
	: packet(server, req, obj)
{

}

pfurlough::~pfurlough()
{

}

void pfurlough::process()
{
	obj2["data"] = amf3object();
	amf3object & data2 = obj2["data"];

	if ((command == "isFurlought"))
	{
		int32_t playerid = data["playerId"];
		std::string password = data["password"];
		bool autofurlough = data["isAutoFurlough"];
		int32_t day = data["day"];

		if (client->password != password)
		{
			gserver.SendObject(client, gserver.CreateError("furlough.isFurlought", -50, "Incorrect account or password."));
			return;
		}

		if (client->cents < day * 10)
		{
			gserver.SendObject(client, gserver.CreateError("furlough.isFurlought", -99, "Not enough cents."));
			return;
		}
		client->cents -= day * 10;

		client->SetBuff("FurloughBuff", "", timestamp + (day * 24 * 60 * 60 * 1000));

		obj2["cmd"] = "furlough.isFurlought";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;
		data2["playerBean"] = client->ToObject();

		gserver.SendObject(client, obj2);
		return;
	}
	if ((command == "cancelFurlought"))
	{
		client->SetBuff("FurloughBuff", "", 0);

		obj2["cmd"] = "furlough.cancelFurlought";
		data2["packageId"] = 0.0;
		data2["ok"] = 1;

		gserver.SendObject(client, obj2);

		client->SaveToDB();

		return;
	}
}

