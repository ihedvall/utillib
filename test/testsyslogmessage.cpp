/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>
#include "util/syslogmessage.h"
#include "util/logconfig.h"

using namespace util::syslog;
using namespace util::log;

namespace util::test {

TEST(SyslogMessage, BasicUse) { // NOLINT
  auto& log_config = LogConfig::Instance();
  log_config.ApplicationName("SysLogMessage::BasicUse");

  SyslogMessage msg;
  msg.Message("What happened!");
  msg.MessageId("OLLE23");

  StructuredData data;
  data.Identity( "loginfo");
  data.AddParameter("olle", "189");
  msg.AddData(data);

  const auto out = msg.GenerateMessage();
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

TEST(SyslogMessage, LogUse) { // NOLINT
  auto& log_config = LogConfig::Instance();
  log_config.ApplicationName("SysLogMessage::LogUse");

  LogMessage log;
  log.location = Loc::current();
  log.message = "Testing log message";
  log.severity = LogSeverity::kWarning;

  SyslogMessage msg(log, false);

  const auto out = msg.GenerateMessage();
  std::cout << out << std::endl;
  EXPECT_FALSE(out.empty());
}
} // end namespace