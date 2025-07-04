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

#include "util/ilisten.h"
#include "util/ilistenclient.h"
#include "util/ilogger.h"
#include "util/isyslogserver.h"
#include "util/isupervisemaster.h"

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
  static std::unique_ptr<syslog::ISyslogServer> CreateSyslogServer(
      syslog::SyslogServerType type);

  /** \brief Creates a pre-defined log source.
   *
   *  Creates a pre-defined log source. Choose between console, file, listen or
   * syslog sources. The file source requires a base name (no path or extension)
   * as input argument. The syslog require 2 arguments, remote host name and
   * port.
   *
   * @param type Type of log source.
   * @param arg_list Extra arguments
   * @return Smart pointer to a log source.
   */
  static std::unique_ptr<log::ILogger> CreateLogger(
      log::LogType type, const std::vector<std::string> &arg_list);

  /** \brief Creates a listen object.
   *
   * Creates a listen object. It only exist 3 basic objects to create.
   *
   * The 'ListenProxy' object sends messages through a shared memory to a common
   * server object. This type of listen object is normal when several executable
   * should share a listen 'channel'. Typically is when debugging database
   * calls.
   *
   * The 'ListenServer' shall be used when the application want its own listen
   * 'channel'. This is typically used for protocol drivers.
   *
   * The 'ListenConsole' logs the message onto a console window (std::cout). It
   * shall be used when putting messages from a proxy onto a console window. This
   * object is mainly used for testing.
   *
   * @param type Select between 'ListenProxy','ListenServer' or 'ListenConsole'
   * @param share_name Unique share name or empty string.
   * @return A smart pointer to a listen object.
   */
  [[deprecated("Use util::UtilFactory::CreateListen with enumeration instead")]]
  static std::unique_ptr<log::IListen> CreateListen(
      const std::string &type, const std::string &share_name);

  /** \brief Creates a listen object.
   *
   * Creates a listen (server) object.
   * A listen object is an abstract object that is used to debug inside
   * application.
   * This enables simple debugging in delivered system.
   * The user simply generates debug text lines that the Listen object forward
   * to a Listen Window.
   * The Listen Window (listenviewer.exe) GUI present the debug text lines.
   * The trick is that when no Listen Window is open, the Listen object goes
   * into idle mode not disturbing or delaying the application software.
   * It exist 3 basic objects to create.
   *
   * The ListenProxyType object sends messages through a shared memory to a common
   * server object. This type of listen object is normal when several executable
   * should share a listen 'channel'. Typical application is when debugging database
   * calls.
   *
   * The 'ListenServer' shall be used when the application want its own listen
   * 'channel'. Typical application is when protocol drivers.
   *
   * The 'ListenConsole' logs the message onto a console window (std::cout). It
   * shall be used when putting messages from a proxy onto a console window. This
   * object is mainly used for testing.
   *
   * @param type Select between ListenProxyType,ListenServerType'
   * or ListenConsoleType
   * @param share_name Unique share name or empty string.
   * @return A smart pointer to a listen object.
   */
  static std::unique_ptr<log::IListen> CreateListen(
      log::TypeOfListen type, const std::string &share_name);
  /** \brief Creates a listen client.
   *
   * Creates a TCP/IP listen client object.
   * @param host Host name
   * @param port TCP/IP port to connect to.
   * @return Smart pointer to a IListenClient.
   */
  static std::unique_ptr<log::IListenClient> CreateListenClient(
      const std::string &host, uint16_t port);

  /** \brief Create a supervise master object.
   *
   * Creates a supervise
   * @param type
   * @return
   */
  static std::unique_ptr<supervise::ISuperviseMaster> CreateSuperviseMaster(
      supervise::TypeOfSuperviseMaster type);
};

}  // namespace util
