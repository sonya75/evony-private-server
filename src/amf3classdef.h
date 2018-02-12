/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <stdint.h>
#include <string>
#include <vector>

class amf3classdef
{
public:
    amf3classdef(void);
    amf3classdef(std::string name, std::vector<std::string> & properties, bool dynamic, bool externalizable);
    amf3classdef(const amf3classdef & classdef);
    ~amf3classdef(void);

    std::string name;
    bool dynamic;
    bool externalizable;
    std::vector<std::string> properties;
    bool IsEqual(amf3classdef & obj) const;
};

