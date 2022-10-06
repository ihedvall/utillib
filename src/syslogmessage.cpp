/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/syslogmessage.h"

#include <util/logconfig.h>
#include <util/timestamp.h>

#include <boost/asio.hpp>
#include <sstream>

#include "syslogscanner.h"

using namespace util::log;

namespace {
bool IsUtf8(const std::string &text) {
  return std::ranges::any_of(text, [](const auto &cin) { return cin < 0; });
}
}  // namespace
namespace util::syslog {

SyslogMessage::SyslogMessage() {
  // TIMESTAMP
  timestamp_ = util::time::TimeStampToNs();  // Set it to now

  // HOSTNAME
  try {
    hostname_ = boost::asio::ip::host_name();
  } catch (const std::exception &) {
  }

  // APP-NAME
  application_name_ = LogConfig::Instance().ApplicationName();

  // PID
#if (_MSC_VER)
  const auto pid = _getpid();
#else
  const auto pid = getpid();
#endif

  process_id_ = std::to_string(pid);
}

SyslogMessage::SyslogMessage(const LogMessage &log, bool show_location)
    : SyslogMessage() {
  Message(log.message);

  Facility(SyslogFacility::Local0);

  if (show_location) {
    StructuredData location;
    location.Identity("source_location@37916");  // Volvo Car
    location.AddParameter("Function", log.location.function_name());
    location.AddParameter("File", log.location.file_name());
    location.AddParameter("Column", std::to_string(log.location.column()));
    location.AddParameter("Line", std::to_string(log.location.line()));
    AddData(location);
  }

  switch (log.severity) {
    case LogSeverity::kInfo:
      Severity(SyslogSeverity::Informational);
      break;

    case LogSeverity::kNotice:
      Severity(SyslogSeverity::Notice);
      break;

    case LogSeverity::kWarning:
      Severity(SyslogSeverity::Warning);
      break;

    case LogSeverity::kError:
      Severity(SyslogSeverity::Error);
      break;

    case LogSeverity::kCritical:
      Severity(SyslogSeverity::Critical);
      break;

    case LogSeverity::kAlert:
      Severity(SyslogSeverity::Alert);
      break;
    case LogSeverity::kEmergency:
      Severity(SyslogSeverity::Emergency);
      break;

    case LogSeverity::kTrace:
    case LogSeverity::kDebug:
    default:
      Severity(SyslogSeverity::Debug);
      break;
  }
}
std::string SyslogMessage::GenerateMessage() const {
  std::ostringstream msg;

  //  HEADER

  // PRI
  int pri =
      (static_cast<uint8_t>(facility_) * 8) + static_cast<uint8_t>(severity_);
  msg << "<" << pri << ">";

  // VERSION
  msg << static_cast<int>(version_);

  // TIMESTAMP
  int resolution = 0;  // Second resolution
  if (timestamp_ % 1'000'000 != 0) {
    resolution = 2;
  } else if (timestamp_ % 1'000'000'000 != 0) {
    resolution = 1;
  }

  msg << " " << util::time::NsToIsoTime(timestamp_, resolution);

  // HOSTNAME
  if (hostname_.empty()) {
    msg << " -";
  } else {
    msg << " " << hostname_;
  }

  // APP-NAME
  if (application_name_.empty()) {
    msg << " -";
  } else {
    msg << " " << application_name_;
  }

  // PID
  if (process_id_.empty()) {
    msg << " -";
  } else {
    msg << " " << process_id_;
  }

  // MSG-ID
  if (message_id_.empty()) {
    msg << " -";
  } else {
    msg << " " << message_id_;
  }

  // SD-DATA
  if (sd_list_.empty()) {
    msg << " -";
  } else {
    for (const auto &data : sd_list_) {
      msg << " [" << data.Identity();
      for (const auto &pair : data.Parameters()) {
        msg << " " << pair.first << "=\"" << pair.second << "\"";
      }
      msg << "]";
    }
  }
  if (!message_.empty()) {
    if (IsUtf8(message_)) {
      msg << " " << '\xEF' << '\xBB' << '\xBF' << message_;
    } else {
      msg << " " << message_;
    }
  }
  return msg.str();
}

void SyslogMessage::IsoTime(const std::string &iso_time) {
  // Check if it is a correct time. Note that syslog protocol may return a "-"
  // indicating that the sender doesn't have time. In this case, use the
  // computer time.
  if (iso_time.empty() || iso_time == "-") {   //
    timestamp_ = util::time::TimeStampToNs();  // Use now as
  } else {
    timestamp_ = util::time::IsoTimeToNs(iso_time, false);
  }
}

void SyslogMessage::Hostname(const std::string &name) {
  if (name.empty() || name == "-") {
    hostname_.clear();
  } else {
    hostname_ = name;
  }
}

void SyslogMessage::ApplicationName(const std::string &name) {
  if (name.empty() || name == "-") {
    application_name_.clear();
  } else {
    application_name_ = name;
  }
}

void SyslogMessage::ProcessId(const std::string &pid) {
  if (pid.empty() || pid == "-") {
    process_id_.clear();
  } else {
    process_id_ = pid;
  }
}

void SyslogMessage::MessageId(const std::string &msg_id) {
  if (msg_id.empty() || msg_id == "-") {
    message_id_.clear();
  } else {
    message_id_ = msg_id;
  }
}

void SyslogMessage::AddStructuredData(const std::string &identity) {
  StructuredData data;
  data.Identity(identity);

  if (!data.Identity().empty()) {
    sd_list_.emplace_back(data);
  }
}

void SyslogMessage::AppendParameter(const std::string &name,
                                    const std::string &value) {
  if (sd_list_.empty()) {
    return;
  }
  auto &last = sd_list_.back();
  last.AddParameter(name, value);
}

void SyslogMessage::Message(const std::string &msg) {
  // Remove any BOM from the string
  bool bom = false;
  if (msg.size() > 2) {
    const auto byte1 = static_cast<uint8_t>(msg[0]);
    const auto byte2 = static_cast<uint8_t>(msg[1]);
    const auto byte3 = static_cast<uint8_t>(msg[2]);
    bom = (byte1 == 0xEF) && (byte2 == 0xBB) && (byte3 == 0xBF);
  }
  message_ = bom ? msg.substr(3) : msg;
}

bool SyslogMessage::ParseMessage(const std::string &msg) {
  std::istringstream temp(msg);
  syslog::SyslogScanner scanner(temp);
  syslog::SyslogParser parser(scanner);
  const auto ret = parser.parse();
  const SyslogMessage *slicing_fix = &scanner;
  *this = *slicing_fix;
  return ret == 0;
}

}  // namespace util::syslog
