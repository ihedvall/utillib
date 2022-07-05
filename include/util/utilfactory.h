/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file utilfactory.h
 * Implement the creators for most of the interface related classes.
 */
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "util/isyslogserver.h"
#include "util/ilogger.h"
namespace util {
/** \class UtilFactory utilfactory.h "util/utilfactory.h"
 * \brief Implements creators for most interface classes.
 */
class UtilFactory {
 public:
  /** \brief Creates different types of syslog servers
   *
   * Creates a syslog server. Choose between UDP, TLS or TCP servers.
   * @param type Type of syslog protocol-
   * @return Smart pointer to a syslog server.
   */
  static std::unique_ptr<syslog::ISyslogServer> CreateSyslogServer(syslog::SyslogServerType type);

  /** \brief Creates a pre-defined log source.
   *
   *  Creates a pre-defined log source. Choose between console, file, listen or syslog sources.
   *  The file source requires a base name (no path or extension) as input argument. The syslog require 2
   *  arguments, remote host name and port.
   *
   * @param type Type of log source.
   * @param arg_list Extra arguments
   * @return Smart pointer to a log source.
   */
  static std::unique_ptr<log::ILogger> CreateLogger(log::LogType type, std::vector<std::string>& arg_list);
};

} // util
