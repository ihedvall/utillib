/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "testlisten.h"

#include <chrono>
#include <string_view>

#include "listenclient.h"
#include "listenserver.h"
#include "util/ilisten.h"
#include "util/listenconfig.h"
#include "util/logconfig.h"
#include "util/timestamp.h"
#include "util/utilfactory.h"

using namespace util::log;
using namespace util::log::detail;
using namespace std::chrono_literals;

namespace {
constexpr uint64_t kServerPort = 43099;
constexpr std::string_view kServerName = "TestServer";
constexpr std::string_view kServerPreText = "TS>";
}  // namespace
namespace util::test {
void TestListen::SetUpTestCase() {
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();
}

void TestListen::TearDownTestCase() {
  auto &log_config = LogConfig::Instance();
  log_config.DeleteLogChain();
}

bool ListenMock::IsActive() const { return true; }

void ListenMock::AddMessage(uint64_t nano_sec_1970, const std::string &pre_text,
                            const std::string &text) {
  std::cout << time::NsToLocalIsoTime(nano_sec_1970) << " " << pre_text << " "
            << text << std::endl;
}

size_t ListenMock::LogLevel() { return 0; }

TEST_F(TestListen, ListenBasic) {
  ListenMock listen;
  listen.PreText("TEST>");

  listen.ListenOut() << "Test text " << 10;
  std::ostringstream temp;
  temp << "Test text " << 11;
  listen.ListenString(temp.str());
  listen.ListenText("Test text %d", 12);
  listen.ListenTextEx(0, "NULL>", "Test text %d", 33);
  const std::vector<uint8_t> buffer = {0, 1, 2, 3, 4, 5, 6, 7};
  listen.ListenTransmit(time::TimeStampToNs(), "T>", buffer, nullptr);
  listen.ListenReceive(time::TimeStampToNs(), "R>", buffer, nullptr);
}

TEST_F(TestListen, ListenServer) {
  auto server = UtilFactory::CreateListen("ListenServer", "");
  ASSERT_TRUE(server);
  server->Name(kServerName.data());
  server->Description("Test server");
  server->HostName("127.0.0.1");
  server->Port(kServerPort);

  const bool start = server->Start();
  EXPECT_TRUE(start);

  auto client = UtilFactory::CreateListenClient("localhost", kServerPort);
  ASSERT_TRUE(client);

  for (size_t index1 = 0; index1 < 100; ++index1) {
    if (server->NofConnections() == 1) {
      break;
    }
    std::this_thread::sleep_for(100ms);
  }
  EXPECT_EQ(server->NofConnections(), 1);

  client.reset();

  const bool stop = server->Stop();
  EXPECT_TRUE(stop);
  server.reset();
}

TEST_F(TestListen, ListenConfig) {
  ListenPortConfig devils_port;
  devils_port.port = 666;
  devils_port.name = "Devils Gate";
  devils_port.share_name = "Trump";
  devils_port.description = "Republican";

  {
    auto list_empty = GetListenConfigList();
    EXPECT_TRUE(list_empty.empty());

    AddListenConfig(devils_port);
    auto list_one = GetListenConfigList();
    EXPECT_EQ(list_one.size(), 1);

    DeleteListenConfig(devils_port.port);
    auto list_none = GetListenConfigList();
    EXPECT_TRUE(list_none.empty());
  }
  {
    AddListenConfig(devils_port);
    DeleteListenConfig(devils_port.port);
  }
}

}  // namespace util::test