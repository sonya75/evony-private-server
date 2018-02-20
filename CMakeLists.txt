cmake_minimum_required(VERSION 2.8)
project(spitfire)

file(GLOB_RECURSE sources src/*.cpp src/*.h)

add_executable(spitfire ${sources})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")

set(Poco_DIR ${CMAKE_SOURCE_DIR}/lib/poco-1.8.1-all)
set(Poco_LIBRARY_DIR ${Poco_DIR}/lib)

ADD_DEFINITIONS(-DASIO_STANDALONE -DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB)

target_include_directories(spitfire PUBLIC src)

include_directories(${Poco_DIR}/Foundation/include ${Poco_DIR}/Crypto/include ${Poco_DIR}/Net/include ${Poco_DIR}/Data/include ${Poco_DIR}/Data/MySQL/include)
link_directories(${Poco_LIBRARY_DIR})

MESSAGE( STATUS "MAKE_SOURCE_DIR     " ${CMAKE_SOURCE_DIR} )
MESSAGE( STATUS "Poco_LIBRARY_DIR:   " ${Poco_LIBRARY_DIR} )

target_link_libraries(spitfire pthread ${Poco_LIBRARY_DIR}/libPocoFoundation.so ${Poco_LIBRARY_DIR}/libPocoData.so ${Poco_LIBRARY_DIR}/libPocoDataMySQL.so ssl crypto z
