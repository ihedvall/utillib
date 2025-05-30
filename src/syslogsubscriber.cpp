/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "syslogsubscriber.h"

#include <chrono>
#include <utility>
#include <boost/container/container_fwd.hpp>

#include "util/logstream.h"

using namespace util::log;
using namespace boost::asio;
using namespace boost::system;
using namespace std::chrono_literals;

namespace util::syslog {

SyslogSubscriber::SyslogSubscriber()
 : lookup_(context_),
   retry_timer_(context_) {
  type_ = SyslogServerType::TcpSubscriber;
  operable_ = false;
}

SyslogSubscriber::~SyslogSubscriber() {
  stop_subscriber_ = true;
  SyslogSubscriber::Stop();
}

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
  stop_subscriber_ = false;
  ISyslogServer::Start();
  worker_thread_ = std::thread(&SyslogSubscriber::WorkerTask, this);
}

void SyslogSubscriber::Stop() {
  stop_subscriber_ = true;
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
  try {
    if (socket_ && socket_->is_open()) {

      error_code shutdown_error;
      const error_code sh_error = socket_->shutdown(ip::tcp::socket::shutdown_both, shutdown_error);
      if (sh_error.failed() && !stop_subscriber_) {
        LOG_TRACE() << "Socket shutdown error. Error: " << sh_error.message();
      }
      error_code close_error;
      const error_code cl_error = socket_->close(close_error);
      if (cl_error.failed() && !stop_subscriber_) {
        LOG_TRACE() << "Socket close error. Error: " << cl_error.message();
      }
    }

  } catch (const std::exception& err) {
      LOG_ERROR() << "Close exception error. Error: " << err.what();
  }
}

void SyslogSubscriber::DoLookup() {
  operable_ = false;

  std::string address(Address());
  std::string port(std::to_string(Port()));

  lookup_.async_resolve(ip::tcp::v4(), address, port,
    [&] (const error_code& error, ip::tcp::resolver::results_type result) -> void {
      if (error.failed()) {
        LOG_TRACE() << "Lookup error. Host: " << Address() << ":" << Port()
                          << ",Error (" << error.value() << "): " << error.message();
        DoRetryWait();
      } else if (result.empty()) {
        LOG_TRACE() << "DNS doesn't find any connections. Host: "
          << Address() << ":" << Port();
        DoRetryWait();
      } else {
        socket_ = std::make_unique<ip::tcp::socket>(context_);
        endpoints_ = std::move(result);
        endpoint_itr_ = endpoints_.begin();
        DoConnect();
      }
    });
}

void SyslogSubscriber::DoRetryWait() {
  operable_ = false;
  retry_timer_.expires_after(5s);
  retry_timer_.async_wait([&](const error_code error) {
    if (error ) {
      LOG_TRACE() << "Retry timer error. Error: " << error.message();
      return;
    }
    if (!stop_subscriber_) {
      DoLookup();
    }
  });

  Close();
}

void SyslogSubscriber::DoConnect() {
    auto& endp = *endpoint_itr_;
    ++endpoint_itr_;
     socket_->async_connect(endp, [&] ( error_code error) -> void {
      if (error.failed() ) {
        LOG_TRACE() << "Connect error. Host: " << endp.host_name()
          << ", Service/Port:" << endp.service_name()
          << ", Error: " << error.message();
        if (endpoint_itr_ == endpoints_.end()) {
          DoRetryWait();
        } else {
          socket_ = std::make_unique<ip::tcp::socket>(context_);
          DoConnect();
        }
      } else if (!socket_->is_open()) {
        LOG_TRACE() << "Connect failed. Host: " << endp.host_name()
        << ", Service/Port:" << endp.service_name();
        if (endpoint_itr_ == endpoints_.end()) {
          DoRetryWait();
        } else {
          socket_ = std::make_unique<ip::tcp::socket>(context_);
          DoConnect();
        }
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