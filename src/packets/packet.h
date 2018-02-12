/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "../structs.h"
#include "../amf3.h"
#include "../PlayerCity.h"

class spitfire;
class Client;
struct request;

class packet
{
public:
    packet(spitfire & server, request & req, amf3object & o);
    virtual ~packet();
    spitfire & gserver;
    Client * client;
    amf3object & obj;
    request & req;
    std::string cmdtype;
    std::string command;
    amf3object & data;
    amf3object obj2;
    uint64_t timestamp;
    PlayerCity * city;
    void CHECKCASTLEID() const;
    void VERIFYCASTLEID() const;
    void check() const;

    virtual void process() = 0;
};
