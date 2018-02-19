/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "packet.h"
#include "../spitfire.h"
#include "../Client.h"
#include "../City.h"
#include "../connection.h"

packet::packet(spitfire & server, request & req, amf3object & obj)
    : req(req),
    gserver(server),
    obj(obj),
    client(req.conn!=0?req.conn->client_:0),
    data(obj["data"]),
    obj2(amf3object())
{
    std::string cmd = obj["cmd"];
    std::vector<std::string> tokens;
    my_split(tokens, cmd, ".");
    //boost::split(tokens, cmd, boost::is_any_of("."));

    if (tokens.size() == 2)
    {
        cmdtype = tokens[0];
        command = tokens[1];
    }
    else if (tokens.size() == 1)
    {
        cmdtype = tokens[0];
    }

    timestamp = Utils::time();

    obj2["cmd"] = "";

    city = 0;
    if (client && client->currentcityindex != -1)
        city = client->citylist[client->currentcityindex];
}

packet::~packet()
{

}


void packet::CHECKCASTLEID() const
{
    if (IsObject(data) && KeyExists(data, "castleId") && (int)data["castleId"] != client->currentcityid)
    {
        gserver.log->error("castleId does not match castle focus! gave: {} is:{} - cmd: {}.{} - accountid:{} - playername: {}", (int)data["castleId"], client->currentcityid, cmdtype, command, client->accountid, (char*)client->playername.c_str());
        throw(0);
    }
}

void packet::VERIFYCASTLEID() const
{
    if (!IsObject(data) || !KeyExists(data, "castleId"))
    {
        gserver.log->error("castleId not received! - cmd: {}.{} - accountid:{} - playername: {}", cmdtype, command, client->accountid, (char*)client->playername.c_str());
        throw(1);
    }
}

void packet::check() const
{
    if (client == nullptr)
        throw(2);
}
