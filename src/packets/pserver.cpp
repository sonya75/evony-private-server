/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "pserver.h"

pserver::pserver(spitfire & server, request & req, amf3object & obj)
    : packet(server, req, obj)
{

}

pserver::~pserver()
{

}

void pserver::process()
{
    obj2["data"] = amf3object();
    amf3object & data2 = obj2["data"];


#pragma region server
    //     if ((cmdtype == "server"))
    //     {
    //         //shouldn't have anything
    //     }
#pragma endregion
}
