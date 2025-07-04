/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file systeminfo.h
 * @brief Interface against the system information.
 *
 * Support class that is a wrapper around against
 * system settings and known directories.
 */
#pragma once

#include <filesystem>
#include <string>
#include <algorithm>

namespace util::supervise {

/**
 * @brief Simple support class to known directories.
 *
 * Support class that is a wrapper around against
 * system settings and known directories.
 */
class SystemInfo {
 public:
  /**
   * @brief Default constructor.
   *
   * Use this constructor to get the operating system
   * default directories.
   */
  SystemInfo() = default;

  /**
   * @brief Constructor with its program or system name.
   *
   * This is the normal use.
   * The program or system name defines where the system
   * places its log and configuration files.
   *
   * @param program_name Program or system name.
   */
  explicit SystemInfo(std::string program_name);

  /**
   * @brief Sets the system or program name.
   *
   * The system program are appended to the
   * operating system default paths.
   * @param name System or program name.
   */
  void ProgramName(std::string name) { program_name_ = std::move(name); }

  /**
   * @brief Returns the program or system name.
   * @return Program or system name.
   */
  [[nodiscard]] std::string ProgramName() const { return program_name_; }

  /**
   * @brief Returns the default data path.
   * @return Default data path.
   */
  [[nodiscard]] std::string DataPath() const;

  /**
   * @brief Returns the default configuration path.
   * @return Default configuration path.
   */
  [[nodiscard]] std::string ConfigPath() const;

  /**
   * @brief Returns the default cache path.
   * @return Cache path.
   */
  [[nodiscard]] std::string CachePath() const;

  /**
   * @brief Returns the default state path.
   * @return Default state path.
   */
  [[nodiscard]] std::string StatePath() const;

  /**
   * @brief Returns the default program data path.
   * @return Default state path.
   */
  [[nodiscard]] std::string ProgramDataPath() const;

 private:
  std::string program_name_;
};

} // util


