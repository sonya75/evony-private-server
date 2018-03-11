/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#include "connection.h"
#include "spitfire.h"
#include "request_handler.h"
#include "Client.h"
#include "Utils.h"
#include <Poco/Data/MySQL/MySQLException.h>


connection::connection(asio::ip::tcp::socket socket, request_handler& handler)
    : socket_(std::move(socket)),
    request_handler_(handler),
    uid(0),
    client_(nullptr)
{
    policy = false;
    size = 0;
    lastpacketsent = lastpacketreceive = timeconnected = 0;
}

asio::ip::tcp::socket& connection::socket()
{
    return socket_;
}

void connection::start()
{
    timeconnected = Utils::time();

    uid = rand()*rand()*rand();

    spitfire::GetSingleton().PlayerCount(1);

    asio::async_read(socket_, asio::buffer(buffer_.data(), 4), std::bind(&connection::handle_read_header, shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2));
}

void connection::startpolicy()
{
    timeconnected = Utils::time();
    asio::async_read(socket_, asio::buffer(buffer_.data(), 22), std::bind(&connection::handle_read_policy, shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2));
}

void connection::stop()
{
    socket_.close();
    if (client_)
    {
        uint32_t tc = 0;
        for (Client * client : spitfire::GetSingleton().players)
        {
            if (client->socket)
            {
                tc++;
            }
        }
        spitfire::GetSingleton().currentplayersonline = tc;

        spitfire::GetSingleton().log->info("Players online: {}", tc);

        //         if (!client_->m_accountexists)
        //         {
        //             int num = client_->m_clientnumber;
        //             delete s->m_clients[num];
        //             s->m_clients[num] = 0;
        //             return;
        //         }
        client_->socket = 0;
        client_->socknum = 0;
        client_ = nullptr;
        //spitfire::GetSingleton().PlayerCount(-1);

        //TODO: record last online time
    }
}

void connection::write(const char * data, const int32_t size)
{
    if (!this)
        return;
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(data, size),
        [this, self, size](asio::error_code ec, std::size_t written)
    {
        lastpacketsent = Utils::time();
        if (!ec)
        {
            // No socket errors on write
            if (size != written)
            {
                //sent size not matching
                spitfire::GetSingleton().stop(shared_from_this(), false);
                std::cerr << "Data sent does not match size sent. Socket closed.\n";
            }
        }
        else if (ec != asio::error::operation_aborted)
        {
            spitfire::GetSingleton().stop(shared_from_this(), false);
            std::cerr << "asio::async_write() failure\n";
        }
        else
        {
            std::cerr << "asio::async_write() failure - operation_aborted\n";
        }
    });
}

void connection::handle_read_policy(const asio::error_code& e,
    std::size_t bytes_transferred)
{
    if (bytes_transferred)
    {
        if (!memcmp(buffer_.data(), "<policy-file-request/>", 22))
        {
            auto self(shared_from_this());
            write("<?xml version=\"1.0\"?><!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\"><cross-domain-policy><site-control permitted-cross-domain-policies=\"master-only\"/><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>\n\n\0", 246);
            policy = true;
            lastpacketsent = Utils::time();
            //Server::GetSingleton().stop(shared_from_this());
        }
    }
    else if (e != asio::error::operation_aborted)
    {
        spitfire::GetSingleton().stop(shared_from_this());
        return;
    }

}

void connection::handle_read_header(const asio::error_code& e,
    std::size_t bytes_transferred)
{
    if (!e)
    {
        if (bytes_transferred == 4)
        {
            size = *(int32_t*)buffer_.data();
            Utils::ByteSwap5((unsigned char*)&size, sizeof(size));

            if ((size < 4) || (size >= MAXPACKETSIZE))
            {
                spitfire::GetSingleton().log->error("Did not receive proper amount of bytes : sent: {} - ip:{}", size, address);
                spitfire::GetSingleton().stop(shared_from_this());
                return;
            }

            asio::async_read(socket_, asio::buffer(buffer_.data(), size), std::bind(&connection::handle_read, shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
        }
    }
    else if (e != asio::error::operation_aborted)
    {
        spitfire::GetSingleton().stop(shared_from_this());
        return;
    }

}

void connection::handle_read(const asio::error_code& e,
    std::size_t bytes_transferred)
{
    if (!e)
    {
        if (bytes_transferred != size)
        {
            spitfire::GetSingleton().log->error("Did not receive proper amount of bytes : rcv: {} needed: {}", bytes_transferred, size);
            spitfire::GetSingleton().stop(shared_from_this());
            return;
        }
        char * t = buffer_.data();
        if ((*(int8_t*)t != 0x0a) && (*(int8_t*)(t + 1) != 0x0b) && (*(int8_t*)(t + 2) != 0x01))
        {
            spitfire::GetSingleton().log->error("Not an AMF3 object - ip:{}", address);
            spitfire::GetSingleton().stop(shared_from_this());
            return;
        }
        //printf("uid("XI64")\n", uid);
        // read object size
        if ((size > MAXPACKETSIZE) || (size <= 0))
        {
            //ERROR - object too large - close connection
            spitfire::GetSingleton().stop(shared_from_this());
            return;
        }

        //TODO: Decision: have socket read thread handle packets, or push into a queue
        //socket thread (easy, already done)
        // PRO: typically instant response times due to it being processed as it comes in
        // CON: a large request (legit or non) would cause the socket read thread to lag
        // 
        //process thread (complex, ideally better)
        // PRO: can alleviate lag on socket threads and create multiple thread processing queues depending on importance
        // CON: complexity and large requests would typically land in the same thread (causing even more lag for them) unless
        //        made even more complex to have multiple threads for large requests
        //
        //Option 3: Evony style
        // -- create a process thread per x amount of sockets
        // PRO: lag from one client only affects a set amount of players and not the entire server
        // CON: quite complex. is ultimately the process thread option only for x amount of sockets

        lastpacketreceive = Utils::time();

        // parse packet
        request_.size = size;
        try
        {
            amf3parser cparser(buffer_.data());
            request_.object = cparser.ReadNextObject();
        }
        catch (...)
        {
            std::cerr << "uncaught handle_request()::amf3parser exception\n";
        }
        request_.conn = this;
        {
//             std::lock_guard<std::mutex> l(spitfire::GetSingleton().m);
//             spitfire::GetSingleton().packetqueue.push_back(request_);
            request_handler_.handle_request(spitfire::GetSingleton(), request_);
        }

        //         if (reply_.objects.size() > 0)
        //         {
        //             // send reply packets
        //             try {
        //                 socket_.write_some(reply_.to_buffers());
        //             }
        //             catch (std::exception& e)
        //             {
        //                 std::cerr << "asio::write_some() exception: " << e.what() << "\n";
        //             }
        //             reply_.objects.clear();
        //         }

        asio::async_read(socket_, asio::buffer(buffer_.data(), 4), std::bind(&connection::handle_read_header, shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2));
    }
    else if (e != asio::error::operation_aborted)
    {
        spitfire::GetSingleton().stop(shared_from_this());
        return;
    }
}

void connection::handle_write(const asio::error_code& e)
{
    if (!e)
    {
        //         // Initiate graceful connection closure.
        //         asio::error_code ignored_ec;
        //         socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }

    if (e != asio::error::operation_aborted)
    {
        spitfire::GetSingleton().stop(shared_from_this());
    }
}
