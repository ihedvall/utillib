/*
 * Copyright 2024 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/ihwinfo.h"
#include <hwinfo/hwinfo.h>
#include "util/stringutil.h"

using namespace util::string;
using namespace hwinfo;

namespace util::hw_info {

std::string IHwInfo::CpuModel() {
  const auto cpu_list = hwinfo::getAllCPUs();
  for (const auto &cpu : cpu_list) {
    return Trim(cpu.modelName());
  }
  return {};
}

std::string IHwInfo::CpuVendor() {
  const auto cpu_list = hwinfo::getAllCPUs();
  for (const auto &cpu : cpu_list) {
    return Trim(cpu.vendor());
  }
  return {};
}

std::string IHwInfo::OsName() {
  OS operating_system;
  return Trim(operating_system.name());
}

std::string IHwInfo::OsVersion() {
  OS operating_system;
  return Trim(operating_system.version());
}

std::string IHwInfo::OsKernel() {
  OS operating_system;
  return Trim(operating_system.kernel());
}

std::string IHwInfo::MainBoardName() {
  MainBoard main_board;
  return Trim(main_board.name());
}

std::string IHwInfo::MainBoardVendor() {
  MainBoard main_board;
  return Trim(main_board.vendor());
}

std::string IHwInfo::MainBoardVersion() {
  MainBoard main_board;
  return Trim(main_board.version());
}

std::string IHwInfo::MainBoardSerialNumber() {
  MainBoard main_board;
  return Trim(main_board.serialNumber());
}

} // util::hw_info