/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#define DEF_RESEARCH 1
#define DEF_BUILDING 2
#define DEF_TRAIN 3

#define DEF_NORMAL 0
#define DEF_PEACETIME 1
#define DEF_TRUCE 2
#define DEF_BEGINNER 3
#define DEF_HOLIDAY 5

#define FOREST 1
#define DESERT 2
#define HILL 3
#define SWAMP 4
#define GRASS 5
#define LAKE 6
#define FLAT 10
#define CASTLE 11
#define NPC 12


#define WORKER 1
#define WARRIOR 2
#define SCOUT 3
#define PIKE 4
#define SWORDS 5
#define ARCHER 6
#define CAVALRY 7
#define CATAPHRACT 8
#define TRANSPORTER 9
#define RAM 10
#define CATAPULT 11

#define DEF_ALLIANCE 0
#define DEF_ALLY 1
#define DEF_NEUTRAL 2
#define DEF_ENEMY 3
#define DEF_NORELATION 4
#define DEF_SELFRELATION 5

#define DEF_ALLIANCEHOST 4
#define DEF_ALLIANCEVICEHOST 5
#define DEF_ALLIANCEPRES 6
#define DEF_ALLIANCEOFFICER 7
#define DEF_ALLIANCEMEMBER 8


#define DEF_HEROIDLE 0
#define DEF_HEROMAYOR 1
#define DEF_HEROREINFORCE //exists?
#define DEF_HEROATTACK 3
#define DEF_HEROSEIZED 4
#define DEF_HEROSCOUT //exists?
#define DEF_HERORETURN 5

#define MISSION_TRANS 1
#define MISSION_REINF 2
#define MISSION_SCOUT 3
#define MISSION_BUILDCITY 4
#define MISSION_ATTACK 5

#define DIRECTION_FORWARD 1
#define DIRECTION_BACKWARD 2
#define DIRECTION_STAY 3

// BUILDING IDS

#define B_COTTAGE 1
#define B_BARRACKS 2
#define B_WAREHOUSE 3
#define B_SAWMILL 4
#define B_STONEMINE 5
#define B_IRONMINE 6
#define B_FARM 7
#define B_STABLE 20
#define B_INN 21
#define B_FORGE 22
#define B_MARKETPLACE 23
#define B_RELIEFSTATION 24
#define B_ACADEMY 25
#define B_WORKSHOP 26
#define B_FEASTINGHALL 27
#define B_EMBASSY 28
#define B_RALLYSPOT 29
#define B_BEACONTOWER 30
#define B_TOWNHALL 31
#define B_WALLS 32

// TROOP IDS
#define TR_WORKER 2
#define TR_WARRIOR 3
#define TR_SCOUT 4
#define TR_PIKE 5
#define TR_SWORDS 6
#define TR_ARCHER 7
#define TR_TRANSPORTER 8
#define TR_CAVALRY 9
#define TR_CATAPHRACT 10
#define TR_BALLISTA 11
#define TR_RAM 12
#define TR_CATAPULT 13

#define TR_TRAP 14
#define TR_ABATIS 15
#define TR_ARCHERTOWER 16
#define TR_ROLLINGLOG 17
#define TR_TREBUCHET 18

#define MAIL_INBOX 1
#define MAIL_SYSTEM 2
#define MAIL_SENT 3


// RESEARCH IDS
#define T_AGRICULTURE 1
#define T_LUMBERING 2
#define T_MASONRY 3
#define T_MINING 4
#define T_METALCASTING 5
#define T_INFORMATICS 7
#define T_MILITARYSCIENCE 8
#define T_MILITARYTRADITION 9
#define T_IRONWORKING 10
#define T_LOGISTICS 11
#define T_COMPASS 12
#define T_HORSEBACKRIDING 13
#define T_ARCHERY 14
#define T_STOCKPILE 15
#define T_MEDICINE 16
#define T_CONSTRUCTION 17
#define T_ENGINEERING 18
#define T_MACHINERY 19
#define T_PRIVATEERING 20


#define GETX xfromid
#define GETY yfromid
#define GETID idfromxy

#define GETXYFROMID4(b,a,c,d) a = short(c / d); b = short(c % d);//x, y, id, mapsize

#define GETXYFROMID(a) short yfromid = short(a / mapsize); short xfromid = short(a % mapsize);

#define GETIDFROMXY(x,y) int idfromxy = y*mapsize+x;

#define DEF_MAXITEMS 400

#define DEF_STATES 16

#define DEF_STATE1 "FRIESLAND"
#define DEF_STATE2 "SAXONY"
#define DEF_STATE3 "NORTH MARCH"
#define DEF_STATE4 "BOHEMIA"
#define DEF_STATE5 "LOWER LORRAINE"
#define DEF_STATE6 "FRANCONIA"
#define DEF_STATE7 "THURINGIA"
#define DEF_STATE8 "MORAVIA"
#define DEF_STATE9 "UPPER LORRAINE"
#define DEF_STATE10 "SWABIA"
#define DEF_STATE11 "BAVARIA"
#define DEF_STATE12 "CARINTHIA"
#define DEF_STATE13 "BURGUNDY"
#define DEF_STATE14 "LOMBARDY"
#define DEF_STATE15 "TUSCANY"
#define DEF_STATE16 "ROMAGNA"

inline void my_split(std::vector<std::string> & toks, const std::string & src, const std::string & anyof)
{
    std::string target;
    std::string::size_type n, nxt, offset = 0;
    toks.clear();
    if (anyof.size() == 0)
        return;
    while (true)
    {
        n = src.find_first_not_of(anyof[0], offset);
        if (n != std::string::npos)//non-delim found
        {
            nxt = src.find_first_of(anyof[0], n + 1);
            if (nxt != std::string::npos)//found another delim
            {
                toks.push_back(src.substr(n, nxt - n));
                offset = nxt;
            }
            else//no delim found, end of content
            {
                toks.push_back(src.substr(n, nxt - n));
                break;
            }
        }
        else//nothing else found
            break;
    }
}

// #define SQLCATCH(a)    catch (std::exception & e)\
// {\
//     std::string file = __FILE__;\
//     log->error("std::exception: {} {} {}", file, __LINE__, e.what()); \
// }

// #define SQLCATCH2(a, b)    catch (std::exception & e)\
// {\
//     std::string file = __FILE__;\
//     b->log->error("std::exception: {} {} {}", file, __LINE__, e.what()); \
// }

// #define SQLCATCH3(a, b)    catch (std::exception & e)\
// {\
//     std::string file = __FILE__;\
//     b.log->error("std::exception: {} {} {}", file, __LINE__, e.what()); \
// }

#ifdef WIN32
#define _SFFUNC_ __FUNCSIG__
#else
#define _SFFUNC_ __PRETTY_FUNCTION__
#endif


#define SQLCATCH(a) __SQLCATCH(_SFFUNC_, a)
#define SQLCATCH2(a, b) __SQLCATCH2(_SFFUNC_, a, b)
#define SQLCATCH3(a, b) __SQLCATCH3(_SFFUNC_, a, b)


#define __SQLCATCH(b, a)    catch (Poco::Data::MySQL::ConnectionException& e)\
{\
    log->error("ConnectionException: {} {} {} {}", __FILE__, b, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::Data::MySQL::StatementException& e)\
{\
    log->error("StatementException: {} {} {} {}", __FILE__, b, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::Data::MySQL::MySQLException& e)\
{\
    log->error("MySQLException: {} {} {} {}", __FILE__, b, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::InvalidArgumentException& e)\
{\
    log->error("InvalidArgumentException: {} {} {} {}", __FILE__, b, __LINE__, e.displayText());\
    a; \
}\
    catch (std::exception & e)\
{\
    log->error("std::exception: {} {} {} {}", __FILE__, b, __LINE__, e.what()); \
}

#define __SQLCATCH2(c, a, b)    catch (Poco::Data::MySQL::ConnectionException& e)\
{\
    b->log->error("ConnectionException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::Data::MySQL::StatementException& e)\
{\
    b->log->error("StatementException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::Data::MySQL::MySQLException& e)\
{\
    b->log->error("MySQLException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::InvalidArgumentException& e)\
{\
    b->log->error("InvalidArgumentException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (std::exception & e)\
{\
    b->log->error("std::exception: {} {} {} {}", __FILE__, c, __LINE__, e.what()); \
}

#define __SQLCATCH3(c, a, b)    catch (Poco::Data::MySQL::ConnectionException& e)\
{\
    b.log->error("ConnectionException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::Data::MySQL::StatementException& e)\
{\
    b.log->error("StatementException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::Data::MySQL::MySQLException& e)\
{\
    b.log->error("MySQLException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (Poco::InvalidArgumentException& e)\
{\
    b.log->error("InvalidArgumentException: {} {} {} {}", __FILE__, c, __LINE__, e.displayText());\
    a; \
}\
    catch (std::exception & e)\
{\
    b.log->error("std::exception: {} {} {} {}", __FILE__, c, __LINE__, e.what()); \
}


#define SOCKET_SERVER 2

//#define DEF_MAXCLIENTS 1024
#define DEF_MAXPHP 100
#define DEF_MAXALLIANCES 1000
#define DEF_MAXITEMS 400

#define DEF_LISTENSOCK 1
#define DEF_NORMALSOCK 2

#ifndef WIN32
#define strtok_s strtok_r
#define _atoi64 atoll
#define sprintf_s snprintf
#define strcpy_s(a,b,c) strcpy(a,c)
#endif

#define KeyExists(x,y) ((x._object->Exists(y))>=0)
#define IsString(x) (x.type==String)
#define IsObject(x) (x.type==Object)

#define LAST_INSERT_ID (std::string)"LAST_INSERT_ID()"

