# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(utilLib
        VERSION 2.0
        DESCRIPTION "Common C++ library with utilities."
        LANGUAGES CXX)

if("${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
    set(USE_VCPKG OFF)
else()
    set(USE_VCPKG ON)
endif()

option(BUILD_SHARED_LIBS "Static libraries are preferred" OFF)
option(UTIL_DOC "If doxygen is installed, then build documentation in Release mode" OFF)
option(UTIL_TOOLS "Building applications" ON)
option(UTIL_TEST "Building unit test" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX d)

include(CMakePrintHelpers)

include(script/boost.cmake)
include(script/expat.cmake)
#include(script/fetch_expat.cmake)

if (UTIL_TOOLS)
    if (USE_VCPKG)
        list(APPEND VCPKG_MANIFEST_FEATURES "tools")
     endif()
    include(script/wxwidgets.cmake)
endif()

if (UTIL_TEST)
    if (USE_VCPKG)
        list(APPEND VCPKG_MANIFEST_FEATURES "tests")
    endif()
    include(script/googletest.cmake)
endif()

if (UTIL_DOC)
    include(script/doxygen.cmake)
endif()

include(script/flex.cmake)
include(script/bison.cmake)

if (BISON_FOUND)
    BISON_TARGET(SYSLOG_PARSER src/syslogparser.y ${CMAKE_CURRENT_SOURCE_DIR}/src/syslogparser.cpp)
    cmake_print_variables(BISON_SYSLOG_PARSER_DEFINED
            BISON_SYSLOG_PARSER_INPUT
            BISON_SYSLOG_PARSER_OUTPUT_SOURCE
            BISON_SYSLOG_PARSER_OUTPUT_HEADER
            BISON_SYSLOG_PARSER_OUTPUTS
            BISON_SYSLOG_PARSER_COMPILE_FLAGS)
endif ()

if (FLEX_FOUND)
    FLEX_TARGET(SYSLOG_SCANNER src/syslogflexer.l ${CMAKE_CURRENT_SOURCE_DIR}/src/syslogflexer.cpp)
    cmake_print_variables(FLEX_SYSLOG_SCANNER_DEFINED
            FLEX_SYSLOG_SCANNER_OUTPUTS
            FLEX_SYSLOG_SCANNER_INPUT
            FLEX_SYSLOG_SCANNER_OUTPUT_HEADER)
# Copy FlexLexer.h. This is a dirty trick to avoid flex/bison installation in windows build
    file(COPY_FILE ${FLEX_INCLUDE_DIRS}/FlexLexer.h ${CMAKE_CURRENT_SOURCE_DIR}/src/FlexLexer.h)
endif ()

if (FLEX_FOUND AND BISON_FOUND)
    ADD_FLEX_BISON_DEPENDENCY(SYSLOG_SCANNER SYSLOG_PARSER)
endif()

add_library(util
        src/logfile.cpp src/logfile.h
        include/util/timestamp.h src/timestamp.cpp
        include/util/logconfig.h src/logconfig.cpp
        include/util/logging.h src/logging.cpp
        src/logconsole.cpp src/logconsole.h
        include/util/logmessage.h src/logmessage.cpp
        include/util/logstream.h src/logstream.cpp
        src/expatxml.h src/expatxml.cpp
        src/xmlnode.h src/xmlnode.cpp
        include/util/ixmlnode.h src/ixmlnode.cpp
        include/util/ixmlfile.h src/ixmlfile.cpp
        include/util/stringutil.h src/stringutil.cpp
        include/util/ilogger.h
        include/util/csvwriter.h src/csvwriter.cpp
        src/messagequeue.cpp src/messagequeue.h
        src/listenmessage.h src/listenmessage.cpp
        src/ilisten.cpp include/util/ilisten.h
        src/listenserver.cpp src/listenserver.h
        src/listenproxy.cpp src/listenproxy.h
        include/util/threadsafequeue.h
        src/listenserverconnection.h src/listenserverconnection.cpp
        src/listenconfig.cpp include/util/listenconfig.h
        src/listenclient.cpp src/listenclient.h
        src/listenlogger.cpp src/listenlogger.h
        src/writexml.cpp src/writexml.h
        src/stringparser.cpp include/util/stringparser.h
        src/gnuplot.cpp include/util/gnuplot.h
        src/tempdir.cpp include/util/tempdir.h
        src/syslogmessage.cpp include/util/syslogmessage.h
        src/syslog.cpp src/syslog.h src/ilogger.cpp
        src/syslogparser.cpp src/syslogparser.hpp
        src/syslogflexer.cpp src/FlexLexer.h
        src/syslogscanner.cpp src/syslogscanner.h
        src/structureddata.cpp include/util/structureddata.h
        src/isyslogserver.cpp include/util/isyslogserver.h
        src/udpsyslogserver.cpp src/udpsyslogserver.h
        src/utilfactory.cpp include/util/utilfactory.h
        src/listenconsole.cpp src/listenconsole.h
        src/unithelper.cpp include/util/unithelper.h
        src/idirectory.cpp include/util/idirectory.h
        src/ifile.cpp include/util/ifile.h
        src/syslogpublisher.cpp src/syslogpublisher.h
        src/syslogconnection.cpp src/syslogconnection.h
        src/syslogsubscriber.cpp src/syslogsubscriber.h
        src/tcpsyslogserver.cpp src/tcpsyslogserver.h
        src/ilistenclient.cpp include/util/ilistenclient.h
        src/serialportinfo.cpp include/util/serialportinfo.h
        src/serialportinfowin.inc)

#get_target_property(PAHO_C_INCLUDE_DIRS eclipse-paho-mqtt-c::paho-mqtt3as-static INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(util PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
target_include_directories(util PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(util PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(util PRIVATE ${EXPAT_INCLUDE_DIRS})



cmake_print_properties(TARGETS util PROPERTIES INCLUDE_DIRECTORIES)

target_compile_definitions(util PRIVATE XML_STATIC)

if (MSVC)
    target_compile_definitions(util PRIVATE _WIN32_WINNT=0x0A00)
endif ()

set(UTIL_PUBLIC_HEADERS
        include/util/csvwriter.h
        include/util/gnuplot.h
        include/util/idirectory.h
        include/util/ifile.h
        include/utililisten.h
        include/util/ilistenclient.h
        include/util/ilogger.h
        include/util/isyslogserver.h
        include/util/ixmlfile.h
        include/util/ixmlnode.h
        include/util/listenconfig.h
        include/util/logconfig.h
        include/util/logging.h
        include/util/logmessage.h
        include/util/logstream.h
        include/util/serialportinfo.h
        include/util/stringparser.h
        include/util/stringutil.h
        include/util/structureddata.h
        include/util/syslogmessage.h
        include/util/tempdir.h
        include/util/threadsafequeue.h
        include/util/timestamp.h
        include/util/unithelper.h
        include/util/utilfactory.h
        )

set_target_properties(util PROPERTIES PUBLIC_HEADER "${UTIL_PUBLIC_HEADERS}")
get_target_property(PH util PUBLIC_HEADER)
message(STATUS "UTIL Target Includes: " "${PH}")
if (UTIL_TOOLS)
    add_subdirectory(listend)
endif ()

if (WIN32 AND UTIL_TOOLS)
    add_subdirectory(serviced)
endif ()

if (wxWidgets_FOUND AND UTIL_TOOLS)
    add_subdirectory(listenviewer)
endif ()

if (wxWidgets_FOUND AND WIN32 AND UTIL_TOOLS)
    add_subdirectory(serviceexplorer)
endif ()

if (GTEST_FOUND AND UTIL_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()

if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") AND UTIL_DOC)
    set(DOXYGEN_RECURSIVE NO)
    set(DOXYGEN_REPEAT_BRIEF NO)
    set(DOXYGEN_PROJECT_NAME "UTILLib")
    set(DOXYGEN_HTML_EXTRA_STYLESHEET doxygen/utillib.css)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/manual)
    doxygen_add_docs(doc_libutil ALL
            include/util doxygen
            COMMENT "Util Library generating API documentation with Doxygen")
endif ()

install(TARGETS util
        EXPORT UtilLibTargets
        LIBRARY DESTINATION util/lib
        ARCHIVE DESTINATION util/lib
        RUNTIME DESTINATION util/bin
        PUBLIC_HEADER DESTINATION util/include/util)

#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#        UtilLibConfigVersion.cmake
#        VERSION ${PACKAGE_VERSION}
#        COMPATIBILITY AnyNewerVersion)
#
#install(EXPORT UtilLibTargets
#        FILE UtilLibTargets.cmake
#        NAMESPACE UtilLib::
#        DESTINATION lib/cmake/UtilLib
#        )

#export(TARGETS util NAMESPACE UtilLib:: FILE UtilLibTargets.cmake)