/*
 * Copyright 2024 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file ihwinfo.h
 * \brief Simple interface against hardware information.
 *
 * A simple interface against hardware information for this machine.
 */
#pragma once

#include <string>

namespace util::hw_info {

/** \brief Simple wrapper around hardware information.
 *
 * Implements a simple wrapper around functions that
 * reads hardware information about this computer.
 */
class IHwInfo final {
 public:
  /**
   * @brief Returns the CPU Model string.
   * @return CPU Model
   */
  [[nodiscard]] static std::string CpuModel();

  /**
   * @brief Returns the CPU Vendor.
   * @return CPU vendor
   */
  [[nodiscard]] static std::string CpuVendor();

  /**
   * @brief Returns the operating system.
   * @return Operating system name.
   */
  [[nodiscard]] static std::string OsName();

  /**
   * @brief Returns the operating system version.
   * @return Operating system version.
   */
  [[nodiscard]] static std::string OsVersion();

  /**
   * @brief Returns the operating system kernel.
   * @return Operating system kernel.
   */
  [[nodiscard]] static std::string OsKernel();

  /**
   * @brief Returns the mother board name.
   * @return Mother board name.
   */
  [[nodiscard]] static std::string MainBoardName();

  /**
   * @brief Returns the mother board manufacture.
   * @return Mother board manufacture.
   */
  [[nodiscard]] static std::string MainBoardVendor();

  /**
   * @brief Returns the mother board version.
   * @return Mother board version.
   */
  [[nodiscard]] static std::string MainBoardVersion();

  /**
   * @brief Returns the mother board serial number
   * @return
   */
  [[nodiscard]] static std::string MainBoardSerialNumber();
};

} // util::hw_info


