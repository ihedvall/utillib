/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <stdexcept>
#include <filesystem>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <array>

#include "util/isupervisemaster.h"

#include "util/ixmlfile.h"
#include "util/logstream.h"
#include "util/systeminfo.h"

using namespace std::filesystem;
using namespace util::xml;
using namespace util::log;

namespace util::supervise {
ISuperviseMaster::~ISuperviseMaster() {
  ISuperviseMaster::Stop();
}

void ISuperviseMaster::ConfigFile(std::string config_file) {
  config_file_ = std::move(config_file);
  try {
    path filename(config_file_);
    path stem = filename.stem();
    if (program_name_.empty()) {
      program_name_ = stem.string();
    }

  } catch (const std::exception& err) {
    LOG_ERROR() << "Config file error. Error: " << err.what();
  }
}

void ISuperviseMaster::Start() {
  for (const auto& app : applications_) {
    if (app) {
      app->Start();
    }
  }
}

void ISuperviseMaster::Stop() {
  for (const auto& app : applications_) {
    if (app) {
      app->Stop();
    }
  }
}

ISuperviseApplication* ISuperviseMaster::CreateApplication() {
  auto application = std::make_unique<ISuperviseApplication>();
  applications_.push_back(std::move(application));
  auto& app_ptr = applications_.back();
  return app_ptr.get();
}

void ISuperviseMaster::ReadConfig() {
  FindConfigFile();
  try {
   path filename(config_file_);
   if (!exists(filename)) {
     std::ostringstream err;
     err << "The configuration file doesn't exist. File: " << config_file_;
     throw std::runtime_error(err.str());
   }
   auto xml_file = xml::CreateXmlFile();
   if (!xml_file) {
     std::ostringstream err;
     err << "Couldn't create an XML file. File: " << config_file_;
     throw std::runtime_error(err.str());
   }
   xml_file->FileName(config_file_);
   bool parse = xml_file->ParseFile();
   if (!parse) {
     std::ostringstream err;
     err << "Could not parse XML file. File: " << config_file_;
     throw std::runtime_error(err.str());
   }
   const IXmlNode* root_node = xml_file->RootNode();
   if (root_node == nullptr) {
     std::ostringstream err;
     err << "No root node found in XML file. File: " << config_file_;
     throw std::runtime_error(err.str());
   }
   IXmlNode::ChildList app_list;
   root_node->GetChildList(app_list);
   for (const auto& app_node : app_list) {
     if (!app_node || !app_node->IsTagName("Application")) {
       continue;
     }
     auto application = CreateApplication();
     if (!application) {
       throw std::runtime_error(
         "Could create a new application. File: " + config_file_);
     }
     application->ReadConfig(*app_node);
   }
  } catch (const std::exception& err) {
   LOG_ERROR() << "Fail to read the config file. Error: " << err.what();
  }
}

void ISuperviseMaster::SaveConfig() {

  try {
    if (config_file_.empty()) {
      throw std::runtime_error("The config file is not defined.");
    }

    path filename(config_file_);
    path home_path;
    if (!filename.has_parent_path()) {
      SystemInfo system_info(ProgramName());
      home_path = system_info.StatePath();
      filename = home_path;
      filename.append(config_file_);
      config_file_ = filename.string();
      LOG_INFO() << "Replaced path in config file. Path: " << config_file_;
    } else {
      home_path = filename.parent_path();
    }
    create_directories(home_path);

    auto xml_file = xml::CreateXmlFile("FileWriter");
    if (!xml_file) {
      std::ostringstream err;
      err << "Couldn't create an XML file. File: " << config_file_;
      throw std::runtime_error(err.str());
    }
    auto& root_node = xml_file->RootName("SuperviseMaster");
    for (const auto& app : applications_) {
      if (!app) {
        continue;
      }
      app->SaveConfig(root_node);
    }
    xml_file->FileName(config_file_);
    const bool save = xml_file->WriteFile();
    if (!save) {
      throw std::runtime_error("Failed to write the config file. File: "
        + config_file_);
    }

  } catch (const std::exception& err) {
      LOG_ERROR() << "Fail to save the config file. Error: " << err.what();
  }
}

void ISuperviseMaster::FindConfigFile() {
  SystemInfo system_info(ProgramName());
  try {
    // First check if file is not set yet
    // Try using default name.
    if (config_file_.empty()) {
      config_file_ = "supervise.xml";
    }

    // Check if the file exists.
    // If so return.
    const path fullname(config_file_);
    if (exists(fullname)) {
      return;
    }
    const path filename = fullname.filename();

    // Check if exist in knsown path
    const std::array<std::string, 6> know_paths {
      current_path().string(),
      system_info.StatePath(),
      system_info.CachePath(),
      system_info.ConfigPath(),
      system_info.DataPath(),
      system_info.ProgramDataPath(),
    };
    for (const std::string& known_path : know_paths) {
      path home(known_path);
      home.append(filename.string());
      if (exists(home)) {
        config_file_ = home.string();
        LOG_INFO() << "found config file. Path: " << config_file_;
        return;
      }
    }
    LOG_ERROR() << "Config file not found. File; "
      << config_file_;
  } catch (const std::exception& err) {
    LOG_ERROR() << "Fail to find the config file. Error: " << err.what();
  }
}

} // util::supervise