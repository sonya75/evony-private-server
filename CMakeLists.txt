cmake_minimum_required(VERSION 2.8)
project(spitfire)

file(GLOB_RECURSE sources src/*.cpp src/*.h)

add_executable(spitfire ${sources})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -Wall")

set(Poco_DIR ${CMAKE_SOURCE_DIR}/lib/poco-1.8.1)
set(Poco_LIBRARY_DIR ${Poco_DIR}/lib)

ADD_DEFINITIONS(-DASIO_STANDALONE -DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB)

target_include_directories(spitfire PUBLIC src)

include_directories(${Poco_DIR}/Foundation/include ${Poco_DIR}/Crypto/include)

link_directories(${Poco_LIBRARY_DIR})

MESSAGE( STATUS "MAKE_SOURCE_DIR     " ${CMAKE_SOURCE_DIR} )
MESSAGE( STATUS "Poco_LIBRARY_DIR:   " ${Poco_LIBRARY_DIR} )

target_link_libraries(spitfire pthread ${Poco_LIBRARY_DIR}/libPocoFoundation.so ${Poco_LIBRARY_DIR}/libPocoData.so ${Poco_LIBRARY_DIR}/libPocoDataMySql.so ssl crypto z)
