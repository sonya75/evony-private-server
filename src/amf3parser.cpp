/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include <vector>

#include "amf3parser.h"
#include "amf3object.h"
#include "amf3objectmap.h"
#include "amf3array.h"


amf3parser::amf3parser(char * stream)
{
    this->stream = stream;
    position = 0;
}

amf3parser::~amf3parser()
{

}

amf3object amf3parser::ReadNextObject()
{
    int b = this->stream[position++];

    Amf3TypeCode type = (Amf3TypeCode)b;

    amf3object obj = amf3object();

    obj.type = type;

    char * pstr = 0;
    int len = 0;

    std::string str;

    switch (type)
    {
        case Undefined:
        case Null:
            return obj;
        case False:
            obj._value.booltest = false;
            return obj;
        case True:
            obj._value.booltest = true;
            return obj;
        case Integer:
            obj._value.integer = ReadInteger();
            return obj;
        case Number:
            obj._value.number = ReadNumber();
            return obj;
        case String:
            obj.text = ReadString();
            return obj;
        case Date:
            obj.date = ReadDate();
            return obj;
        case Array:
            obj._array = std::shared_ptr<amf3array>(ReadArray());
            return obj;
        case Object:
            obj._object = std::shared_ptr<amf3objectmap>(ReadAMF3Object());
            return obj;
        case LegacyXmlDocument: break;
        case Xml: break;
        case ByteArray: break;
        default: break;
            //default:
            //Log("Invalid object type (%d)", type);
            //delete obj;
    }
    return amf3object(0);
}

int amf3parser::ReadInteger(void)
{
    int64_t integer = 0;
    int seen = 0;
    int b = 0;

    for (;;)
    {
        b = (unsigned char)stream[position++];

        if (seen == 3)
        {
            integer = (integer << 8) | b;
            break;
        }

        integer = (integer << 7) | (b & 0x7f);

        if ((b & 0x80) == 0x80)
        {
            seen++;
        }
        else
        {
            break;
        }
    }

    if (integer > (0x7FFFFFFF >> 3))
        integer -= (1 << 29);

    return (int)integer;
}

double amf3parser::ReadNumber(void)
{
    int integer = 0;
    int seen = 0;
    int b = 0;

    unsigned char num[8];

    num[0] = stream[position++];
    num[1] = stream[position++];
    num[2] = stream[position++];
    num[3] = stream[position++];
    num[4] = stream[position++];
    num[5] = stream[position++];
    num[6] = stream[position++];
    num[7] = stream[position++];
    int i = 0;
    int j = 7;
    while (i < j)
    {
        num[i] ^= num[j];
        num[j] ^= num[i];
        num[i] ^= num[j];
        i++, j--;
    }

    return *(double*)num;
}

std::string amf3parser::ReadString(void)
{
    int num = ReadInteger();

    int len = (num >> 1);
    if ((num & 1) == 0)
    {
        return strlist.GetObj(num >> 1);
    }

    std::string str;
    str.insert(0, stream + position, num >> 1);
    position += (num >> 1);
    if (str.length() > 0)
        strlist.AddObj(str);
    return str;
}

// TODO: Implement
std::string amf3parser::ReadDate(void)
{
    int num = ReadInteger();

    if ((num & 1) == 0)
    {
        return 0;
    }

    double val = ReadNumber();
    return "datetime";
}

amf3array * amf3parser::ReadArray(void)
{
    int num = ReadInteger();

    if ((num & 1) == 0)
    {
        return (amf3array*)objectlist.GetObj(num >> 1);
    }
    num >>= 1;

    amf3array * amfarray = new amf3array();

    std::string key = ReadString();
    if (key.length() != 0)
    {
        amf3object obj = ReadNextObject();
        amfarray->Add(key, obj);
        encapslist.AddObj(obj);

        key = ReadString();
    }

    while (num-- > 0)
    {
        amf3object obj = ReadNextObject();
        amfarray->Add(obj);
        encapslist.AddObj(obj);
    }
    return amfarray;
}

amf3objectmap * amf3parser::ReadAMF3Object(void)
{
    int flags = (Flags)ReadInteger();

    if ((flags & Inline) == 0)
    {
        return objectlist.GetObj(((int)flags) >> 1)._object.get();
    }

    amf3classdef classdef;

    bool cdfound = false;

    int members = 0;

    if ((flags & InlineClassDef) == 0)
    {
        classdef = deflist.GetObj((((int)flags) >> 2));
        cdfound = true;
    }
    else
    {
        bool externalizable = ((flags & Externalizable) != 0);
        bool dynamic = ((flags & Dynamic) != 0);
        std::string name = ReadString();

        if (externalizable && dynamic)
            throw "Serialized objects cannot be both dynamic and externalizable";

        // TODO FIX EXTERNALIZED CLASS CODE
        if (externalizable)
            throw "Externalized object. CATCH()";
        // TODO FIX IT

        std::vector<std::string> properties;

        members = ((int)flags) >> 4;

        for (int i = 0; i < members; i++)
        {
            properties.push_back(ReadString());
        }
        classdef = amf3classdef(name, properties, dynamic, externalizable);
        deflist.AddObj(classdef);
    }

    amf3objectmap * obj = new amf3objectmap(classdef);
    obj->selfdel = (cdfound) ? false : true;
    obj->flags = flags;

    if (classdef.externalizable)
    {
        amf3object temp = ReadNextObject();
        obj->Add("inner", temp);
        return obj;
    }

    amf3object tempobject;
    std::string name;
    for (int i = 0; i < classdef.properties.size(); i++)
    {
        name = classdef.properties.at(i);
        tempobject = ReadNextObject();
        encapslist.AddObj(tempobject);
        obj->Add(name, tempobject);
    }
    amf3object temp;
    if (classdef.dynamic)
    {
        std::string key = ReadString();
        while (key.length() != 0)
        {
            temp = ReadNextObject();
            obj->Add(key, temp);
            encapslist.AddObj(temp);
            key = ReadString();
        }
    }
    return obj;

}
