# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT

project(TestUtil
        VERSION 1.0
        DESCRIPTION "Google unit tests for the util library"
        LANGUAGES CXX)

add_executable(test_util
        test_logfile.cpp
        test_timestamp.cpp
        test_logging.cpp
        test_xml.cpp
        teststringutil.cpp
        test_message_queue.cpp
        testlisten.cpp testlisten.h
        test_tempdir.cpp
        testsyslogmessage.cpp
        testsyslog.cpp
        test_syslogflexer.cpp
        test_base64.cpp test_unithelper.cpp test_idirectory.cpp testsyslog.h test_serial.cpp)

target_include_directories(test_util PRIVATE ../include)
target_include_directories(test_util PRIVATE ../src)
target_include_directories(test_util PRIVATE ${GTEST_INCLUDE_DIRS})
target_include_directories(test_util PRIVATE ${Boost_INCLUDE_DIRS})



target_link_libraries(test_util PRIVATE util)
target_link_libraries(test_util PRIVATE ${Boost_LIBRARIES})
target_link_libraries(test_util PRIVATE EXPAT::EXPAT)
target_link_libraries(test_util PRIVATE ${GTEST_BOTH_LIBRARIES})

if (WIN32)
    target_link_libraries(test_util PRIVATE ws2_32)
    target_link_libraries(test_util PRIVATE mswsock)
    target_link_libraries(test_util PRIVATE bcrypt)
    target_link_libraries(test_util PRIVATE setupapi)
endif ()

if (MINGW)
    target_link_options(test_util PRIVATE -static -fstack-protector)
elseif (MSVC)
    target_compile_options(test_util PRIVATE -D_WIN32_WINNT=0x0A00)
endif ()


include(GoogleTest)
gtest_discover_tests(test_util)

