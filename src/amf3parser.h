/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <string>

#include "amf3reflist.h"

class amf3object;
class amf3array;
class amf3classdef;
class amf3objectmap;

class amf3parser
{
public:
    amf3parser(char * stream);
    ~amf3parser(void);

    amf3object ReadNextObject(void);

    int ReadInteger(void);
    double ReadNumber(void);
    std::string ReadString(void);
    std::string ReadDate(void);
    amf3array * ReadArray(void);
    amf3objectmap * ReadAMF3Object(void);

    amf3reflist<std::string> strlist;
    amf3reflist<std::string> internalstrlist;
    amf3reflist<amf3object> objectlist;
    amf3reflist<amf3object> encapslist;
    amf3reflist<amf3classdef> deflist;

    char * stream;
    int position;
};

