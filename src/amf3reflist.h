/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once


template<typename T>
class amf3reflist
{
public:
    amf3reflist(void) {};
    ~amf3reflist(void)
    {
        properties.clear();
        propnames.clear();
    };

    void AddObj(T obj)
    {
        properties.push_back(obj);
    }

    void AddObj(std::string key, T obj)
    {
        propnames.push_back(key);
        properties.push_back(obj);
    }

    T & GetObj(std::string key)
    {
        for (unsigned int i = 0; i < propnames.size(); ++i)
        {
            if (propnames[i] == key)
                return properties.at(i);
        }
        throw "Key: " + key + " does not exist in object";
    }

    T & GetObj(int offset)
    {
        return properties.at(offset);
    }

    int Exists(std::string key)
    {
        for (unsigned int i = 0; i < propnames.size(); ++i)
        {
            if (propnames[i] == key)
                return i;
        }
        return -1;
    }

    std::vector<std::string> propnames;
    std::vector<T> properties;
};
