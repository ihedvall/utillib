/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <thread>

#include "util/logconfig.h"
#include "util/logging.h"
#include "util/logstream.h"
#include "util/logtolist.h"

using namespace util::log;

namespace {

std::atomic<int> jj = 0;

void TestLogInfo(int ii) {
  LOG_TRACE() << "Pelle: " << ii;
  LOG_INFO() << "Olle: " << ii;
  LOG_ERROR() << "Olle: " << ii;
  LogInfo(Loc::current(), "Pelle %d", ii);
  LogError(Loc::current(), "Pelle: %d", ii);
}

void TestThreadFunction() {
  while (jj < 1000) {
    int ii = jj++;
    LOG_INFO() << "Nisse: " << ii;
  }
}

}  // namespace

namespace util::test {
TEST(Logging, Console)  // NOLINT
{
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();
  for (int ii = 0; ii < 10; ++ii) TestLogInfo(ii);
  log_config.DeleteLogChain();
}

TEST(Logging, ConsoleMutiThread)  // NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();
  std::array<std::thread, 100> thread_list;
  for (auto &t : thread_list) {
    t = std::thread(TestThreadFunction);
  }
  for (auto &t : thread_list) {
    t.join();
  }
  log_config.DeleteLogChain();
}

TEST(Logging, ConsolePerformance)  // NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  TestThreadFunction();  // Reuse of the threading function

  log_config.DeleteLogChain();
}

TEST(Logging, LogToFile)  // NOLINT
{
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing/log");
  log_config.CreateDefaultLogger();
  for (int ii = 0; ii < 10; ++ii) TestLogInfo(ii);
  log_config.DeleteLogChain();
}

TEST(Logging, LogToFileMutiThread)  // NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing/log");
  log_config.CreateDefaultLogger();
  std::array<std::thread, 100> thread_list;
  for (auto &t : thread_list) {
    t = std::thread(TestThreadFunction);
  }
  for (auto &t : thread_list) {
    t.join();
  }
  log_config.DeleteLogChain();
}

TEST(Logging, DISABLED_LogToFilePerformance)  // NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing/log");
  log_config.CreateDefaultLogger();

  TestThreadFunction();  // Reuse of the threading function

  log_config.DeleteLogChain();
}

TEST(Logging, DISABLED_LogToFileBackup)  // NOLINT
{
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing/log");
  log_config.CreateDefaultLogger();
  for (int ii = 0; ii < 1000'000; ++ii) TestLogInfo(ii);

  log_config.DeleteLogChain();
}

TEST(Logging, Test_LogToList) {
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToList);
  log_config.CreateDefaultLogger();

  auto *logger = log_config.GetLogger("Default");
  ASSERT_TRUE(logger != nullptr);
  auto *list_logger = dynamic_cast<LogToList *>(logger);
  ASSERT_TRUE(list_logger != nullptr);
  list_logger->MaxSize(10);
  EXPECT_EQ(list_logger->MaxSize(), 10);

  EXPECT_EQ(list_logger->Size(), 0);
  auto change_number = list_logger->ChangeNumber();
  for (size_t index = 0; index < 15; ++index) {
    LogMessage msg;
    msg.message = "Test " + std::to_string(index);
    list_logger->AddLogMessage(msg);
    if (list_logger->Size() < 10) {
      EXPECT_EQ(list_logger->Size(), index + 1);
    } else {
      EXPECT_EQ(list_logger->Size(), 10);
    }
    EXPECT_NE(list_logger->ChangeNumber(), change_number);
    change_number = list_logger->ChangeNumber();
  }
  
  LogMessage latest_msg = list_logger->GetLogMessage(0);
  EXPECT_EQ(latest_msg.message, "Test 14");
  EXPECT_EQ(list_logger->Size(), 10);

  log_config.DeleteLogChain();
}

}  // namespace util::test