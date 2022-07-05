/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include "util/isyslogserver.h"

namespace util::syslog {

class UdpSyslogServer : public ISyslogServer {
 public:
  void Start() override;
  void Stop() override;
 private:
  std::thread server_thread_;
  std::atomic<bool> stop_thread_ = false;
  boost::asio::io_context io_context_;
  std::unique_ptr<boost::asio::ip::udp::socket> socket_;

  void ServerThread();
};

} // end namespace