/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <iostream>
#include <chrono>

#include <gtest/gtest.h>
#include "util/logconfig.h"
#include "util/logstream.h"



#include "../src/syslog.h"
using namespace util::log;
using namespace util::log::detail;
using namespace std::chrono_literals;

namespace util::test {

TEST(SysLog, SendToServer) {
  try {
    // Test normal usage
    auto &log_config = LogConfig::Instance();
    log_config.Type(LogType::LogToSyslog);
    log_config.CreateDefaultLogger();
    log_config.ApplicationName("SYSLOG");

    auto *logger = dynamic_cast<Syslog *>(log_config.GetLogger("Default"));
    ASSERT_TRUE(logger != nullptr);
    logger->ShowLocation(false);
    for (int test = 0; test < 10; ++test) {
      LOG_INFO() << "Testing " << test;
    }


    std::this_thread::sleep_for(4s);
    log_config.DeleteLogChain();

  } catch (const std::exception &error) {
    GTEST_FAIL() << error.what() << std::endl;
  }
}


} // end namespace