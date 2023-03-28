/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "util/isyslogserver.h"

namespace util::syslog {

class UdpSyslogServer : public ISyslogServer {
 public:
  UdpSyslogServer();
  UdpSyslogServer(const UdpSyslogServer&) = delete;
  ~UdpSyslogServer() override;

  void Start() override;
  void Stop() override;

 private:
  std::thread server_thread_;
  std::atomic<bool> stop_thread_ = false;
  boost::asio::io_context io_context_;
  std::unique_ptr<boost::asio::ip::udp::socket> socket_;

  void ServerThread();
};

}  // namespace util::syslog