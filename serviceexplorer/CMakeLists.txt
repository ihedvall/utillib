# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(ServiceExplorer
        VERSION 1.0
        DESCRIPTION "Service Explorer for viewing Windows services"
        LANGUAGES CXX)

add_executable(serviceexplorer
        src/serviceexplorer.cpp src/serviceexplorer.h
        src/mainframe.cpp src/mainframe.h
        src/serviceexplorer.rc
        src/serviceexplorerid.h
        src/serviceinfo.cpp src/serviceinfo.h
        src/servicedialog.cpp src/servicedialog.h src/servicehelper.cpp src/servicehelper.h)


target_include_directories(serviceexplorer PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(serviceexplorer PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(serviceexplorer PRIVATE "../../utillib/include")

target_compile_definitions(serviceexplorer PRIVATE XML_STATIC)

if (MINGW)
target_link_options(serviceexplorer PRIVATE -mwindows -static)
endif()

if (MSVC)
    target_link_options(serviceexplorer PRIVATE /SUBSYSTEM:WINDOWS)
    target_compile_definitions(serviceexplorer PRIVATE _WIN32_WINNT=0x0A00)
endif()

target_link_libraries(serviceexplorer PRIVATE util)
target_link_libraries(serviceexplorer PRIVATE EXPAT::EXPAT)
target_link_libraries(serviceexplorer PRIVATE wxWidgets::wxWidgets)
target_link_libraries(serviceexplorer PRIVATE ${Boost_LIBRARIES})


if (WIN32)
#target_link_libraries(serviceexplorer PRIVATE iconv)
target_link_libraries(serviceexplorer PRIVATE bcrypt)
target_link_libraries(serviceexplorer PRIVATE ws2_32)
target_link_libraries(serviceexplorer PRIVATE mswsock)
endif()
