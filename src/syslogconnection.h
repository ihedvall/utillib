/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <util/syslogmessage.h>

#include <boost/asio.hpp>

namespace util::syslog {

class ISyslogServer;

class SyslogConnection {
 public:
  SyslogConnection(ISyslogServer& server,
                   std::unique_ptr<boost::asio::ip::tcp::socket>& socket);
  ~SyslogConnection();

  SyslogConnection() = delete;
  SyslogConnection(SyslogConnection&) = delete;
  SyslogConnection& operator=(SyslogConnection&) = delete;

  bool Cleanup();

  void SendSyslogMessage(const SyslogMessage& message);

 private:
  ISyslogServer& server_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  size_t length_ = 0;
  std::string length_buffer_ = " ";
  std::string msg_buffer_;
  void Close();  ///< Closes the socket connection

  void DoReadLength();
  void DoReadMessage();
};

}  // namespace util::syslog
