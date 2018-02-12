/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include <string.h>

#include "amf3writer.h"
#include "amf3objectmap.h"


amf3writer::amf3writer(char * stream)
{
    if (stream == 0)
        throw "Stream error";
    this->stream = stream;
    this->position = 0;
}


amf3writer::~amf3writer(void)
{
}

void amf3writer::Write(Amf3TypeCode type)
{
    stream[position++] = type;
}

void amf3writer::Write(const amf3object & obj)
{
    if (obj.type == Null)
    {
        Write(Null);
        return;
    }
    if (obj.type == True)
    {
        Write(True);
        return;
    }
    if (obj.type == False)
    {
        Write(False);
        return;
    }
    if (obj.type == Integer)
    {
        Write((int32_t)obj._value.integer);
        return;
    }
    if (obj.type == Number)
    {
        Write(obj._value.number);
        return;
    }
    if (obj.type == String)
    {
        Write(obj.text);
        return;
    }
    if (obj.type == Date)
    {
        Write(Date);
        TypelessWrite(obj.date);
        return;
    }
    if (obj.type == Array)
    {
        Write(obj._array.get(), obj);
        return;
    }
    if (obj.type == Object)
    {
        Write(obj._object.get(), obj);
        return;
    }
    throw "Invalid object type";
}
void amf3writer::Write(short integer)
{
    Write((int)integer);
}
void amf3writer::Write(char integer)
{
    Write((int)integer);
}
void amf3writer::Write(unsigned short integer)
{
    Write((int)integer);
}
void amf3writer::Write(unsigned char integer)
{
    Write((int)integer);
}
void amf3writer::Write(unsigned int integer)
{
    Write((int)integer);
}
void amf3writer::Write(int integer)
{
    Write(Integer);
    TypelessWrite((int)integer);
}
void amf3writer::Write(double number)
{
    Write(Number);
    TypelessWrite(number);
}
void amf3writer::Write(std::string str)
{
    Write(String);
    TypelessWrite(str);
}
void amf3writer::Write(amf3array * _array, const amf3object & obj)
{
    Write(Array);
    TypelessWrite(_array, obj);
}
void amf3writer::Write(amf3objectmap * _object, const amf3object & obj)
{
    Write(Object);
    TypelessWrite(_object, obj);
}
void amf3writer::TypelessWrite(uint32_t integer)
{
    TypelessWrite(int(integer));
}
void amf3writer::TypelessWrite(int integer)
{
    integer &= ~(7 << 29);

    if ((integer & (0xFF << 21)) != 0)
    {
        stream[position++] = (uint8_t)(((integer >> 22) & 0x7f) | 0x80);
        stream[position++] = (uint8_t)(((integer >> 15) & 0x7f) | 0x80);
        stream[position++] = (uint8_t)(((integer >> 8) & 0x7f) | 0x80);
        stream[position++] = (uint8_t)(integer & 0xff);
        return;
    }

    bool force = false;

    if ((integer & (0x7f << 14)) != 0)
    {
        stream[position++] = (char)(((integer >> 14) & 0x7f) | 0x80);
        force = true;
    }

    if (force || (integer & (0x7f << 7)) != 0)
    {
        stream[position++] = (char)(((integer >> 7) & 0x7f) | 0x80);
    }
    stream[position++] = (char)(integer & 0x7f);
}
void amf3writer::TypelessWrite(double number)
{
    unsigned char num[8];

    *(double*)num = number;

    int i = 0;
    int j = 7;
    while (i<j)
    {
        num[i] ^= num[j];
        num[j] ^= num[i];
        num[i] ^= num[j];
        i++, j--;
    }

    stream[position++] = num[0];
    stream[position++] = num[1];
    stream[position++] = num[2];
    stream[position++] = num[3];
    stream[position++] = num[4];
    stream[position++] = num[5];
    stream[position++] = num[6];
    stream[position++] = num[7];
}
void amf3writer::TypelessWrite(std::string str)
{
    if (str.length() == 0)
    {
        TypelessWrite(1);
        return;
    }


    std::map<int, std::string>::const_iterator iter;
    iter = stringTable.begin();
    for (int i = 0; i < stringTable.size(); ++iter, ++i)
    {
        if (iter->second == str)
        {
            TypelessWrite(i << 1);
            return;
        }
    }

    //Need UTF8 code here...
    TypelessWrite((int)(str.length() << 1 | 1));
    memcpy(stream + position, str.c_str(), str.length());
    position += str.length();

    stringTable.insert(std::pair<int32_t, std::string>(int32_t(stringTable.size()), str));
    strlist.AddObj(str);
}
void amf3writer::WriteDictionary(amf3reflist<amf3object> * reflist)
{
    for (int i = 0; i < reflist->propnames.size(); ++i)
    {
        TypelessWrite(reflist->propnames.at(i));
        Write(reflist->properties.at(i));
    }
    TypelessWrite("");
}
void amf3writer::TypelessWrite(amf3array * _array, const amf3object & obj)
{
    if (CheckObjectTable(obj))
        return;

    typedef std::pair <int32_t, amf3object> Int_Pair;
    objectTable.insert(Int_Pair(int32_t(objectTable.size()), obj));
    //encapslist.AddObj(obj);

    TypelessWrite(int32_t(obj._array->dense.size() << 1 | 1));

    //	if (obj._value._array->associative.propnames.size() > 0)
    {
        WriteDictionary(&obj._array->associative);
    }
    //	else
    {
        //TypelessWrite(1);
    }

    for (int i = 0; i < obj._array->dense.size(); ++i)
    {
        Write(obj._array->dense.at(i));
    }
}
void amf3writer::TypelessWrite(amf3objectmap * _object, const amf3object & obj)
{
    if (CheckObjectTable(obj))
        return;

    bool found = false;
    std::map<int32_t, amf3classdef>::const_iterator iter;
    iter = classdefTable.begin();
    for (int32_t i = 0; i < classdefTable.size(); ++iter, ++i)
    {
        if ((iter->second).IsEqual(obj._object->classdef))
        {
            TypelessWrite((i << 2) | 1);
            found = true;
            if (obj._object->anoncd)
            {
                obj._object->selfdel = false;
                //delete obj._value._object->classdef;
                //obj._value._object->classdef = 0;
                obj._object->classdef = iter->second;
            }
            break;
        }
    }

    if (!found)
    {
        typedef std::pair <int32_t, amf3classdef> Int_Pair;
        classdefTable.insert(Int_Pair(int32_t(classdefTable.size()), obj._object->classdef));
        deflist.AddObj(obj._object->classdef);

        int flags = Inline | InlineClassDef;
        if (_object->classdef.externalizable)
            flags |= Externalizable;
        if (_object->classdef.dynamic)
            flags |= Dynamic;

        TypelessWrite(int32_t(flags | (_object->classdef.properties.size() << 4)));

        TypelessWrite(_object->classdef.name);

        for (int i = 0; i < _object->classdef.properties.size(); ++i)
        {
            TypelessWrite(_object->classdef.properties.at(i));
        }
    }

    typedef std::pair <int32_t, amf3object> Int_Pair;
    objectTable.insert(Int_Pair(int32_t(objectTable.size()), obj));
    //encapslist.AddObj(obj);

    if (_object->classdef.externalizable)
    {
        Write((amf3object&)_object->Get("inner"));
        return;
    }

    for (int i = 0; i < _object->classdef.properties.size(); ++i)
    {
        Write((amf3object&)_object->Get(_object->classdef.properties.at(i)));
    }

    bool booltest = false;
    if (_object->classdef.dynamic)
    {
        for (int i = 0; i < _object->properties.properties.size(); ++i)
        {
            /************************************************************************/
            /* This is for "true" flash applications that would know the difference */
            /************************************************************************/
            /*if (_object->classdef->properties)
            {
            booltest= false;
            for (int x = 0; x < _object->properties.propnames.size(); ++x)
            {
            if (!strcmp(_object->classdef->properties->at(i)))
            booltest = true;
            }
            if (booltest == false)
            {
            TypelessWrite((char*)_object->properties.propnames.at(i));
            Write(_object->Get(_object->properties.propnames.at(i)));
            }
            }
            else*/
            {
                TypelessWrite(_object->properties.propnames.at(i));
                Write((amf3object&)_object->Get(_object->properties.propnames.at(i)));
            }
        }
        TypelessWrite("");
    }


}
