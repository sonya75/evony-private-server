/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

/*
PocoNetd.lib
PocoNetSSLd.lib
*/
#ifndef WIN32
#include <stdio.h>
#include <signal.h>
void sigfunc(int sig_no);
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
// #ifndef WIN32
//     struct sigaction sa = { 0 };
//     sa.sa_handler = &sigfunc;
//     sigaction(SIGINT, &sa, 0);
//     sigaction(SIGHUP, &sa, 0);
//     sigaction(SIGUSR1, &sa, 0);
//     sigaction(SIGUSR2, &sa, 0);
// #endif

/*
    auto randstr = [](uint16_t n) -> std::string
    {
        std::stringstream ss;
        char r[] = "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "0123456789";
        for (int i = 0; i < n; ++i)
        {
            ss << rand() % sizeof(r);
        }
        return ss.str();
    };

    {
        amf3object obj;
        amf3array arr;
        for (int i = 0; i < 1000; ++i)
        {
            arr.Add(amf3object(randstr(10)));
        }
    }*/



    srand(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()));
 
    spitfire::CreateInstance();
    spitfire::GetSingleton().serverstatus = spitfire::SERVERSTATUS_STARTING;

    try
    {
        //Initialize server and load config files
        if (!spitfire::GetSingleton().Init()) { printf("Init() Exception: Init Failure"); std::cin.ignore(); return 0; }

        //Connect to SQL server
        if (!spitfire::GetSingleton().ConnectSQL()) { printf("Init() Exception: SQL Failure"); std::cin.ignore(); return 0; }

        //Open sockets
        if (!spitfire::GetSingleton().InitSockets()) { printf("Init() Exception: Socket failure"); std::cin.ignore(); return 0; }
    }
    catch (std::exception& e)
    {
        printf("Init() Exception: %s", e.what());
        std::cin.ignore();
        return 0;
    }
    catch (...)
    {
        printf("Unspecified Init() Exception.");
        std::cin.ignore();
        return 0;
    }

    //Server is finally online here
    spitfire::GetSingleton().serverstatus = spitfire::SERVERSTATUS_ONLINE;


    // Run the server until stopped.
    try
    {
        asio::io_service::work work(spitfire::GetSingleton().io_service);
        spitfire::GetSingleton().run();
    }
    catch (Poco::Data::MySQL::MySQLException * e)
    {
        spitfire::GetSingleton().log->critical("SQL Exception: {}", e->displayText());
    }
    catch (std::exception & e)
    {
        spitfire::GetSingleton().log->critical("gserver->run() Exception: {}", e.what());
    }

    spitfire::GetSingleton().log->critical("Shutting down");

    //does this need a thread join here?

    spitfire::GetSingleton().stop_all();
    spitfire::DestroyInstance();
    printf("Exiting...\n");

    std::cin.ignore();
    return 0;
}

#ifndef WIN32
void sigfunc(int sig_no)
{
    printf("Signal received: %d\n", sig_no);
    //	if (sig_no == SIGSEGV)
    //	{
    //		Log("Seg fault onoes!");
    //	}
    /*else*/ if (sig_no == SIGINT)
    {
        printf("Interrupt request\n");
        //spitfire::GetSingleton().Shutdown();
        //LoginServer->serverrunning = false;
    }
    else if (sig_no == SIGHUP)
    {
        printf("Rehash Signal\n");
    }
    else if (sig_no == SIGUSR1)
    {
        printf("user1 Signal\n");
    }
    else if (sig_no == SIGUSR2)
    {
        printf("user2 Signal\n");
    }
}
#endif
