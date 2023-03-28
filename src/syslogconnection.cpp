/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "syslogconnection.h"

#include <util/logstream.h>

#include "util/isyslogserver.h"

using namespace boost::asio;
using namespace boost::system;
using namespace util::log;

namespace util::syslog {

SyslogConnection::SyslogConnection(
    ISyslogServer& server,
    std::unique_ptr<boost::asio::ip::tcp::socket>& socket)
    : server_(server), socket_(std::move(socket)) {
  try {
    socket_base::keep_alive option(true);
    socket_->set_option(option);
  } catch (const std::exception& err) {
    // we can live without a keep alive function
    LOG_TRACE() << "Keep-alive option failed. Error: " << err.what();
  }

  length_ = 0;
  DoReadLength();
}

SyslogConnection::~SyslogConnection() {
  try {
    if (socket_ && socket_->is_open()) {
      socket_->close();
    }
  } catch (const std::exception& err) {
    LOG_TRACE() << "Error closing connection. Error: " << err.what();
  }
  socket_.reset();
}

bool SyslogConnection::Cleanup() { return !socket_ || !socket_->is_open(); }

void SyslogConnection::Close() {
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  try {
    socket_->shutdown(ip::tcp::socket::shutdown_both);
    socket_->close();
    socket_.reset();
  } catch (const std::exception& err) {
    LOG_TRACE() << "Improper shutdown. Error: " << err.what();
  }
}

void SyslogConnection::DoReadLength() {  // NOLINT
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  async_read(*socket_, buffer(length_buffer_.data(), 1),
             [&](const error_code& error, std::size_t bytes) {  // NOLINT
               if (error && error == error::eof) {
                 Close();  // Connection closed by remote client
               } else if (error) {
                 LOG_ERROR() << "Read length error. Error: " << error.message();
                 Close();
               } else {
                 char input = length_buffer_[0];
                 if (isdigit(input)) {
                   length_ *= 10;
                   length_ += input - '0';
                   DoReadLength();
                 } else if (length_ > 0) {
                   DoReadMessage();
                 } else {
                   length_ = 0;
                   DoReadLength();
                 }
               }
             });
}

void SyslogConnection::DoReadMessage() {  // NOLINT
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  msg_buffer_.clear();
  msg_buffer_.resize(length_, '\0');
  async_read(*socket_, buffer(msg_buffer_.data(), length_),
             [&](const error_code& error, std::size_t bytes) {  // NOLINT
               if (error && error == error::eof) {
                 Close();  // Connection closed by remote client
               } else if (error) {
                 LOG_ERROR()
                     << "Read message error. Error: " << error.message();
                 Close();
               } else {
                 SyslogMessage message;
                 const auto parse = message.ParseMessage(msg_buffer_);
                 if (parse && server_.Type() == SyslogServerType::TcpServer) {
                   server_.AddMsg(message);
                 } else {
                   LOG_TRACE() << "Parse Error: " << msg_buffer_;
                 }
                 length_ = 0;
                 DoReadLength();
               }
             });
}

void SyslogConnection::SendSyslogMessage(const SyslogMessage& message) {
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  const auto msg_text = message.GenerateMessage();
  std::ostringstream temp;
  temp << msg_text.size() << " " << msg_text;
  try {
    socket_->write_some(buffer(temp.str()));
  } catch (const std::exception& err) {
    LOG_TRACE() << "Send error: " << err.what();
    Close();
  }
}

}  // namespace util::syslog