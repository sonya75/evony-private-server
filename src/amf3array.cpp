/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "amf3array.h"


amf3array::amf3array()
{
    this->type = 1;
}

amf3array::~amf3array(void)
{

}

void amf3array::Add(amf3object & obj)
{
    this->type = 1;
    dense.push_back(obj);
}

void amf3array::Add(amf3object && obj)
{
    this->type = 1;
    dense.push_back(std::move(obj));
}

void amf3array::Add(std::string key, amf3object & obj)
{
    this->type = 2;
    associative.AddObj(key, obj);
    dense.push_back(obj);
}

amf3object & amf3array::Get(int id)
{
    return dense.at(id);
}

amf3object & amf3array::Get(std::string key)
{
    return associative.GetObj(key);
}

bool amf3array::IsEqual(amf3array * obj)
{
    if (this->type == 1)
    {
        if (this->dense.size() != obj->dense.size())
            return false;
        for (uint32_t i = 0; i < this->dense.size(); ++i)
        {
            if (obj->dense.at(i).IsNEqual(this->dense.at(i)))
                return false;
        }
    }
    else if (this->type == 2)
    {
        if (this->associative.properties.size() != obj->associative.properties.size())
            return false;
        for (uint32_t i = 0; i < this->associative.properties.size(); ++i)
        {
            if (obj->associative.properties.at(i).IsNEqual(this->associative.properties.at(i)))
                return false;
        }
    }
    return true;
}
