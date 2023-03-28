/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <boost/asio.hpp>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include "syslogconnection.h"
#include "util/isyslogserver.h"
namespace util::syslog {

class TcpSyslogServer : public ISyslogServer {
 public:
  TcpSyslogServer();
  TcpSyslogServer(const TcpSyslogServer&) = delete;
  ~TcpSyslogServer() override;

  void Start() override;
  void Stop() override;

  [[nodiscard]] size_t NofConnections() const override;

 private:
  boost::asio::io_context context_;
  boost::asio::steady_timer cleanup_timer_;
  std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  std::thread server_thread_;
  mutable std::mutex connection_list_lock_;
  std::deque<std::unique_ptr<SyslogConnection>> connection_list_;

  void ServerThread();
  void DoAccept();
  void DoCleanupConnections();
};

}  // namespace util::syslog
