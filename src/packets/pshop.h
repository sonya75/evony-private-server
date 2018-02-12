/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "packet.h"

class pshop : public packet
{
public:
    pshop(spitfire & server, request & req, amf3object & o);
    ~pshop();
    void process();

    void ShopUseGoods(amf3object & data, Client * client);
    void ShopUseCastleGoods(amf3object & data, Client * client);
    int32_t GetGambleCount(std::string item);
    amf3object GenerateGamble();
};
