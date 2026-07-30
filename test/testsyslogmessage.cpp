/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>

#include <any>
#include <boost/asio/ip/host_name.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "util/logconfig.h"
#include "util/syslogmessage.h"

using namespace util::syslog;
using namespace util::log;

namespace {
constexpr std::string_view kHostName = "ACME_11";
constexpr std::string_view kAppName = "Test_SyslogMessage";
}

namespace util::test {

TEST(SyslogMessage, Properties) {
  auto& log_config = LogConfig::Instance();
  log_config.HostName(std::string(kHostName));
  log_config.ApplicationName(std::string(kAppName));

  SyslogMessage msg;

  EXPECT_EQ(msg.Index(), 0);
  msg.Index(11);
  EXPECT_EQ(msg.Index(), 11);

  EXPECT_EQ(msg.Severity(), SyslogSeverity::Informational);
  msg.Severity(SyslogSeverity::Alert);
  EXPECT_EQ(msg.Severity(), SyslogSeverity::Alert);

  EXPECT_EQ(msg.Facility(), SyslogFacility::Local0);
  msg.Facility(SyslogFacility::Local1);
  EXPECT_EQ(msg.Facility(), SyslogFacility::Local1);

  EXPECT_EQ(msg.Version(), 1);
  msg.Version(2);
  EXPECT_EQ(msg.Version(), 2);

  EXPECT_GT(msg.Timestamp(), 0);
  msg.Timestamp(0);
  EXPECT_EQ(msg.Timestamp(), 0);

  EXPECT_EQ(msg.Hostname(), kHostName);
  msg.Hostname("Road Runner");
  EXPECT_EQ(msg.Hostname(), "Road_Runner");

  EXPECT_EQ(msg.ApplicationName(), kAppName);
  msg.ApplicationName("Coyote");
  EXPECT_EQ(msg.ApplicationName(), "Coyote");

  EXPECT_FALSE(msg.ProcessId().empty());
  msg.ProcessId("666");
  EXPECT_EQ(msg.ProcessId(), "666");

  EXPECT_TRUE(msg.MessageId().empty());
  msg.MessageId("999");
  EXPECT_EQ(msg.MessageId(), "999");

  EXPECT_TRUE(msg.Message().empty());
  msg << "ACME Tar Ball Failure";
  EXPECT_EQ(msg.Message(), "ACME Tar Ball Failure" );

  msg.AddSourceLocation(std::source_location::current());
  EXPECT_EQ(msg.DataList().size(), 1);

  const std::string out = msg.GenerateMessage();
  std::cout << out << std::endl;

  SyslogMessage msg1;
  msg1.ParseMessage(out);
  const std::string out1 = msg1.GenerateMessage();
  EXPECT_EQ(out, out1) << out1;

}

TEST(SyslogMessage, BasicUse) {  // NOLINT
  auto& log_config = LogConfig::Instance();
  log_config.HostName(std::string(kHostName));
  log_config.ApplicationName(std::string(kAppName));

  SyslogMessage msg;
  msg.Message("What happened!");
  msg.MessageId("OLLE23");

  StructuredData data;
  data.Identity("loginfo");
  data.AddParameter("olle", "189");
  msg.AddData(data);

  const std::string out = msg.GenerateMessage();
  std::cout << out << std::endl;
  EXPECT_FALSE(out.empty());

  SyslogMessage msg1;
  msg1.ParseMessage(out);

  EXPECT_EQ(msg.MessageId(), msg1.MessageId());
  EXPECT_EQ(msg.Message(), msg1.Message());
  ASSERT_EQ(msg1.DataList().size(), 1);
  const auto& data1 = msg.DataList().front();
  EXPECT_EQ(data1.Parameters().size(), 1);
}

TEST(SyslogMessage, LogUse) {
  auto& log_config = LogConfig::Instance();
  log_config.ApplicationName("SysLogMessage::LogUse");

  const auto loc = std::source_location::current();
  LogMessage log;
  log.line = loc.line();
  log.column = loc.column();
  log.file = loc.file_name();
  log.function = loc.function_name();
  log.message = "Testing log message";
  log.severity = LogSeverity::kWarning;

  SyslogMessage msg(log, false);

  const auto out = msg.GenerateMessage();
  std::cout << out << std::endl;
  EXPECT_FALSE(out.empty());
}

TEST(SyslogMessage, AnyTest) {
  SyslogMessage original;

  auto ptr = std::make_unique<SyslogMessage>();
  using SyslogList = std::vector<SyslogMessage>;
  SyslogList list;

  auto holder = std::make_any<SyslogList>();
}

TEST(SyslogMessage, TestParser) {
  constexpr std::array<std::string_view, 1> test_message_list = {
    "<134>1 2026-07-30T16:57:17.241528Z DESKTOP-8J6AV3O SYSLOG 4152 - - Testing 0",
  };
  for (const auto& test_message : test_message_list) {
     SyslogMessage msg;
     const bool parse = msg.ParseMessage(std::string(test_message));
    EXPECT_TRUE(parse) << test_message;
  }

}
}  // namespace util::test