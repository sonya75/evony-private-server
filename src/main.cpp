/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#ifndef WIN32
#include <stdio.h>
#else
#define WIN32_LEAN_AND_MEAN   
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <signal.h>
#include <conio.h>
#endif

#include <iostream>
#include <chrono>

#include "spitfire.h"
#include "Utils.h"
#include <Poco/Data/MySQL/MySQLException.h>


int main(int argc, char * argv[])
{
    std::cout << "Startup\n";
 
    try
    {
        spitfire server;

        server.serverstatus = spitfire::SERVERSTATUS_STARTING;

        //Initialize server and load config files
        if (!server.initialize()) { printf("Init() Exception: Init Failure"); std::cin.ignore(); return 0; }

        //Server is finally online here
        server.serverstatus = spitfire::SERVERSTATUS_ONLINE;


        // Run the server until stopped.
        try
        {
            asio::io_service::work work(server.io_service);
            server.run(5);//threads to run
        }
        catch (Poco::Data::MySQL::MySQLException * e)
        {
            server.log->critical("SQL Exception: {}", e->displayText());
        }
        catch (std::exception & e)
        {
            server.log->critical("server->run() Exception: {}", e.what());
        }

        server.log->critical("Shutting down");

        //does this need a thread join here?

        server.stop_all();
        printf("Exiting...\n");
    }
    catch (std::exception& e)
    {
        printf("main() Exception: %s", e.what());
        std::cin.ignore();
        return 0;
    }
    catch (...)
    {
        printf("Unspecified main() Exception.");
        std::cin.ignore();
        return 0;
    }

    printf("Exited. Press enter to continue.\n");

    std::cin.ignore();
    return 0;
}
