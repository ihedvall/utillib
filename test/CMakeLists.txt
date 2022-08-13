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
        testzlib.cpp testzlib.h
        testcrypto.cpp
        teststringutil.cpp
        test_message_queue.cpp
        testlisten.cpp testlisten.h
        test_tempdir.cpp
        testsyslogmessage.cpp
        test_syslog.cpp
        test_syslogflexer.cpp
        test_base64.cpp
        test_mqtt.cpp)

target_include_directories(test_util PRIVATE ../include)
target_include_directories(test_util PRIVATE ../src)
target_include_directories(test_util PRIVATE ${GTEST_INCLUDE_DIRS})
target_include_directories(test_util PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(test_util PRIVATE ${FLEX_INCLUDE_DIRS})


target_link_libraries(test_util PRIVATE util)
target_link_libraries(test_util PRIVATE ${Boost_LIBRARIES})
target_link_libraries(test_util PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(test_util PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(test_util PRIVATE ${GTEST_BOTH_LIBRARIES})
target_link_libraries(test_util PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(test_util PRIVATE eclipse-paho-mqtt-c::paho-mqtt3as-static)

if (WIN32)
target_link_libraries(test_util PRIVATE ws2_32)
target_link_libraries(test_util PRIVATE mswsock)
target_link_libraries(test_util PRIVATE bcrypt)
endif()

if (MINGW)
    target_link_options(test_util PRIVATE -static -fstack-protector )
elseif (MSVC)
    target_compile_options(test_util PRIVATE -D_WIN32_WINNT=0x0A00)
endif()


include(GoogleTest)
gtest_discover_tests(test_util)

