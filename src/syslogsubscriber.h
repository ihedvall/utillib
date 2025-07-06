/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <atomic>

#include <memory>
#include <thread>

#include <boost/asio.hpp>
#include "util/isyslogserver.h"

namespace util::syslog {

class SyslogSubscriber : public ISyslogServer {
 public:
  SyslogSubscriber();

  SyslogSubscriber(const SyslogSubscriber &) = delete;

  ~SyslogSubscriber() override;

  void Start() override;

  void Stop() override;

 private:
  boost::asio::io_context context_;
  boost::asio::ip::tcp::resolver lookup_;
  boost::asio::steady_timer retry_timer_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  // This is a list of endpoints, so it has to iterate through them
  // and trying to connect.
  boost::asio::ip::tcp::resolver::results_type endpoints_;
  boost::asio::ip::tcp::resolver::results_type::iterator endpoint_itr_;
  std::string length_buffer_ = " ";
  std::string msg_buffer_;
  size_t length_ = 0;
  std::atomic<bool> stop_subscriber_ = false;
  std::thread worker_thread_;

  void WorkerTask();

  void Close();

  void DoLookup();

  void DoConnect();

  void DoRetryWait();

  void DoReadLength();

  void DoReadMessage();
};

}  // namespace util::syslog
