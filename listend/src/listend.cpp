/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */



#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include <util/ixmlfile.h>
#include <util/listenconfig.h>
#include <util/logconfig.h>
#include <util/logstream.h>
#include <util/utilfactory.h>

using namespace util::log;
using namespace util::xml;
using namespace std::chrono_literals;
namespace {

std::atomic<bool> kStopMain = false;
std::vector<std::unique_ptr<IListen>> kServerList;
uint16_t kFreePort = 42511;

void StopMainHandler(int signal) {

  LOG_INFO() << "Stopping. Signal: " << signal;
  kStopMain = true;
}

void AddAllKnownServers() {
  // Add System Logger
  {
    auto system_log = util::UtilFactory::CreateListen(
    "ListenServer", "LISLOG");
    system_log->Name("System Messages");
    system_log->Description("Logs all system messages");
    system_log->HostName("127.0.0.1");
    system_log->Port(kFreePort++);
    system_log->SetLogLevelText(0, "Show all log messages");
    system_log->SetLogLevelText(1, "Hide trace messages");
    system_log->SetLogLevelText(2, "Hide trace/debug messages");
    system_log->SetLogLevelText(3, "Hide trace/debug/info messages");
    kServerList.push_back(std::move(system_log));
  }

  // Add SQLite Logger
  {
    auto sqlite_log =
        util::UtilFactory::CreateListen(
        "ListenServer", "LISSQLITE");
    sqlite_log->Name("SQLite messages");
    sqlite_log->Description("Logs all SQL calls");
    sqlite_log->HostName("127.0.0.1");
    sqlite_log->Port(kFreePort++);
    sqlite_log->SetLogLevelText(0, "Show all SQL calls");
    kServerList.push_back(std::move(sqlite_log));
  }

  // Add MQTT Logger
  {
    auto mqtt_log = util::UtilFactory::CreateListen(
    "ListenServer", "LISMQTT");
    mqtt_log->Name("MQTT messages");
    mqtt_log->Description("Logs all MQTT calls");
    mqtt_log->HostName("127.0.0.1");
    mqtt_log->Port(kFreePort++);
    mqtt_log->SetLogLevelText(0, "Show basic MQTT messages");
    mqtt_log->SetLogLevelText(1, "Show MQTT publish messages");
    mqtt_log->SetLogLevelText(2, "Show MQTT subscribe messages");
    mqtt_log->SetLogLevelText(3, "Trace MQTT messages");
    kServerList.push_back(std::move(mqtt_log));
  }

    // Add Bus Message Logger
  {
    auto mqtt_log = util::UtilFactory::CreateListen(
    "ListenServer", "LISBUS");
    mqtt_log->Name("Bus Messages");
    mqtt_log->Description("Logs all message on a bus message queue");
    mqtt_log->HostName("127.0.0.1");
    mqtt_log->Port(kFreePort++);
    mqtt_log->SetLogLevelText(0, "Show all messages");
    mqtt_log->SetLogLevelText(1, "Show CAN messages");
    mqtt_log->SetLogLevelText(2, "Show LIN messages");
    mqtt_log->SetLogLevelText(3, "Show FlexRay messages");
    mqtt_log->SetLogLevelText(4, "Show MOST messages");
    mqtt_log->SetLogLevelText(5, "Show Ethernet messages");
    kServerList.push_back(std::move(mqtt_log));
  }
}
}  // namespace

int main(int nof_arg, char* arg_list[]) {
  signal(SIGTERM, StopMainHandler);
  signal(SIGABRT, StopMainHandler);
  signal(SIGINT, StopMainHandler);
#if (_MSC_VER)
  signal(SIGABRT_COMPAT, StopMainHandler);
  signal(SIGBREAK, StopMainHandler);
  signal(SIGINT, StopMainHandler);
 // signal(SIGABRT_COMPAT, StopMainHandler);
#endif

  // Set log file name to the service name
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("utillib/log");
  log_config.BaseName("listend");
  log_config.CreateDefaultLogger();
  LOG_DEBUG() << "Log File created. Path: " << log_config.GetLogFile();

  std::vector<std::string> config_files;
  std::string program_data_path = ProgramDataPath();
  for (int arg = 1; arg < nof_arg; ++arg) {
    // configuration files
    const std::string config_file(arg_list[arg]);
    try {
      std::filesystem::path full_name(config_file);
      if (!full_name.has_parent_path()) {
        std::filesystem::path temp_name = program_data_path;
        temp_name.append("utillib");
        temp_name.append("conf");
        temp_name.append(config_file);
        full_name = temp_name;
      }
      if (!std::filesystem::exists(full_name)) {
        LOG_ERROR() << "Config file doesn't exist- File: "
                    << full_name.string();
        continue;
      }
      config_files.emplace_back(full_name.string());
    } catch (const std::exception& error) {
      LOG_ERROR() << "Failed to find the configuration file. File: "
                  << config_file << ", Error: " << error.what();
    }
  }
  // First check if the 'listend.xml' exist in default program path
  if (config_files.empty()) {
    try {
      std::filesystem::path temp_name = program_data_path;
      temp_name.append("utillib");
      temp_name.append("conf");
      temp_name.append("listend.xml");
      if (std::filesystem::exists(temp_name)) {
        config_files.emplace_back(temp_name.string());
      } else {
        LOG_INFO() << "There is no configuration files for this application";
      }
    } catch (const std::exception& error) {
      LOG_ERROR() << "Failed to find the default configuration file. Error: "
                  << error.what();
    }
  }

  // If no configuration files exist. Add all known servers.
  if (config_files.empty()) {
    AddAllKnownServers();
  }

  {
    // Start the listen config
    ListenConfig master;

    // Read all configuration files and
    for (const auto& config_file : config_files) {
      auto xml_file = util::xml::CreateXmlFile();
      xml_file->FileName(config_file);
      const auto parse = xml_file->ParseFile();
      if (!parse) {
        LOG_ERROR() << "Failed to parse the XML file. File: " << config_file;
        continue;
      }
      IXmlNode::ChildList node_list;
      xml_file->GetChildList(node_list);
      for (const auto* node : node_list) {
        if (node->IsTagName("ListenServer")) {
          const auto share_name = node->Property<std::string>("ShareName", "");
          const auto name = node->Property<std::string>("Name", "");
          const auto description =
              node->Property<std::string>("Description", "");
          const auto pre_text = node->Property<std::string>("PreText", "");
          const auto host_name =
              node->Property<std::string>("HostName", "127.0.0.1");
          const auto port = node->Property<uint16_t>("Port", 0);

          auto listen =
              util::UtilFactory::CreateListen("ListenServer", share_name);
          if (!listen) {
            continue;
          }
          listen->Name(name);
          listen->Description(description);
          listen->PreText(pre_text);
          listen->HostName(host_name);
          listen->Port(port);

          const auto* list = node->GetNode("LogLevelList");
          IXmlNode::ChildList log_list;
          if (list != nullptr) {
            list->GetChildList(log_list);
          }
          for (const auto* level : log_list) {
            if (!level->IsTagName("LogLevel")) {
              continue;
            }
            const auto number = level->Attribute<uint64_t>("level", 0);
            const auto menu_text = level->Value<std::string>();
            listen->SetLogLevelText(number, menu_text);
          }
          kServerList.push_back(std::move(listen));
        }
      }
    }
    const std::vector<std::string> no_arg;
    log_config.AddLogger("Listen Daemon Message", LogType::LogToListen, no_arg);
    for (auto& server : kServerList) {
      if (!server) {
        continue;
      }
      LOG_DEBUG() << "Starting listen server. Name: " << server->Name();
      server->Start();
    }


    while (!kStopMain) {
#ifdef _WIN32
      MSG msg = {};
      if (const BOOL peek_msg = ::PeekMessage(&msg, nullptr,
            0, 0, PM_REMOVE);
        peek_msg) {

        switch (msg.message) {
          case WM_QUIT:
          case WM_CLOSE:
            LOG_TRACE() << "Close/Quit of application received.";
            kStopMain = true;
            continue;

          default:
            break;
        }
      }
#endif
      std::this_thread::sleep_for(100ms);
    }

    for (auto& server : kServerList) {
      if (!server) {
        continue;
      }
      LOG_DEBUG() << "Stopping listen server. Name: " << server->Name();
      server->Stop();
    }
    log_config.DeleteLogger("Listen Daemon Message");

    kServerList.clear();
    LOG_DEBUG() << "Stopped listen daemon";
  }
  log_config.DeleteLogChain();
  kStopMain = false;

  return EXIT_SUCCESS;
}
