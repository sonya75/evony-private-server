/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "amf3classdef.h"



amf3classdef::amf3classdef()
{
    dynamic = true;
    externalizable = false;
}

amf3classdef::amf3classdef(std::string name, std::vector<std::string> & properties, bool dynamic, bool externalizable)
    : name(name)
    , properties(properties)
    , dynamic(dynamic)
    , externalizable(externalizable)
{

}

amf3classdef::amf3classdef(const amf3classdef & classdef)
    : name(classdef.name)
    , properties(classdef.properties)
    , dynamic(classdef.dynamic)
    , externalizable(classdef.externalizable)
{

}

amf3classdef::~amf3classdef(void)
{

}

bool amf3classdef::IsEqual(amf3classdef & obj) const
{
    if (name != obj.name || dynamic != obj.dynamic || externalizable != obj.externalizable || (properties.size() != obj.properties.size()))
        return false;

    if (properties.size() == obj.properties.size())
    {
        for (uint32_t i = 0; i < properties.size(); ++i)
        {
            if (properties.at(i) != obj.properties.at(i))
                return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}
