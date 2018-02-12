/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include "amf3.h"


class Utils
{
public:
    static uint64_t time();
    static void a_swap(unsigned char * a, unsigned char * b);
    static void ByteSwap5(unsigned char * b, int n);
    static int htoi(char hex);
    static char itoh(int num);
    static std::string GetBuildingName(int id);
    static bool ci_equal(char ch1, char ch2);
    static size_t ci_find(const std::string& str1, const std::string& str2);
    static std::string makesafe(std::string in);
    static char * strtolower(char * x);
};
