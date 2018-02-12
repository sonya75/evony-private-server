/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "amf3objectmap.h"
#include "amf3object.h"


amf3objectmap::amf3objectmap(amf3classdef & classdef)
{
    selfdel = true;
    this->classdef = amf3classdef(classdef);
    anoncd = false;
}

amf3objectmap::amf3objectmap(amf3objectmap & objectmap)
{
    anoncd = false;
    classdef = objectmap.classdef;
    for (uint32_t i = 0; i < objectmap.properties.properties.size(); ++i)
    {
        properties.AddObj(objectmap.properties.propnames.at(i), objectmap.properties.GetObj(i));
    }
    selfdel = false;
}

amf3objectmap::amf3objectmap()
{
    anoncd = true;
    selfdel = true;
}

amf3objectmap::~amf3objectmap()
{

}

amf3object & amf3objectmap::Get(std::string key)
{
    return properties.GetObj(key);
}

void amf3objectmap::Add(std::string key, amf3object & obj)
{
    properties.AddObj(key, obj);
}

void amf3objectmap::Add(std::string key)
{
    properties.AddObj(key, amf3object());
}

int amf3objectmap::Exists(std::string key)
{
    return properties.Exists(key);
}

bool amf3objectmap::IsEqual(amf3objectmap * obj)
{
    for (uint32_t i = 0; i < properties.properties.size(); ++i)
    {
        if (obj->properties.properties.at(i).IsNEqual(properties.properties.at(i)))
            return false;
    }
    return true;
}
