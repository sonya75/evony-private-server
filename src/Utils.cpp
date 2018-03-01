/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "Utils.h"

#include <cstdio>
#include <stdarg.h>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <chrono>

uint64_t Utils::time()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
void Utils::a_swap(unsigned char * a, unsigned char * b)
{
    char c, d;
    c = *a;
    d = *b;
    *b = c;
    *a = d;
}
void Utils::ByteSwap5(unsigned char * b, int n)
{
    int i = 0;
    int j = n - 1;
    while (i < j)
    {
        Utils::a_swap(&b[i], &b[j]);
        i++, j--;
    }
}
int Utils::htoi(char hex)
{
    switch (hex)
    {
        case 49:
            return 1;
        case 50:
            return 2;
        case 51:
            return 3;
        case 52:
            return 4;
        case 53:
            return 5;
        case 54:
            return 6;
        case 55:
            return 7;
        case 56:
            return 8;
        case 57:
            return 9;
        case 48:
            return 0;
        case 97://a
            return 10;
        case 98://b
            return 11;
        case 99://c
            return 12;
        case 100://d
            return 13;
        case 101://e
            return 14;
        case 102://f
            return 15;
    }
    return 0;
}
char Utils::itoh(int num)
{
    switch (num)
    {
        case 1:
            return 49;
        case 2:
            return 50;
        case 3:
            return 51;
        case 4:
            return 52;
        case 5:
            return 53;
        case 6:
            return 54;
        case 7:
            return 55;
        case 8:
            return 56;
        case 9:
            return 57;
        case 0:
            return 48;
        case 10://a
            return 97;
        case 11://b
            return 98;
        case 12://c
            return 99;
        case 13://d
            return 100;
        case 14://e
            return 101;
        case 15://f
            return 102;
    }
    return 0;
}

std::string Utils::GetBuildingName(int id)
{
    switch (id)
    {
        case 1:
            return "Cottage";
        case 2:
            return "Barracks";
        case 3:
            return "Warehouse";
        case 4:
            return "Sawmill";
        case 5:
            return "Stonemine";
        case 6:
            return "Ironmine";
        case 7:
            return "Farm";
        case 20:
            return "Stable";
        case 21:
            return "Inn";
        case 22:
            return "Forge";
        case 23:
            return "Marketplace";
        case 24:
            return "Relief Station";
        case 25:
            return "Academy";
        case 26:
            return "Workshop";
        case 27:
            return "Feasting Hall";
        case 28:
            return "Embassy";
        case 29:
            return "Rally Spot";
        case 30:
            return "Beacon Tower";
        case 31:
            return "Town Hall";
        case 32:
            return "Walls";
    }
    return "Failed";
}

bool Utils::ci_equal(char ch1, char ch2)
{
    return toupper((unsigned char)ch1) == toupper((unsigned char)ch2);
}

size_t Utils::ci_find(const std::string& str1, const std::string& str2)
{
    std::string::const_iterator pos = search(str1.begin(), str1.end(), str2.
                                             begin(), str2.end(), ci_equal);
    if (pos == str1.end())
        return std::string::npos;
    else
        return pos - str1.begin();
}

std::string Utils::makesafe(std::string in)
{
    std::stringstream ss;
    for (int i = 0; i < in.size(); ++i)
    {
        if (((in[i] >= '0') && (in[i] <= '9')) || ((in[i] >= 'A') && (in[i] <= 'Z')) || ((in[i] >= 'a') && (in[i] <= 'z')) || (in[i] == '_') || (in[i] == '.') || (in[i] == '/') || (in[i] == ':') || (in[i] == '@') || (in[i] == '|'))
            ss << in[i];
        else
            ss << ' ';
    }
    return ss.str();
}

char * Utils::strtolower(char * x)
{
    for (int i = 0; i < strlen(x); ++i)
    {
        if (x[i] >= 65 && x[i] <= 90)
        {
            x[i] += 32;
        }
    }
    return x;
}

int32_t Utils::TROOP_EXPS[12] = { 225, 305, 695, 900, 1350, 1350, 2975, 2850, 8750, 10000, 13750, 29000 };

std::string Utils::generaterandomstring(int x)
{
    static const char characters[37] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char * y = new char[x+1];
    y[x] = 0;
    for (int i = 0; i < x; i++) {
        y[i] = characters[rand() % 36];
    }
    std::string str(y);
    delete[] y;
    return str;
}