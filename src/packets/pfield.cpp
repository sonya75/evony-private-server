/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pfield.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../Map.h"
#include "../Tile.h"

pfield::pfield(spitfire & server, request & req, amf3object & obj)
	: packet(server, req, obj)
{

}

pfield::~pfield()
{

}

void pfield::process()
{
	obj2["data"] = amf3object();
	amf3object & data2 = obj2["data"];

	if (command == "getOtherFieldInfo")
	{
		int fieldid = data["fieldId"];

		obj2["cmd"] = "field.getOtherFieldInfo";

		amf3object bean;

		data2["bean"] = gserver.map->GetMapCastle(fieldid, req.conn->client_->accountid);

		//data2["errorMsg"] = "";
		data2["ok"] = 1;
		data2["packageId"] = 0.0;

		gserver.SendObject(client, obj2);
		return;
	}
}

