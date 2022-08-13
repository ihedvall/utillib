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
#include "util/ipubsubclient.h"
#include "util/ilisten.h"

namespace util {

enum class PubSubType : int {
  Mqtt3Client = 0, ///< MQTT 3.11 client interface.
  Mqtt5Client = 1, ///< MQTT 5 client interface.
  SparkPlugClient = 2, ///< MQTT version 3.11 with SparkPlug interface.
  KafkaClient = 3, ///< Kafka client.
};

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

  /** \brief Creates a publisher client interface.
   *
   *  Creates a pre-defined publisher source. Currently on MQTT is available.
   *
   * @param type Type of publisher.
   * @return Smart pointer to a Pub/Sub client source.
   */
  static std::unique_ptr<mqtt::IPubSubClient> CreatePubSubClient(PubSubType type);

  /** \brief Creates a listen object.
   *
   * Creates a listen object. It only exist 3 basic objects to create.
   *
   * The 'ListenProxy' object sends messages through a shared memory to a common server object. This
   * type of listen object is normal when several executable should share a listen 'channel'. Typically
   * is when debugging database calls.
   *
   * The 'ListenServer' shall be used when the application want its own listen 'channel'. This is
   * typically used for protocol drivers.
   *
   * The 'ListenConsole' logs the message onto a console window (std::cout). It shallbe used
   * when putting messages from a proxy onto a console window. This object is mainly used for testing.
   *
   * @param type Select between 'ListenProxy' or 'ListenServer'
   * @param share_name Unique share name or empty string.
   * @return A smart pointer to a listen object.
   */
  static std::unique_ptr<log::IListen> CreateListen(const std::string& type, const std::string& share_name);
};

} // util
