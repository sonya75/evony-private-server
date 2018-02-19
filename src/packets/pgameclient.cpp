/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pgameclient.h"
#include "../spitfire.h"

pgameclient::pgameclient(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pgameclient::~pgameclient()
{

}

void pgameclient::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];

    if (command == "version")
    {
        if (data == "091103_11")
        {
            //pass
            return;
        }
        else
        {
//             obj2["cmd"] = "gameClient.kickout";
//             obj2["data"] = amf3object();
//             data2["msg"] = "You suck.";
// 
//             gserver.SendObject(client, obj2);
//             //"other" version
//             return;

            obj2["cmd"] = "gameClient.errorVersion";
            data2["version"] = "091103_11";
            data2["msg"] = "Invalid Version.";

            gserver.SendObject(client, obj2);
            //"other" version
            return;
        }
    }
    return;
}
