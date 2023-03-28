/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "tcpsyslogserver.h"

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

TcpSyslogServer::TcpSyslogServer() : ISyslogServer(), cleanup_timer_(context_) {
  type_ = SyslogServerType::TcpServer;
  Address("127.0.0.1");  // Change default to enable only localhost
}

TcpSyslogServer::~TcpSyslogServer() { Stop(); }

void TcpSyslogServer::Start() {
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
    server_thread_ = std::thread(&TcpSyslogServer::ServerThread, this);
    operable_ = true;
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to start receiver thread. Name: " << Name()
                << ", Error: " << err.what();
    operable_ = false;
  }
}

void TcpSyslogServer::Stop() {
  try {
    if (!context_.stopped()) {
      context_.stop();
    }
    if (server_thread_.joinable()) {
      server_thread_.join();
    }
    std::lock_guard lock(connection_list_lock_);
    connection_list_.clear();
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to stop the server. Name: " << Name()
                << ", Error: " << error.what();
  }
}

void TcpSyslogServer::ServerThread() {
  try {
    const auto& count = context_.run();
    LOG_TRACE() << "Stopped main worker thread. Name: " << Name()
                << ", Count: " << count;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Context error. Name: " << Name()
                << ", Error: " << error.what();
  }
}

void TcpSyslogServer::DoAccept() {
  auto connection_socket = std::make_unique<ip::tcp::socket>(context_);
  acceptor_->async_accept(
      *connection_socket, [&](const boost::system::error_code& error) {
        if (error) {
          connection_socket.reset();
          LOG_ERROR() << "Accept error. Name: " << Name()
                      << ", Error: " << error.message();
        } else {
          auto connection =
              std::make_unique<SyslogConnection>(*this, connection_socket);
          {
            std::lock_guard lock(connection_list_lock_);
            connection_list_.push_back(std::move(connection));
          }
          DoAccept();
        }
      });
}

void TcpSyslogServer::DoCleanupConnections() {
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
          LOG_INFO() << "Deleted a connection";
          itr = connection_list_.erase(itr);
        } else {
          ++itr;
        }
      }
      DoCleanupConnections();
    }
  });
}

size_t TcpSyslogServer::NofConnections() const {
  std::lock_guard lock(connection_list_lock_);
  return connection_list_.size();
}
}  // namespace util::syslog