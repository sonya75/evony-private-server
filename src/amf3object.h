/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <memory>

class amf3array;
class amf3objectmap;

enum Amf3TypeCode
{
    Undefined = 0,
    Null = 1,
    False = 2,
    True = 3,
    Integer = 4,
    Number = 5,
    String = 6,
    LegacyXmlDocument = 7,
    Date = 8,
    Array = 9,
    Object = 10,
    Xml = 11,
    ByteArray = 12
};
enum Flags
{
    Inline = 1,
    InlineClassDef = 2,
    Externalizable = 4,
    Dynamic = 8
};

class amf3object
{
public:
    ~amf3object(void);

    Amf3TypeCode type;

    bool IsEqual(amf3object & obj2);

    bool IsNEqual(amf3object & obj2)
    {
        return !(IsEqual(obj2));
    }
    std::string text;
    std::string date;
    union
    {
        bool booltest;
        int64_t integer;
        double number;
    } _value;
    std::shared_ptr<amf3array> _array;
    std::shared_ptr<amf3objectmap> _object;

    const char * c_str()
    {
        return text.c_str();
    }

    amf3object();
    amf3object(const std::string &val);
    amf3object(const amf3object &val);

    template<typename T>
    amf3object(const T val)
    {
        type = Integer;
        _value.integer = val;
    }

    amf3object(const bool val);
    amf3object(const double val);
    void InternalCopy(const amf3object &val);
    void InternalCopy(const amf3array &val);

    template<typename T>
    amf3object & operator=(const T &val)
    {
        Reset();
        type = Integer;
        _value.integer = (T)val;
        return *this;
    }

    amf3object & operator=(const amf3object &val);
    amf3object & operator=(const amf3array &val);
    amf3object & operator=(const bool &val);
    amf3object & operator=(const double &val);

    //Variant & operator=(const Timestamp &val);

    amf3object & operator=(const char *pVal);
    amf3object & operator=(const std::string &val);

    operator std::string();
    operator uint8_t();
    operator uint16_t();
    operator uint32_t();
    operator uint64_t();
    operator int8_t();
    operator int16_t();
    operator int32_t();
    operator int64_t();
    operator Amf3TypeCode();
    operator bool();
    operator double();
    //operator Timestamp();
    //operator char*();
    //operator const char*();
    operator amf3array*();
    operator amf3array&();
    void Reset(bool isUndefined = false);

    std::string temp;

    //amf3object & operator[](const string &key);
    amf3object & operator[](const char *key);
    amf3object & operator[](const double &key);
    amf3object & operator[](const uint32_t &key);
    amf3object & operator[](const int &key);
    amf3object & operator[](amf3object &key);
    amf3object & GetValue(std::string key, bool caseSensitive);

    bool operator==(amf3object variant);
    bool operator==(const char * str);
    bool operator==(char * str);
    bool operator==(std::string str);
    bool operator!=(amf3object variant);
    bool operator==(Amf3TypeCode type);
    bool operator!=(Amf3TypeCode type);

};

