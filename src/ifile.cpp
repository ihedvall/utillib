/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/ifile.h"
#include <chrono>
#include <filesystem>
#include "util/timestamp.h"

using namespace std::filesystem;
using namespace std::chrono;

namespace util::log {
IFile::IFile(const std::string& filename) {
  Filename(filename);
}

IFile::~IFile() {

}

void IFile::Filename(const std::string& filename) {
  filename_ = filename;
  try {
    const path file(filename_);
    const auto time_file = last_write_time(file);
    modified_ = time::FileTimeToNs(time_file);
    file_size_ = file_size(file);

  } catch (const std::exception&) {}
}
std::string IFile::Name() const {
  try {
    const path full_name(filename_);
    return full_name.filename().string();
  } catch (const std::exception&) {
  }
  return {};
}


}  // namespace util::log