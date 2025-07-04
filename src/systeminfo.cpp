/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <filesystem>

#include <sago/platform_folders.h>

#include "util/systeminfo.h"
#include "util/logstream.h"
#include "util/logconfig.h"

using namespace sago;
using namespace std::filesystem;
using namespace util::log;

namespace util::supervise {
SystemInfo::SystemInfo(std::string program_name)
  : program_name_(std::move(program_name)) {

}

std::string SystemInfo::DataPath() const {
  try {
    path home(getDataHome());
    if (!program_name_.empty()) {
      home.append(program_name_);
    }
    return home.string();
  } catch (const std::exception& err) {
    LOG_ERROR() << "Data home dir failed: Error " << err.what();
  }
  return {};
}

std::string SystemInfo::ConfigPath() const {
  try {
    path home(getConfigHome());
    if (!program_name_.empty()) {
      home.append(program_name_);
    }
    return home.string();
  } catch (const std::exception& err) {
    LOG_ERROR() << "Config home dir failed: Error " << err.what();
  }
  return {};
}

std::string SystemInfo::CachePath() const {
  try {
    path home(getCacheDir());
    if (!program_name_.empty()) {
      home.append(program_name_);
    }
    home.append("cache");
    return home.string();
  } catch (const std::exception& err) {
    LOG_ERROR() << "Cache dir failed: Error " << err.what();
  }
  return {};
}

std::string SystemInfo::StatePath() const {
  try {
    path home(getStateDir());
    if (!program_name_.empty()) {
      home.append(program_name_);
    }
    return home.string();
  } catch (const std::exception& err) {
    LOG_ERROR() << "State dir failed: Error " << err.what();
  }
  return {};
}

std::string SystemInfo::ProgramDataPath() const {
  try {
    path home(util::log::ProgramDataPath());
    if (!program_name_.empty()) {
      home.append(program_name_);
    }
    return home.string();
  } catch (const std::exception& err) {
    LOG_ERROR() << "Program data dir failed: Error " << err.what();
  }
  return {};
}
} // util