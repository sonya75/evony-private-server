/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <string>
#include <vector>
#include <asio.hpp>
#include "amf3.h"
#include "Utils.h"

class connection;
class spitfire;

struct reply
{
    std::vector<amf3object> objects;

    /// Convert the reply into a vector of buffers. The buffers do not own the
    /// underlying memory blocks, therefore the reply object must remain valid and
    /// not be changed until the write operation has completed.
    std::vector<asio::const_buffer> to_buffers()
    {
        std::vector<asio::const_buffer> buffers;
        for (std::size_t i = 0; i < objects.size(); ++i)
        {
            amf3writer * writer;

            //TODO: use boost buffer object instead?
            char tbuff[2000];
            int length = 0;
            writer = new amf3writer(tbuff + 4);

            writer->Write(objects[i]);

            (*(int32_t*)tbuff) = length = (uint32_t)writer->position;
            Utils::ByteSwap5((unsigned char*)&tbuff, sizeof(*(int32_t*)tbuff));

            buffers.push_back(asio::buffer(tbuff, length + 4));
            delete writer;
        }
        return buffers;
    }
};
struct request
{
    request()
    {
        size = 0;
        cmd = "";
        uri = "";
        object = amf3object();
        conn = nullptr;
    }
    request(const request & req)
    {
        size = req.size;
        cmd = req.cmd;
        uri = req.uri;
        object = req.object;
        conn = req.conn;
    }
    int32_t size;
    std::string cmd;
    std::string uri;
    amf3object object;
    connection * conn;
};

/// The common handler for all incoming requests.
class request_handler
{
    request_handler(const request_handler&) = delete;
    request_handler& operator=(const request_handler&) = delete;

public:
    explicit request_handler();

    /// Handle a request and produce a reply.
    void handle_request(spitfire & server, request& req) const;
};
