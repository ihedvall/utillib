/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <string_view>

#include "util/isuperviseapplication.h"
#include "util/ixmlnode.h"
#include "util/logstream.h"
#include "util/stringutil.h"

using namespace util::xml;
using namespace util::log;
using namespace util::string;

namespace {

constexpr std::array<std::string_view, 3> kStartupList = {
    "Manual", "Once", "Automatic"
};

constexpr std::array<std::string_view, 6> kPriorityList = {
    "Realtime", "High", "AboveNormal", "Normal",
    "BelowNormal", "Idle"
};
}

namespace util::supervise {

void ISuperviseApplication::StartupAsString(const std::string &startup) {
  for (size_t index = 0; index < kStartupList.size(); ++index) {
    if (IEquals(std::string(kStartupList[index]), startup)) {
      Startup(static_cast<ApplicationStartup>(index));
      return;
    }
  }
  // Keep the default/current value if coming here
  if (!startup.empty()) {
    LOG_ERROR() << "Unknown startup string. Value: " << startup;
  }
}

std::string_view ISuperviseApplication::StartupAsString() const {
  if (const auto index = static_cast<uint8_t> (Startup());
      index < kStartupList.size()) {
    return kStartupList[index];
  }
  return kStartupList[0];
}

void ISuperviseApplication::PriorityAsString(const std::string &priority) {
  for (size_t index = 0; index < kPriorityList.size(); ++index) {
    if (IEquals(std::string(kPriorityList[index]), priority)) {
      Priority(static_cast<ApplicationPriority>(index));
      return;
    }
  }
  // Keep the default/current value if coming here
  if (!priority.empty()) {
    LOG_ERROR() << "Unknown priority string. Value: " << priority;
  }
}

std::string_view ISuperviseApplication::PriorityAsString() const {
  if (const auto index = static_cast<uint8_t> (Priority());
      index < kPriorityList.size()) {
    return kPriorityList[index];
  }
  return kPriorityList[3];
}

void ISuperviseApplication::Start() {}

void ISuperviseApplication::Stop() {}

void ISuperviseApplication::Poll() {}

void ISuperviseApplication::ReadConfig(const IXmlNode &application_node) {
  Name(application_node.Attribute<std::string>("name"));
  if (Name().empty()) {
    Name(application_node.Property<std::string>("Name"));
  }
  Path(application_node.Property<std::string>("Path"));
  Arguments(application_node.Property<std::string>("Arguments"));
  StartupAsString(application_node.Property<std::string>("Startup"));
  PriorityAsString(application_node.Property<std::string>("Priority"));
  ShowWindow(application_node.Property<bool>("ShowWindow"));
  StartupDelay(application_node.Property<uint32_t>("StartupDelay"));
  MaxRestarts(application_node.Property<uint32_t>("MaxRestarts"));
}

void ISuperviseApplication::SaveConfig(IXmlNode &app_node) const {
  app_node.SetAttribute("name", Name());
  app_node.SetProperty("Name", Name());
  app_node.SetProperty("Path", Path());
  app_node.SetProperty("Arguments", Arguments());
  app_node.SetProperty("Startup", StartupAsString());
  app_node.SetProperty("Priority", PriorityAsString());
  app_node.SetProperty("ShowWindow", ShowWindow());
  app_node.SetProperty("StartupDelay", StartupDelay());
  app_node.SetProperty("MaxRestarts", MaxRestarts());
}

std::vector<std::string> ISuperviseApplication::ParseCommandLine() const {
  std::vector<std::string> list;
  std::ostringstream arg;
  bool text = false;
  for (const char cin : arguments_) {
    switch (cin) {
      case '\"':
      case '\'':
        if (!text) {
          text = true;
        } else {
          text = false;
        }
        arg << cin;
        break;
      default:
        if (iswspace(cin)) {
          if (text) {
            arg << cin;
          } else if (!arg.str().empty()) {
            list.push_back(arg.str());
            arg.clear();
            arg.str("");
          }
        }
        break;
    }
  }
  if (!arg.str().empty()) {
    list.push_back(arg.str());
  }
  return list;
}
} // util