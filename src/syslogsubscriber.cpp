/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "syslogsubscriber.h"

#include <chrono>

#include "util/logstream.h"

using namespace util::log;
using namespace boost::asio;
using namespace boost::system;
using namespace std::chrono_literals;

namespace util::syslog {

SyslogSubscriber::SyslogSubscriber()
    : retry_timer_(context_), lookup_(context_) {
  type_ = SyslogServerType::TcpSubscriber;
  operable_ = false;
}

SyslogSubscriber::~SyslogSubscriber() { SyslogSubscriber::Stop(); }

void SyslogSubscriber::WorkerTask() {
  try {
    DoLookup();
    const auto count = context_.run();
    LOG_TRACE() << "Stopped main worker thread. Count: " << count;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Context error. Error: " << error.what();
  }
}

void SyslogSubscriber::Start() {
  ISyslogServer::Start();
  worker_thread_ = std::thread(&SyslogSubscriber::WorkerTask, this);
}

void SyslogSubscriber::Stop() {
  Close();
  if (!context_.stopped()) {
    context_.stop();
  }
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
  ISyslogServer::Stop();
  operable_ = false;
}

void SyslogSubscriber::Close() {
  if (socket_ && socket_->is_open()) {
    error_code dummy;
    socket_->shutdown(ip::tcp::socket::shutdown_both, dummy);
    socket_->close(dummy);
  }
  socket_.reset();
}

void SyslogSubscriber::DoLookup() {
  operable_ = false;
  lookup_.async_resolve(
      ip::tcp::v4(), Address(), std::to_string(Port()),
      [&](const error_code& error, ip::tcp::resolver::results_type result) {
        if (error) {
          LOG_TRACE() << "Lookup error. Host: " << Address() << ":" << Port()
                      << ",Error: (" << error << ") " << error.message();
          DoRetryWait();
        } else {
          socket_ = std::make_unique<ip::tcp::socket>(context_);
          endpoints_ = std::move(result);

          DoConnect();
        }
      });
}

void SyslogSubscriber::DoRetryWait() {
  operable_ = false;
  retry_timer_.expires_after(5s);
  retry_timer_.async_wait([&](const error_code error) {
    if (error) {
      LOG_TRACE() << "Retry timer error. Error: " << error.message();
      return;
    }
    Close();
    DoLookup();
  });
}
void SyslogSubscriber::DoConnect() {
  socket_->async_connect(*endpoints_, [&](const error_code error) {
    if (error) {
      LOG_TRACE() << "Connect error. Error: " << error.message();
      DoRetryWait();
    } else {
      operable_ = true;
      length_ = 0;
      DoReadLength();
    }
  });
}

void SyslogSubscriber::DoReadLength() {  // NOLINT
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  async_read(*socket_, buffer(length_buffer_.data(), 1),
             [&](const error_code& error, std::size_t bytes) {  // NOLINT
               if (error) {
                 DoRetryWait();
               } else {
                 char input = length_buffer_[0];
                 if (isdigit(input)) {
                   length_ *= 10;
                   length_ += input - '0';
                   DoReadLength();
                 } else {
                   if (length_ > 0) {
                     DoReadMessage();
                   } else {
                     length_ = 0;
                     DoReadLength();
                   }
                 }
               }
             });
}
void SyslogSubscriber::DoReadMessage() {  // NOLINT
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  msg_buffer_.clear();
  msg_buffer_.resize(length_, '\0');
  async_read(*socket_, buffer(msg_buffer_.data(), length_),
             [&](const error_code& error, std::size_t bytes) {  // NOLINT
               if (error) {
                 LOG_TRACE()
                     << "Read message error. Error: " << error.message();
                 DoRetryWait();
               } else {
                 SyslogMessage message;
                 const auto parse = message.ParseMessage(msg_buffer_);
                 if (parse) {
                   AddMsg(message);
                 } else {
                   LOG_TRACE() << "Parse error: " << msg_buffer_;
                 }
                 length_ = 0;
                 DoReadLength();
               }
             });
}

}  // namespace util::syslog