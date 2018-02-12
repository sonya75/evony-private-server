/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "amf3object.h"
#include "amf3array.h"
#include "amf3objectmap.h"



amf3object::amf3object()
{
    type = Null;
    _value.integer = 0;
}

amf3object::amf3object(const amf3object &val)
{
    InternalCopy(val);
}

amf3object::amf3object(const std::string &val)
{
    type = String;
    text = val;
}

void amf3object::InternalCopy(const amf3object &val)
{
    type = val.type;
    _value.integer = 0;
    switch (val.type)
    {
        case Date:
        {
            date = val.date;
            break;
        }
        case String:
        {
            text = val.text;
            break;
        }
        case Array:
        {
            _array = val._array;
            break;
        }
        case Object:
        {
            _object = val._object;
            break;
        }
        default:
        {
            _value.integer = val._value.integer;
            break;
        }
    }
}

void amf3object::InternalCopy(const amf3array &val)
{
    type = Array;
    _value.integer = 0;
    _array = std::shared_ptr<amf3array>(new amf3array(val));
    //_value._array = new amf3array3(val);
}

void amf3object::Reset(bool isUndefined)
{
    switch (type)
    {
        case String:
        {
            text = "";
            break;
        }
        case Array:
        {
            _array.reset();
            break;
        }
        case Object:
        {
            _object.reset();
            break;
        }
        default:
        {
            break;
        }
    }
    if (isUndefined)
        type = Undefined;
    else
        type = Null;
    _value.integer = 0;
}

amf3object::~amf3object()
{
    Reset();
}

amf3object::amf3object(const bool val)
{
    if (val)
        type = True;
    else
        type = False;
    _value.booltest = val;
}

amf3object::amf3object(const double val)
{
    type = Number;
    _value.number = val;
}

amf3object& amf3object::operator=(const double &val)
{
    Reset();
    type = Number;
    _value.number = val;
    return *this;
}

amf3object& amf3object::operator=(const bool &val)
{
    Reset();
    if (val)
        type = True;
    else
        type = False;
    _value.booltest = false;
    return *this;
}

amf3object& amf3object::operator=(const amf3object &val)
{
    Reset();
    InternalCopy(val);
    return *this;
}

amf3object& amf3object::operator=(const amf3array &val)
{
    Reset();
    InternalCopy(val);
    return *this;
}

amf3object& amf3object::operator=(const char *pVal)
{
    Reset();
    type = String;
    this->text = pVal;
    return *this;
}

amf3object& amf3object::operator=(const std::string &val)
{
    Reset();
    type = String;
    this->text = val;
    return *this;
}

amf3object::operator Amf3TypeCode()
{
    return type;
}

amf3object::operator bool()
{
    switch (type)
    {
        case Null:
        case Undefined:
            return false;
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return (_value.integer != 0);
        case Number:
            return (_value.number != 0);
        default:
            return false;
    }
}

amf3object::operator double()
{
    switch (type)
    {
        case Null:
        case Undefined:
            return false;
        case True:
            return 1.0;
        case False:
            return 0.0;
        case Integer:
            return static_cast<double>(_value.integer);
        case Number:
            return _value.number;
        default:
            return false;
    }
}

amf3object::operator uint8_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<uint8_t>(_value.integer);
        case Number:
            return static_cast<uint8_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator uint16_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<uint16_t>(_value.integer);
        case Number:
            return static_cast<uint16_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator uint32_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<uint32_t>(_value.integer);
        case Number:
            return static_cast<uint32_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator uint64_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<uint64_t>(_value.integer);
        case Number:
            return static_cast<uint64_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator int8_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<int8_t>(_value.integer);
        case Number:
            return static_cast<int8_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator int16_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<int16_t>(_value.integer);
        case Number:
            return static_cast<int16_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator int32_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<int32_t>(_value.integer);
        case Number:
            return static_cast<int32_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator int64_t()
{
    switch (type)
    {
        case True:
        case False:
            return _value.booltest;
        case Integer:
            return static_cast<int64_t>(_value.integer);
        case Number:
            return static_cast<int64_t>(_value.number);
        default:
            return 0;
    }
}

amf3object::operator std::string()
{
    switch (type)
    {
        case False:
        case True:
            return _value.booltest ? "true" : "false";
        case Integer:
        {
            std::stringstream ss;
            ss << this->operator int32_t();
            return ss.str();
        }
        case Number:
        {
            std::stringstream ss;
            ss << _value.number;
            return ss.str();
        }
        case String:
        {
            return std::string(text.c_str());
        }
        case Null:
        case Undefined:
        case Object:
        case Array:
        default:
        {
            //ASSERT("Cast to string failed: %s", STR(ToString()));
            return "";
        }
    }
    return "";
}

amf3object::operator amf3array*()
{
    switch (type)
    {
        case False:
        case True:
        case Integer:
        case Number:
        case String:
        case Null:
        case Undefined:
        case Object:
            return 0;
        case Array:
            return _array.get();
        default:
            return 0;
    }
    return 0;
}

amf3object::operator amf3array&()
{
    switch (type)
    {
        case False:
        case True:
        case Integer:
        case Number:
        case String:
        case Null:
        case Undefined:
        case Object:
        default:
            //really reset? or do a throw?
            this->Reset();
            _array = std::make_shared<amf3array>();
            return *(_array.get());
        case Array:
            return *(_array.get());
    }
}

amf3object& amf3object::operator[](const char *key)
{
    if ((type != Object) &&
        (type != Array) &&
        (type != Null) &&
        (type != Undefined))
    {
        //ASSERT("Subscript operator applied on a incorrect Variant type: %s",
        //STR(ToString()));
    }
    else if ((type == Null) || (type == Undefined))
    {
        type = Object;
        _object = std::make_shared<amf3objectmap>();
        _object->Add(std::string(key));
    }
    else if (_object->Exists(std::string(key)) < 0)
    {
        _object->Add(std::string(key));
    }
    return _object->Get(std::string(key));
}

amf3object& amf3object::operator[](const double &key)
{
    return operator[]((uint32_t)key);
}

amf3object& amf3object::operator[](const uint32_t &key)
{
    if ((type != Object) &&
        (type != Array) &&
        (type != Null) &&
        (type != Undefined))
    {
        //ASSERT("Subscript operator applied on a incorrect Variant type: %s",
        //STR(ToString()));
    }
    if ((type == Null) || (type == Undefined))
    {
        throw "Object not an array yet accessed like one";

        //type = Array;
        //_value._array = new amf3array3;
    }
    if (!_array->Get(key))
    {
        throw "Index out of array bounds";
        //amf3object3 temp = amf3object3();
        //_value._array->Add(temp);
    }
    return (_array->Get(key));
}

amf3object& amf3object::operator[](const int &key)
{
    return operator[]((uint32_t)key);
}

bool amf3object::operator==(amf3object variant)
{
    if (type == Integer)
        return this->_value.integer == variant._value.integer;
    if (type == Number)
        return this->_value.number == variant._value.number;
    if (type == String)
        return this->text == variant.text;
    if (type == True)
        if (variant.type == True)
            return true;
    if (type == False)
        if (variant.type == False)
            return true;
    return false;
    // TODO: Finish (add Object and Array cases? Or leave as always not equal?)
}

bool amf3object::operator==(const char * str)
{
    if (type == Integer)
        return false;
    if (type == Number)
        return false;
    if (type == String)
        return this->text == str;
    if (type == True)
        return false;
    if (type == False)
        return false;
    return false;
    // TODO: Finish (add Object and Array cases? Or leave as always not equal?)
}

bool amf3object::operator==(char * str)
{
    return operator==((const char *)str);
}

bool amf3object::operator==(std::string str)
{
    return operator==((const char*)str.c_str());
}

bool amf3object::operator!=(amf3object variant)
{
    return !operator==(variant);
}

bool amf3object::operator==(Amf3TypeCode type)
{
    if ((type == Integer) || (type == Number))
        return type == Integer || type == Number;
    else
        return this->type == type;
}

bool amf3object::operator!=(Amf3TypeCode type)
{
    return !operator ==(type);
}

bool amf3object::IsEqual(amf3object & obj2)
{
    if (obj2.type != type)
        return false;
    switch (obj2.type)
    {
        case 2:
            if (_value.booltest == false)
                return true;
            break;
        case 3:
            if (_value.booltest == true)
                return true;
            break;
        case 4:
            if (_value.integer == obj2._value.integer)
                return true;
            break;
        case 5:
            if (_value.number == obj2._value.number)
                return true;
            break;
        case 6:
            if (text == obj2.text)
                return true;
            break;
        case 7:
            return false;
        case 8:
            if (date == obj2.date)
                return true;
            break;
        case 9:
            if (obj2._array->IsEqual(_array.get()))
                return true;
            break;
        case 10:
            if (obj2._object->IsEqual(_object.get()))
                return true;
            break;
        case 11:
        case 12:
            return false;
    }
    return false;
}
