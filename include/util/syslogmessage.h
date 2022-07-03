/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file syslogmessage.h
 * \brief Implements an interface against a syslog message.
 *
 * The file implements an interface against a syslog message. The
 * syslog messages are sent to a syslog server.
 */
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "logmessage.h"
#include "structureddata.h"
namespace util::syslog {

/** \enum SyslogSeverity
 * \brief Syslog severity codes
 *
 */
enum class SyslogSeverity : uint8_t {
  Emergency = 0,     ///< System is unusable.
  Alert = 1,         ///< Action must be taken immediately.
  Critical = 2,      ///< Critical condition.
  Error = 3,         ///< Error condition.
  Warning = 4,       ///< Warning condition.
  Notice = 5,        ///< Normal but significant condition.
  Informational = 6, ///< Informational message.
  Debug = 7          ///< Debug message.
};

/** \enum SyslogFacility
 * \brief Syslog facility codes.
 *
 */
enum class SyslogFacility : uint8_t {
  Kernel = 0,
  UserLevel = 1,
  Mail = 2,
  SystemDaemon = 3,
  Security = 4,
  Internal = 5,
  Printer = 6,
  Network = 7,
  UUCP = 8,
  Clock = 9,
  Authorization = 10,
  FTP = 11,
  NTP = 12,
  LogAudit = 13,
  LogAlert = 14,
  ClockDaemon = 15,
  Local0 = 16,
  Local1 = 17,
  Local2 = 18,
  Local3 = 19,
  Local4 = 20,
  Local5 = 21,
  Local6 = 22,
  Local7 = 23
};

/** \class SyslogMessage syslogmessage.h "util/syslogmessage.h"
 * \brief Simple wrapper around the parsing and generating os syslog messages.
 *
 * The class is a simple interface against a syslog message according to
 * the RFC5424 standard.
 */
class SyslogMessage {
 public:
  /** \brief Standard constructor
   *
   * Standard constructor that enables creating and parsing of system
   * log messages.
   */
  SyslogMessage();

  /** \brief Constructor that converts a log message,
   *
   * Constructor used when converting a log message into a syslog message, The
   * location flag should be set if the source location should be included.
   * @param log Log message
   * @param show_location Set tor true if source location should be included.
   */
  SyslogMessage(const util::log::LogMessage& log, bool show_location);

  void Severity(SyslogSeverity severity) { ///< Sets the severity level
    severity_ = severity;
  }
  [[nodiscard]] SyslogSeverity Severity() const { ///< Returns the severity level
    return severity_;
  }

  void Facility(SyslogFacility facility) { ///< Sets the facility code
    facility_ = facility;
  }

  [[nodiscard]] SyslogFacility Facility() const { ///< Returns the facility code.
    return facility_;
  }



  [[nodiscard]] uint8_t Version() const { ///< Returns the version.
    return version_;
  }

  void Timestamp(uint64_t ns1970) { ///< Sets the timestamp (ns since 1970).
    timestamp_ = ns1970;
  }

  [[nodiscard]] uint64_t Timestamp() const { ///< Returns the timestamp (ns since 1970).
    return timestamp_;
  }

  void Hostname(const std::string& name); ///< Sets the host name
  [[nodiscard]] const std::string& Hostname() const { ///< Returns the host name
    return hostname_;
  }

  void ApplicationName(const std::string& name); ///< Sets the application name
  [[nodiscard]] const std::string& ApplicationName() const { ///< Returns the application name
    return application_name_;
  }

  void ProcessId(const std::string& pid); ///< Sets the PID
  [[nodiscard]] const std::string& ProcessId() const { ///< Returns the PID
    return process_id_;
  }

  void MessageId(const std::string& msg_id); ///< Sets the message ID
  [[nodiscard]] const std::string& MessageId() const { ///< Returns the message ID
    return message_id_;
  }

  /** \brief Sets the message test.
   *
   * Sets the message text. Note that it assume that the string is UTF8 decoded.
   * @param msg UTF8 decoded string.
   */
  void Message(const std::string& msg);

  /** \brief Returns the message text.
   *
   * Returns the message text. Note that the string is UTF8 decoded.
   * @return Messag text.
   */
  [[nodiscard]] const std::string& Message() const {
    return message_;
  }

  void AddData(const StructuredData& data) { ///< Adds structured data to the message.
    sd_list_.push_back(data);
  }

  [[nodiscard]] const std::vector<StructuredData>& DataList() const { ///< Returns a list of structured data items.
    return sd_list_;
  }

  [[nodiscard]] std::string GenerateMessage() const; ///< Generates a syslog message.
  bool ParseMessage(const std::string& msg); ///< Parses a syslog message.

  void AddStructuredData(const std::string& identity); ///< Adds a structured data item without any parameters.
  void AppendParameter(const std::string& name, const std::string& value); ///< Append parameter item to the last data item.
  void Version(uint8_t version) { ///< Sets the version number. Default is 1.
    version_ = version;
  }

  /** \brief Sets the timestamp by converting a ISO time string.
  *
  * Function that is used when parsing syslog messages and converts
  * the incoming ISO time string into a timestamp.
  * @param iso_time ISO time formatted string.
  */
  void IsoTime(const std::string& iso_time);

 protected:

 private:
  SyslogSeverity severity_ = SyslogSeverity::Informational;
  SyslogFacility facility_ = SyslogFacility::Local0;
  uint8_t version_ = 1;
  uint64_t timestamp_ = 0;
  std::string hostname_;
  std::string application_name_;
  std::string process_id_;
  std::string message_id_;

  std::string message_;

  std::vector<StructuredData> sd_list_;
};

} // end namespace util::log