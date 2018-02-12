/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <string>

#include "amf3classdef.h"
#include "amf3reflist.h"

class amf3object;

class amf3objectmap
{
public:
    amf3objectmap(amf3classdef & classdef);
    amf3objectmap(amf3objectmap & objectmap);
    amf3objectmap();
    ~amf3objectmap(void);

    amf3object & Get(std::string key);
    void Add(std::string key, amf3object & obj);
    void Add(std::string key);
    int Exists(std::string key);

    bool IsEqual(amf3objectmap * obj);

    bool anoncd;
    amf3classdef classdef;
    amf3reflist<amf3object> properties;
    int flags;
    bool selfdel;
};
