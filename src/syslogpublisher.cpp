/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "syslogpublisher.h"

#include <algorithm>
#include <boost/asio.hpp>
#include <chrono>
#include <ranges>
#include <sstream>

#include "util/logstream.h"
#include "util/stringutil.h"
using namespace boost::asio;
using namespace std::chrono_literals;
using namespace util::log;
using namespace util::string;

namespace util::syslog {

SyslogPublisher::SyslogPublisher()
    : ISyslogServer(), cleanup_timer_(context_), check_list_timer_(context_) {
  type_ = SyslogServerType::TcpPublisher;
  Address("127.0.0.1");  // Change default to enable only localhost
}

SyslogPublisher::~SyslogPublisher() { SyslogPublisher::Stop(); }

void SyslogPublisher::Start() {
  if (Name().empty()) {
    std::ostringstream temp;
    temp << "TCP Syslog Publisher:" << Port();
    Name(temp.str());
  }
  ISyslogServer::Start();
  try {
    if (Address().empty() || IEquals(Address(), "0.0.0.0")) {
      const auto address = ip::address_v4::any();
      const ip::tcp::endpoint endpoint(address, Port());
      acceptor_ = std::make_unique<ip::tcp::acceptor>(context_, endpoint);
    } else {
      const auto address = ip::address::from_string("127.0.0.1");
      const ip::tcp::endpoint endpoint(address, Port());
      acceptor_ = std::make_unique<ip::tcp::acceptor>(context_, endpoint);
    }

    DoAccept();              // Accept all incoming connections
    DoCleanupConnections();  // Cleanup unused connections
    DoCheckMessageList();
    server_thread_ = std::thread(&SyslogPublisher::ServerThread, this);
    operable_ = true;
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to start receiver thread. Name: " << Name()
                << ", Error: " << err.what();
    operable_ = false;
  }
}

void SyslogPublisher::Stop() {
  if (context_.stopped()) {
    return;
  }
  try {
    {
      std::lock_guard lock(connection_list_lock_);
      connection_list_.clear();
    }
    if (!context_.stopped()) {
      context_.stop();
    }
    if (server_thread_.joinable()) {
      server_thread_.join();
    }
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to stop the server. Name: " << Name()
                << ", Error: " << error.what();
  }
}

void SyslogPublisher::ServerThread() {
  try {
    const auto& count = context_.run();
    LOG_TRACE() << "Stopped main worker thread. Name: " << Name()
                << ", Count: " << count;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Context error. Name: " << Name()
                << ", Error: " << error.what();
  }
}

void SyslogPublisher::DoAccept() {
  socket_ = std::make_unique<ip::tcp::socket>(context_);
  acceptor_->async_accept(
      *socket_, [&](const boost::system::error_code& error) {
        if (error) {
          socket_.reset();
          LOG_ERROR() << "Accept error. Name: " << Name()
                      << ", Error: " << error.message();
        } else {
          auto connection = std::make_unique<SyslogConnection>(*this, socket_);
          {
            std::lock_guard lock(message_list_lock_);
            for (const auto& msg : message_list_) {
              connection->SendSyslogMessage(msg);
            }
          }

          {
            std::lock_guard lock(connection_list_lock_);
            connection_list_.push_back(std::move(connection));
          }
          socket_.reset();
          DoAccept();
        }
      });
}

void SyslogPublisher::DoCleanupConnections() {
  cleanup_timer_.expires_after(2s);
  cleanup_timer_.async_wait([&](const boost::system::error_code error) {
    if (error) {
      LOG_ERROR() << "Cleanup timer error. Name: " << Name()
                  << ", Error: " << error.message();
    } else {
      std::lock_guard lock(connection_list_lock_);
      for (auto itr = connection_list_.begin(); itr != connection_list_.end();
           /* No ++itr here */) {
        auto& connection = *itr;
        if (!connection || connection->Cleanup()) {
          LOG_TRACE() << "Deleted a connection";
          itr = connection_list_.erase(itr);
        } else {
          ++itr;
        }
      }
      DoCleanupConnections();
    }
  });
}

void SyslogPublisher::DoCheckMessageList() {
  check_list_timer_.expires_after(60s);
  check_list_timer_.async_wait([&](const boost::system::error_code error) {
    if (error) {
      LOG_ERROR() << "Check list timer error. Name: " << Name()
                  << ", Error: " << error.message();
    } else {
      {
        std::lock_guard lock(message_list_lock_);
        while (message_list_.size() > 50) {
          message_list_.pop_front();
        }
      }
      DoCheckMessageList();
    }
  });
}

void SyslogPublisher::AddMsg(const SyslogMessage& message) {
  // Not sending to the internal queue
  {
    std::lock_guard lock(message_list_lock_);
    message_list_.push_back(message);
  }
  std::lock_guard lock(connection_list_lock_);
  for (auto& connection : connection_list_) {
    if (!connection || connection->Cleanup()) {
      continue;
    }
    connection->SendSyslogMessage(message);
  }
}

size_t SyslogPublisher::NofConnections() const {
  std::lock_guard lock(connection_list_lock_);
  return connection_list_.size();
}

}  // namespace util::syslog