/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <string>
#include <vector>
#include "amf3object.h"
#include "amf3reflist.h"

class amf3array
{
public:
    amf3array(void);
    ~amf3array(void);

    void Add(amf3object & obj);
    void Add(amf3object && obj);
    void Add(std::string key, amf3object & obj);
    amf3object & Get(int id);
    amf3object & Get(std::string key);
    bool IsEqual(amf3array * obj);

    amf3reflist<amf3object> associative;
    std::vector<amf3object> dense;

    char type;
};
