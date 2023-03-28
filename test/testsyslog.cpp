/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "testsyslog.h"

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>

#include "../src/syslog.h"
#include "util/logconfig.h"
#include "util/logstream.h"
#include "util/utilfactory.h"

using namespace util::log;
using namespace util::log::detail;
using namespace std::chrono_literals;
using namespace util::syslog;

namespace util::test {

void TestSyslog::SetUpTestCase() {
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();
}

void TestSyslog::TearDownTestCase() {
  auto &log_config = LogConfig::Instance();
  log_config.DeleteLogChain();
}

TEST_F(TestSyslog, StartAndStopUdpServer) {
  auto syslog_server =
      UtilFactory::CreateSyslogServer(SyslogServerType::UdpServer);
  syslog_server->Port(6969);
  syslog_server->Start();
  std::this_thread::sleep_for(1s);
  syslog_server->Stop();
}

TEST_F(TestSyslog, SendToUdpServer) {
  auto syslog_server =
      UtilFactory::CreateSyslogServer(SyslogServerType::UdpServer);
  syslog_server->Port(6969);
  syslog_server->Start();

  // Test normal usage
  auto &log_config = LogConfig::Instance();
  const std::vector<std::string> arg_list = {"localhost", "6969"};
  log_config.AddLogger("Syslog", LogType::LogToSyslog, arg_list);
  log_config.ApplicationName("SYSLOG");

  auto *logger = dynamic_cast<Syslog *>(log_config.GetLogger("Syslog"));
  ASSERT_TRUE(logger != nullptr);
  logger->ShowLocation(false);
  for (int test = 0; test < 10; ++test) {
    LOG_INFO() << "Testing " << test;
  }

  for (size_t count = 0; count < 100; ++count) {
    if (syslog_server->NofMessages() < 10) {
      std::this_thread::sleep_for(100ms);
    }
  }
  EXPECT_GE(syslog_server->NofMessages(), 10);
  for (auto msg = syslog_server->GetMsg(false); msg.has_value();
       msg = syslog_server->GetMsg(false)) {
    std::cout << msg.value().GenerateMessage() << std::endl;
  }

  LOG_TRACE() << "Deleting Syslog logger";
  log_config.DeleteLogger("Syslog");
  LOG_TRACE() << "Deleted Syslog logger";
  syslog_server->Stop();
  LOG_TRACE() << "Stopped syslog server";
}

TEST_F(TestSyslog, StartAndStopPublisher) {
  auto syslog_server =
      UtilFactory::CreateSyslogServer(SyslogServerType::TcpPublisher);
  syslog_server->Address("0.0.0.0");
  syslog_server->Port(6969);
  syslog_server->Start();
  SyslogMessage msg;
  msg.Message("Route 66 by Bob Dylan");
  for (size_t index = 0; index < 12; ++index) {
    syslog_server->AddMsg(msg);
  }
  std::this_thread::sleep_for(1s);
  syslog_server->Stop();
}

TEST_F(TestSyslog, TestSubscriber) {
  auto server = UtilFactory::CreateSyslogServer(SyslogServerType::TcpPublisher);
  ASSERT_TRUE(server != nullptr);
  server->Address("0.0.0.0");
  server->Port(42514);
  server->Start();

  for (size_t index = 0; index < 12; ++index) {
    SyslogMessage msg;
    std::ostringstream text;
    text << "Msg " << index;
    msg.Message(text.str());
    server->AddMsg(msg);
  }
  EXPECT_EQ(server->NofConnections(), 0);

  auto client1 =
      UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
  ASSERT_TRUE(server != nullptr);
  client1->Address("127.0.0.1");
  client1->Port(42514);
  bool connected = false;

  client1->Start();

  for (size_t index1 = 0; index1 < 100; ++index1) {
    connected = client1->IsOperable();
    if (connected) {
      break;
    }
    std::this_thread::sleep_for(100ms);
  }
  EXPECT_TRUE(connected);
  EXPECT_EQ(server->NofConnections(), 1);

  size_t nof_messages = 0;
  for (size_t index2 = 0; index2 < 1000 && connected; ++index2) {
    const auto msg = client1->GetMsg(false);
    if (msg.has_value()) {
      LOG_TRACE() << msg.value().GenerateMessage();
      ++nof_messages;
    }
    if (nof_messages >= 12) {
      break;
    }
    std::this_thread::sleep_for(10ms);
  }
  EXPECT_EQ(nof_messages, 12);

  client1->Stop();

  connected = client1->IsOperable();
  EXPECT_FALSE(connected);
  client1.reset();
  for (size_t index2 = 0; index2 < 20; ++index2) {
    if (server->NofConnections() == 0) {
      break;
    }
    std::this_thread::sleep_for(1s);
  }
  EXPECT_EQ(server->NofConnections(), 0);
  server->Stop();
  server.reset();
}

TEST_F(TestSyslog, TestStress) {
  auto server = UtilFactory::CreateSyslogServer(SyslogServerType::TcpPublisher);
  ASSERT_TRUE(server != nullptr);
  server->Address("0.0.0.0");
  server->Port(42514);
  server->Start();

  for (size_t index = 0; index < 12; ++index) {
    SyslogMessage msg;
    std::ostringstream text;
    text << "Msg " << index;
    msg.Message(text.str());
    server->AddMsg(msg);
  }

  for (size_t index1 = 0; index1 < 100; ++index1) {
    auto client =
        UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
    ASSERT_TRUE(server != nullptr);
    client->Address("127.0.0.1");
    client->Port(42514);
    client->Start();
    size_t nof_messages = 0;
    for (size_t index2 = 0; index2 < 1000; ++index2) {
      const auto msg = client->GetMsg(false);
      if (msg.has_value()) {
        ++nof_messages;
      }
      if (nof_messages >= 12) {
        break;
      }
      std::this_thread::sleep_for(10ms);
    }
    client->Stop();
    client.reset();
  }
  server->Stop();
  server.reset();
}

TEST_F(TestSyslog, TestResolveAll) {
  auto server = UtilFactory::CreateSyslogServer(SyslogServerType::TcpPublisher);
  ASSERT_TRUE(server != nullptr);
  server->Address("0.0.0.0");
  server->Port(42514);
  server->Start();

  {
    auto client =
        UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
    ASSERT_TRUE(server != nullptr);
    client->Address("127.0.0.1");
    client->Port(42514);
    bool connected = false;

    client->Start();
    for (size_t index = 0; index < 100; ++index) {
      connected = client->IsOperable();
      if (connected) {
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
    EXPECT_TRUE(connected);
    client->Stop();
    client.reset();
  }
  {
    auto client =
        UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
    ASSERT_TRUE(server != nullptr);
    client->Address("localhost");
    client->Port(42514);
    bool connected = false;

    client->Start();
    for (size_t index = 0; index < 100; ++index) {
      connected = client->IsOperable();
      if (connected) {
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
    EXPECT_TRUE(connected);
    client->Stop();
    client.reset();
  }

  {
    auto client =
        UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
    ASSERT_TRUE(server != nullptr);
    client->Address(boost::asio::ip::host_name());
    LOG_TRACE() << "Hostname: " << client->Address();
    client->Port(42514);
    bool connected = false;

    client->Start();
    for (size_t index = 0; index < 100; ++index) {
      connected = client->IsOperable();
      if (connected) {
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
    EXPECT_TRUE(connected);
    client->Stop();
    client.reset();
  }
  server->Stop();
  server.reset();
}

TEST_F(TestSyslog, TestResolveLocal) {
  auto server = UtilFactory::CreateSyslogServer(SyslogServerType::TcpPublisher);
  ASSERT_TRUE(server != nullptr);
  server->Address("localhost");
  server->Port(42514);
  server->Start();

  {
    auto client =
        UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
    ASSERT_TRUE(server != nullptr);
    client->Address("127.0.0.1");
    client->Port(42514);
    bool connected = false;

    client->Start();
    for (size_t index = 0; index < 100; ++index) {
      connected = client->IsOperable();
      if (connected) {
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
    EXPECT_TRUE(connected);
    client->Stop();
    client.reset();
  }
  {
    auto client =
        UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
    ASSERT_TRUE(server != nullptr);
    client->Address("localhost");
    client->Port(42514);
    bool connected = false;

    client->Start();
    for (size_t index = 0; index < 100; ++index) {
      connected = client->IsOperable();
      if (connected) {
        break;
      }
      std::this_thread::sleep_for(10ms);
    }
    EXPECT_TRUE(connected);
    client->Stop();
    client.reset();
  }

  {
    auto client =
        UtilFactory::CreateSyslogServer(SyslogServerType::TcpSubscriber);
    ASSERT_TRUE(server != nullptr);
    client->Address(boost::asio::ip::host_name());
    LOG_TRACE() << "Hostname: " << client->Address();
    client->Port(42514);
    bool connected = false;

    client->Start();
    for (size_t index = 0; index < 100; ++index) {
      connected = client->IsOperable();
      if (connected) {
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
    EXPECT_FALSE(connected);
    client->Stop();
    client.reset();
  }
  server->Stop();
  server.reset();
}
}  // namespace util::test