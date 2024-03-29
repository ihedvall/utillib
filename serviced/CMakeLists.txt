# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(serviced
        VERSION 1.0
        DESCRIPTION "Service runs applications"
        LANGUAGES CXX)

add_executable(serviced src/serviced.cpp src/servicehelper.cpp src/servicehelper.h)


target_include_directories(serviced PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(serviced PRIVATE "../../utillib/include")
target_compile_definitions(serviced PRIVATE XML_STATIC)

if (MINGW)
    target_link_options(serviced PRIVATE -static)
endif ()

if (MSVC)
    target_compile_definitions(serviced PRIVATE _WIN32_WINNT=0x0A00)
endif ()

target_link_libraries(serviced PRIVATE util)
target_link_libraries(serviced PRIVATE EXPAT::EXPAT)
target_link_libraries(serviced PRIVATE ${Boost_LIBRARIES})
if (WIN32)
    #target_link_libraries(serviced PRIVATE iconv)
    target_link_libraries(serviced PRIVATE bcrypt)
    target_link_libraries(serviced PRIVATE ws2_32)
    target_link_libraries(serviced PRIVATE mswsock)
endif ()
