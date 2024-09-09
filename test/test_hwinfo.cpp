/*
 * Copyright 2024 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <gtest/gtest.h>
#include <hwinfo/hwinfo.h>
#include "util/stringutil.h"
#include "util/ihwinfo.h"

using namespace hwinfo;
using namespace util::string;
using namespace util::hw_info;

namespace util::test {

TEST(HwInfo, ListCpu) {
  const auto cpu_list = getAllCPUs();
  EXPECT_FALSE(cpu_list.empty());
  for (const auto &cpu : cpu_list) {
    std::cout << "CPU:" << std::endl;
    std::cout << "ID: " << cpu.id() << std::endl
              << "Model: " << Trim(cpu.modelName()) << std::endl
              << "Vendor: " << Trim(cpu.vendor()) << std::endl
              << std::endl;

  }
}

TEST(HwInfo, MainBoard) {
  MainBoard main_board;
  std::cout << "MAIN BOARD:" << std::endl;
  std::cout << "Version: " << main_board.version() << std::endl
            << "Name: " << main_board.name() << std::endl
            << "Vendor: " << main_board.vendor() << std::endl
            << std::endl;

}

TEST(HwInfo, OperatingSystem) {
  OS operating_system;
  std::cout << "OPERATING SYSTEM:" << std::endl;
  std::cout << "Version: " << operating_system.version() << std::endl
            << "Name: " << operating_system.name() << std::endl
            << "Kernel: " << operating_system.kernel() << std::endl
            << std::endl;

}

TEST(HwInfo, Test_IHwInfo) {
  const auto cpu_model = IHwInfo::CpuModel();
  std::cout << "CPU Model: " << cpu_model << std::endl;
  EXPECT_FALSE(cpu_model.empty());

  const auto cpu_vendor = IHwInfo::CpuVendor();
  std::cout << "CPU Vendor: " << cpu_vendor << std::endl;
  EXPECT_FALSE(cpu_vendor.empty());

  const auto os_name = IHwInfo::OsName();
  std::cout << "OS Name: " << os_name << std::endl;
  EXPECT_FALSE(os_name.empty());

  const auto os_version = IHwInfo::OsVersion();
  std::cout << "OS Version: " << os_version << std::endl;

  const auto os_kernel = IHwInfo::OsKernel();
  std::cout << "OS Kernel: " << os_kernel << std::endl;

  const auto main_board_name = IHwInfo::MainBoardName();
  std::cout << "Main Board Name: " << main_board_name << std::endl;

  const auto main_board_vendor = IHwInfo::MainBoardVendor();
  std::cout << "Main Board Vendor: " << main_board_vendor << std::endl;

  const auto main_board_version = IHwInfo::MainBoardVersion();
  std::cout << "Main Board Version: " << main_board_version << std::endl;

  const auto main_board_serial_number = IHwInfo::MainBoardSerialNumber();
  std::cout << "Main Board Serial Number: " << main_board_serial_number << std::endl;
}

}