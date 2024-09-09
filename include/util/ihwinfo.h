/*
 * Copyright 2024 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace util::hw_info {

class IHwInfo final {
 public:
  [[nodiscard]] static std::string CpuModel();
  [[nodiscard]] static std::string CpuVendor();

  [[nodiscard]] static std::string OsName();
  [[nodiscard]] static std::string OsVersion();
  [[nodiscard]] static std::string OsKernel();

  [[nodiscard]] static std::string MainBoardName();
  [[nodiscard]] static std::string MainBoardVendor();
  [[nodiscard]] static std::string MainBoardVersion();
  [[nodiscard]] static std::string MainBoardSerialNumber();
};

} // util::hw_info


