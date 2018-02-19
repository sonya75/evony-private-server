/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "punknown.h"
#include "../spitfire.h"

punknown::punknown(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

punknown::~punknown()
{

}

void punknown::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    obj2["cmd"] = "server.SystemInfoMsg";
    data2["alliance"] = false;
    data2["tV"] = false;
    data2["noSenderSystemInfo"] = false;
    data2["msg"] = std::string("Unknown object - " + (std::string)obj["cmd"]);
    gserver.SendObject(client, obj2);
    gserver.log->error("Unknown object - {}", (std::string)obj["cmd"]);
    return;
}

