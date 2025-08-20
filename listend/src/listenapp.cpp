/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "listenapp.h"
#include <chrono>
#include <filesystem>

#include <util/ixmlfile.h>
#include <util/logconfig.h>
#include <util/logstream.h>
#include <util/utilfactory.h>

using namespace std::filesystem;
using namespace std::chrono_literals;

using namespace util::log;
using namespace util::xml;

namespace util {

void ListenApp::AddAllKnownServers() {
  // Add System Logger
  {
    auto system_log =
        UtilFactory::CreateListen(TypeOfListen::ListenServerType, "LISLOG");
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
    auto sqlite_log = util::UtilFactory::CreateListen(
        TypeOfListen::ListenServerType, "LISSQLITE");
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
        TypeOfListen::ListenServerType, "LISMQTT");
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
        TypeOfListen::ListenServerType, "LISBUS");
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

bool ListenApp::OnInit() {
  PollDelay(1000);
  // Set log filename to the service name
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("utillib/log");
  log_config.BaseName("listend");
  log_config.CreateDefaultLogger();
  LOG_DEBUG() << "Log File created. Path: " << log_config.GetLogFile();

  std::vector<std::string> config_files;
  std::string program_data_path = ProgramDataPath();
  for (const std::string& arg : Args()) {
    // configuration files
    const std::string config_file(arg);
    try {
      path full_name(config_file);
      if (!full_name.has_parent_path()) {
        path temp_name = program_data_path;
        temp_name.append("utillib");
        temp_name.append("conf");
        temp_name.append(config_file);
        full_name = temp_name;
      }
      if (!exists(full_name)) {
        LOG_ERROR() << "Config file doesn't exist- File: "
                    << full_name.string();
        continue;
      }
      config_files.emplace_back(full_name.string());
    } catch (const std::exception& error) {
      LOG_ERROR() << "Didn't find the configuration file. File: " << config_file
                  << ", Error: " << error.what();
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
      LOG_ERROR() << "Didn't find the default configuration file. Error: "
                  << error.what();
    }
  }

  // If no configuration files exist. Add all known servers.
  if (config_files.empty()) {
    AddAllKnownServers();
  }

  // Start the listen config
  master = std::make_unique<ListenConfig>();

  // Read all configuration files and
  for (const auto& config_file : config_files) {
    auto xml_file = CreateXmlFile();
    xml_file->FileName(config_file);
    const auto parse = xml_file->ParseFile();
    if (!parse) {
      LOG_ERROR() << "Didn't parse the XML file. File: " << config_file;
      continue;
    }
    IXmlNode::ChildList node_list;
    xml_file->GetChildList(node_list);
    for (const auto* node : node_list) {
      if (node->IsTagName("ListenServer")) {
        const auto share_name = node->Property<std::string>("ShareName", "");
        const auto name = node->Property<std::string>("Name", "");
        const auto description = node->Property<std::string>("Description", "");
        const auto pre_text = node->Property<std::string>("PreText", "");
        const auto host_name =
            node->Property<std::string>("HostName", "127.0.0.1");
        const auto port = node->Property<uint16_t>("Port", 0);

        auto listen = UtilFactory::CreateListen(TypeOfListen::ListenServerType,
                                                share_name);
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

  ExitCode(EXIT_FAILURE);  // Reset in OnExit
  return ConsoleApp::OnInit();
}

void ListenApp::OnExit() {
  auto& log_config = LogConfig::Instance();
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

  master.reset();

  log_config.DeleteLogChain();
  ExitCode(EXIT_SUCCESS);
  ConsoleApp::OnExit();
}

}  // namespace util