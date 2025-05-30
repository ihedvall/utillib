/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "listenclient.h"

#include "listenmessage.h"
#include "util/logstream.h"
using namespace util::log;
using namespace boost::asio;
using namespace boost::system;
using namespace std::chrono_literals;

namespace util::log::detail {

ListenClient::ListenClient(const std::string& host_name, uint16_t port)
    : IListenClient(host_name, port),
      retry_timer_(context_),
      lookup_(context_) {
  worker_thread_ = std::thread(&ListenClient::WorkerTask, this);
}

ListenClient::~ListenClient() {
  if (!context_.stopped()) {
    context_.stop();
    Close();
  }
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
}

void ListenClient::WorkerTask() {
  try {
    DoLookup();
    const auto& count = context_.run();
    LOG_TRACE() << "Stopped main worker thread";
  } catch (const std::exception& err) {
    LOG_ERROR() << "Context error. Error: " << err.what();
  }
}

void ListenClient::DoLookup() {
  connected_ = false;
  lookup_.async_resolve(
      ip::tcp::v4(), HostName(), std::to_string(Port()),
      [&](const error_code& error, ip::tcp::resolver::results_type result) {
        if (error) {
          LOG_DEBUG() << "Lookup error. Host: " << HostName() << ":" << Port()
                      << ",Error: (" << error << ") " << error.message();
          DoRetryWait();
        } else {
          socket_ = std::make_unique<ip::tcp::socket>(context_);
          endpoints_ = std::move(result);
          DoConnect();
        }
      });
}

void ListenClient::DoRetryWait() {
  Close();
  connected_ = false;
  retry_timer_.expires_after(5s);
  retry_timer_.async_wait([&](const error_code error) {
    if (error) {
      LOG_ERROR() << "Retry timer error. Error: " << error.message();
    }
    DoLookup();
  });
}
void ListenClient::DoConnect() {
  bool connected = false;
  for (const auto& endpoint : endpoints_) {
    socket_->async_connect(endpoint, [&](const error_code error) {
    if (error.failed() || !socket_->is_open()) {
      LOG_ERROR() << "Connect error. Error: " << error.message();
      connected = false;
    } else {
      connected = true;
    }
  });
    if (connected) {
      break;
    }
  }
  if (!connected) {
    DoRetryWait();
  } else {
    DoReadHeader();
  }
 }

void ListenClient::DoReadHeader() {  // NOLINT
  if (!socket_ || !socket_->is_open()) {
    DoRetryWait();
    return;
  }
  connected_ = true;
  async_read(*socket_, buffer(header_data_),
             [&](const error_code& error, size_t bytes) {  // NOLINT
               if (error && error == error::eof) {
                 LOG_INFO() << "Connection closed by remote";
                 DoRetryWait();
               } else if (error) {
                 LOG_ERROR()
                     << "Listen header error. Error: " << error.message();
                 DoRetryWait();
               } else if (bytes != header_data_.size()) {
                 LOG_ERROR() << "Listen header length error. Error: "
                             << error.message();
                 DoRetryWait();
               } else {
                 ListenMessage header;
                 header.FromHeaderBuffer(header_data_);
                 if (header.body_size_ > 0) {
                   body_data_.clear();
                   body_data_.resize(header.body_size_, 0);
                   DoReadBody();
                 } else {
                   DoReadHeader();
                 }
               }
             });
}

void ListenClient::DoReadBody() {  // NOLINT
  if (!socket_ || !socket_->is_open()) {
    DoRetryWait();
    return;
  }
  async_read(*socket_, buffer(body_data_),
             [&](const error_code& error, size_t bytes) {  // NOLINT
               if (error) {
                 LOG_ERROR() << "Listen body error. Error: " << error.message();
                 DoRetryWait();
               } else if (bytes != body_data_.size()) {
                 LOG_ERROR()
                     << "Listen body length error. Error: " << error.message();
                 DoRetryWait();
               } else {
                 HandleMessage();
                 DoReadHeader();
               }
             });
}

void ListenClient::HandleMessage() {
  ListenMessage header;
  header.FromHeaderBuffer(header_data_);
  std::unique_ptr<ListenMessage> message;
  switch (header.type_) {
    case ListenMessageType::LogLevelText: {
      auto msg = std::make_unique<LogLevelTextMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      message = std::move(msg);
      break;
    }

    case ListenMessageType::TextMessage: {
      auto msg = std::make_unique<ListenTextMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      message = std::move(msg);
      break;
    }

    case ListenMessageType::LogLevel: {
      auto msg = std::make_unique<LogLevelMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      message = std::move(msg);
      break;
    }
    default: {
      LOG_DEBUG() << "Unknown message type. Type: "
                  << static_cast<int>(header.type_);
      break;
    }
  }
  if (message) {
    msg_queue_.Put(message);
  }
}

void ListenClient::Close() {
  if (socket_) {
    boost::system::error_code dummy;
    socket_->shutdown(ip::tcp::socket::shutdown_both, dummy);
    socket_->close(dummy);
  }
  socket_.reset();
}

void ListenClient::SendLogLevel(uint64_t level) {
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  LogLevelMessage msg;
  msg.log_level_ = level;
  std::vector<uint8_t> data;
  msg.ToBuffer(data);
  boost::system::error_code error;
  write(*socket_, boost::asio::buffer(data), error);
}

bool ListenClient::GetMsg(std::unique_ptr<ListenMessage>& message) {
  return msg_queue_.Get(message, false);
}

}  // namespace util::log::detail
