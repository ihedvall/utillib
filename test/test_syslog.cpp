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
#include "util/utilfactory.h"
#include "../src/syslog.h"

using namespace util::log;
using namespace util::log::detail;
using namespace std::chrono_literals;
using namespace util::syslog;

namespace util::test {

TEST(SysLog, StartAndStopServer) {
  auto syslog_server = UtilFactory::CreateSyslogServer(SyslogServerType::UdpServer);
  syslog_server->Port(6969);
  syslog_server->Start();
  std::this_thread::sleep_for(1s);
  syslog_server->Stop();
}


TEST(SysLog, SendToServer) {
  auto syslog_server = UtilFactory::CreateSyslogServer(SyslogServerType::UdpServer);
  syslog_server->Port(6969);
  syslog_server->Start();

    // Test normal usage
  auto &log_config = LogConfig::Instance();
  const std::vector<std::string> arg_list = {"localhost", "6969"};
  log_config.AddLogger("Default", LogType::LogToSyslog, arg_list);
  log_config.ApplicationName("SYSLOG");

  auto *logger = dynamic_cast<Syslog *>(log_config.GetLogger("Default"));
  ASSERT_TRUE(logger != nullptr);
  logger->ShowLocation(false);
  for (int test = 0; test < 10; ++test) {
    LOG_INFO() << "Testing " << test;
  }

  for(size_t count = 0; count < 100; ++count) {
    if (syslog_server->NofMessages() < 10) {
      std::this_thread::sleep_for(100ms);
    }
  }
  EXPECT_GE(syslog_server->NofMessages(), 10);
  for (auto msg = syslog_server->GetMsg(false); msg.has_value(); msg = syslog_server->GetMsg(false)) {
    std::cout << msg.value().GenerateMessage() << std::endl;
  }

  log_config.DeleteLogChain();
  syslog_server->Stop();
}


} // end namespace