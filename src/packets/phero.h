/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include "packet.h"


class phero : public packet
{
public:
    phero(spitfire & server, request & req, amf3object & o);
    ~phero();
    void process();
};

