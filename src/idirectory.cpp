/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/idirectory.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <ranges>
#include <sstream>

#include "util/stringutil.h"
#include "util/timestamp.h"

using namespace std::filesystem;
using namespace std::chrono;

namespace {
std::string FilterListToText(const util::log::FilterList& list) {
  std::ostringstream temp;
  size_t count = 0;
  for (const auto& text : list) {
    if (text.empty()) {
      continue;
    }
    if (count > 0) {
      temp << ";";
    }
    temp << text;
    ++count;
  }
  return temp.str();
}

void TextToFilterList(const std::string& text, util::log::FilterList& list) {
  list.clear();
  std::ostringstream temp;
  for (const char cin : text) {
    switch (cin) {
      case ';':
      case ':':
      case ',':
      case '|': {
        auto text1 = util::string::Trim(temp.str());
        if (!text1.empty()) {
          list.emplace_back(text1);
        }
        temp.str("");
        temp.clear();
        break;
      }

      default:
        temp << cin;
        break;
    }
  }
  auto text2 = util::string::Trim(temp.str());
  if (!text2.empty()) {
    list.emplace_back(text2);
  }
}
}  // namespace

namespace util::log {

IDirectory::IDirectory(const std::string& dir_path) { ParentDir(dir_path); }

void IDirectory::ParentDir(const std::string& dir) {
  parent_dir_ = dir;
  try {
    path full_name(parent_dir_);
    const auto time_file = last_write_time(full_name);
    modified_ = time::FileTimeToNs(time_file);
  } catch (const std::exception& err) {
    std::ostringstream msg;
    msg << "Init error. Directory: " << parent_dir_
        << ", Error: " << err.what();
  }
}
std::string IDirectory::Name() const {
  try {
    path parent_dir(parent_dir_);
    return parent_dir.stem().string();
  } catch (const std::exception&) {
  }
  return {};
}

bool IDirectory::ScanDirectory() {  // NOLINT
  dir_list_.clear();
  file_list_.clear();
  try {
    path parent_dir(parent_dir_);

    if (!exists(parent_dir)) {
      std::ostringstream err;
      err << "Parent directory doesn't exist. Directory: " << parent_dir;
      last_error_ = err.str();
      return false;
    }

    for (const auto& entry : directory_iterator(parent_dir)) {
      if (entry.is_directory()) {
        // Directory
        const auto dir_path = entry.path().string();
        const auto name = entry.path().stem().string();
        const auto inc = IncludeFile(name);
        const auto skip = ExcludeFile(name);
        if (!inc && skip) {
          continue;
        }
        auto dir = std::make_unique<IDirectory>(dir_path);
        dir->Level(Level() + 1);
        const auto sub_ok = dir->ScanDirectory();
        dir_list_.emplace(name, std::move(dir));
        if (!sub_ok) {
          return false;
        }
      } else if (entry.is_regular_file()) {
        // File
        const auto file_path = entry.path().string();
        const auto filename = entry.path().filename().string();
        const auto inc = IncludeFile(filename);
        const auto skip = ExcludeFile(filename);
        if (!inc && skip) {
          continue;
        }
        auto file = std::make_unique<IFile>(file_path);
        file_list_.emplace(filename, std::move(file));
      }
    }
  } catch (const std::exception& err) {
    std::ostringstream msg;
    msg << "File system error. Directory: " << parent_dir_
        << ", Error:  " << err.what();
    last_error_ = msg.str();
    return false;
  }
  return true;
}

std::string IDirectory::ExcludeListToString() const {
  return FilterListToText(exclude_list_);
}

void IDirectory::StringToExcludeList(const std::string& text) {
  TextToFilterList(text, exclude_list_);
}

std::string IDirectory::IncludeListToString() const {
  return FilterListToText(include_list_);
}

void IDirectory::StringToIncludeList(const std::string& text) {
  TextToFilterList(text, include_list_);
}

bool IDirectory::IncludeFile(const std::string& name) const {
  if (include_list_.empty() && exclude_list_.empty()) {
    return true;
  }
  return std::ranges::any_of(include_list_, [&](const auto& text) {
    return string::WildcardMatch(name, text, true);
  });
}

bool IDirectory::ExcludeFile(const std::string& name) const {
  if (!include_list_.empty() && exclude_list_.empty()) {
    return true;  // Skip everything that's not in the include list
  }
  return std::ranges::any_of(exclude_list_, [&](const auto& text) {
    return string::WildcardMatch(name, text, true);
  });
}

}  // namespace util::log