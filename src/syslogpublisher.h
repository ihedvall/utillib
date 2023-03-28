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

class SyslogPublisher : public ISyslogServer {
 public:
  SyslogPublisher();
  SyslogPublisher(const SyslogPublisher&) = delete;
  ~SyslogPublisher() override;
  void Start() override;
  void Stop() override;

  void AddMsg(const SyslogMessage& message) override;

  [[nodiscard]] size_t NofConnections() const override;

 private:
  boost::asio::io_context context_;
  boost::asio::steady_timer cleanup_timer_;
  boost::asio::steady_timer check_list_timer_;
  std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  std::thread server_thread_;
  mutable std::mutex connection_list_lock_;
  std::mutex message_list_lock_;
  std::deque<std::unique_ptr<SyslogConnection>> connection_list_;

  std::list<SyslogMessage> message_list_;  ///< Temporary storage of messages
  void ServerThread();
  void DoAccept();
  void DoCleanupConnections();
  void DoCheckMessageList();
};

}  // namespace util::syslog
