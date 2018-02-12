/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <string>
#include <map>


#include "amf3object.h"
#include "amf3array.h"
#include "amf3classdef.h"

class amf3writer
{
public:
    amf3writer(char * stream);
    ~amf3writer(void);

    bool CheckObjectTable(const amf3object & obj)
    {
        // TODO: Fix object tables
        // 		map<int, amf3object>::const_iterator iter;
        // 		iter = objectTable.begin();
        // 		for (uint32_t i = 0; i < objectTable.size(); ++iter, ++i)
        // 		{
        // 			if (amf3object::IsEqual((amf3object&)(iter->second), obj))
        // 			{
        // 				TypelessWrite(i << 1);
        // 				return true;
        // 			}
        // 		}
        return false;
    }
    void Write(Amf3TypeCode type);
    void Write(const amf3object & obj);
    void Write(short integer);
    void Write(char integer);
    void Write(unsigned short integer);
    void Write(unsigned char integer);
    void Write(unsigned int integer);
    void Write(int integer);
    void Write(double number);
    void Write(std::string str);
    void TypelessWrite(uint32_t integer);
    void TypelessWrite(int integer);
    void TypelessWrite(double number);
    void TypelessWrite(std::string str);
    void WriteDictionary(amf3reflist<amf3object> * reflist);
    void Write(amf3array * _array, const amf3object & obj);
    void Write(amf3objectmap * _object, const amf3object & obj);
    void TypelessWrite(amf3array * _array, const amf3object & obj);
    void TypelessWrite(amf3objectmap * _object, const amf3object & obj);

    amf3reflist<std::string> strlist;
    amf3reflist<amf3object> objectlist;
    amf3reflist<amf3object> encapslist;
    amf3reflist<amf3classdef> deflist;
    std::map<int, amf3object> objectTable;
    std::map<int, std::string> stringTable;
    std::map<int, amf3classdef> classdefTable;

    char * stream;
    int64_t position;
};
