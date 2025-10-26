/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file isupervisemaster.h
 * @brief Interface against a supervise master.
 *
 * Interface against a supervise master.
 * The master is mainly a list supervise applications.
 * The master is setup by an XML configuration file.
 */
#pragma once

#include <memory>
#include <string>
#include <algorithm>
#include <vector>

#include "util/isuperviseapplication.h"

namespace util::supervise {

/**
 * @brief Type of master software.
 *
 * Defines the type of master implementation.
 * The supervise master type is used mainly for internal testing.
 * The process master supervise an executable in Windows or Linux.
 *
 */
enum class TypeOfSuperviseMaster {
  SuperviseMasterType = 0,
  ProcessMasterType = 1,
};

/**
 * @brief Interface against a supervise master.
 *
 * A supervise master supervise one or more applications.
 * The purpose is simplify the creation of a system.
 * Instead of manually start several application services, the
 * master will start the system application as normal executables.
 *
 * The master is normally started as a service/Daemon but
 * can also be used in unit tests for testing applications and
 * systems.
 *
 */
class ISuperviseMaster {
 public:
  /**
   * @brief Default constructor.
   */
  ISuperviseMaster() = default;

  /**
   * @brief Default destructor.
   */
  virtual ~ISuperviseMaster();

  /**
   * @brief Defines the program/system name.
   *
   * The program name should be without any space.
   * It is used together with the default directories
   * to find configuration files.
   * @param name Program name or system name.
   */
  void ProgramName(std::string name) { program_name_ = std::move(name); }

  /**
   * @brief Program or system name.
   * @return Program name.
   */
  [[nodiscard]] const std::string &ProgramName() const { return program_name_; }

  /**
   * @brief Configuration file name.
   *
   * Sets the configuration file name.
   * If the configuration file is defined without any paths,
   * the class will search for the file in known directories.
   * Note that the program name is used when searching in directories.
   * @param config_file Configuration file name.
   */
  void ConfigFile(std::string config_file);

  /**
   * @brief Returns the configuration file name.
   * @return Configuration file name.
   */
  [[nodiscard]] const std::string &ConfigFile() const { return config_file_; }

  /**
   * @brief Enable a MQTT client.
   * @param enable True if the MQTT
   */
  void EnableMqtt(bool enable) { enable_mqtt_ = enable; }

  /**
   * @brief Returns true if a MQTT client is enabled.
   * @return True if the MQTT is used.
   */
  [[nodiscard]] bool EnableMqtt() const { return enable_mqtt_; }

  /**
   * @brief Start the system.
   */
  virtual void Start();

  /**
   * @brief Stop the system.
   */
  virtual void Stop();

  /**
   * @brief Create a supervise application.
   * @return Smart pointer to a supervise application.
   */
  virtual ISuperviseApplication *CreateApplication();

  /**
   * @brief Returns a list af supervise applications.
   * @return List of supervise applications.
   */
  std::vector<std::unique_ptr<ISuperviseApplication>> &Applications() {
    return applications_;
  }

  /**
   * @brief Returns a list af supervise applications.
   * @return List of supervise applications.
   */
  [[nodiscard]] const std::vector<std::unique_ptr<ISuperviseApplication>> &Applications() const {
    return applications_;
  }

  /**
   * @brief Read in the configuration.
   */
  void ReadConfig();

  /**
   * @brief Save the configuration.
   */
  void SaveConfig();

 protected:
  /**
   * @brief List of supervise applications.
   */
  std::vector<std::unique_ptr<ISuperviseApplication>> applications_;
 private:
  bool enable_mqtt_ = false;
  std::string program_name_ = "supervise";
  std::string config_file_ = "supervise.xml";

  void FindConfigFile();

};

} // util::supervise


