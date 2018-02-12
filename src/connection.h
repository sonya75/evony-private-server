/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#define MAXPACKETSIZE 32768/10

#include <asio.hpp>
#include <memory>
#include <array>
#include <mutex>
#include "request_handler.h"

class spitfire;
class Client;

/// Represents a single connection from a client.
class connection : public std::enable_shared_from_this<connection>
{
public:
	connection(const connection&) = delete;
	connection& operator=(const connection&) = delete;

	/// Construct a connection with the given io_service.
	explicit connection(::asio::ip::tcp::socket socket, request_handler& handler);

	/// Get the socket associated with the connection.
	asio::ip::tcp::socket& socket();

	/// Start the first asynchronous operation for the connection.
	void start();
	void startpolicy();

	/// Stop all asynchronous operations associated with the connection.
	void stop();

	void write(const char * data, const int32_t size);

private:
	/// Handle completion of a read operation.
	void handle_read_policy(const asio::error_code& e,
		std::size_t bytes_transferred);
	void handle_read_header(const asio::error_code& e,
		std::size_t bytes_transferred);
	void handle_read(const asio::error_code& e,
		std::size_t bytes_transferred);
	/// Handle completion of a write operation.
	void handle_write(const asio::error_code& e);

	/// Socket for the connection.
	asio::ip::tcp::socket socket_;

	/// The handler used to process the incoming request.
	request_handler& request_handler_;

	/// Buffer for incoming data.
	std::array<char, MAXPACKETSIZE> buffer_;

	/// The incoming request.
	request request_;

	int32_t size;

public:
	uint64_t uid = 0;

	Client * client_ = nullptr;

	std::string address;

	std::mutex mtx;

	bool policy = false;
    
	uint64_t lastpacketsent = 0;

	uint64_t lastpacketreceive = 0;

	uint64_t timeconnected = 0;
};

typedef std::shared_ptr<connection> connection_ptr;
