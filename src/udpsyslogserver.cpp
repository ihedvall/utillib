/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <sstream>
#include <chrono>
#include <boost/array.hpp>

#include "util/logstream.h"
#include "udpsyslogserver.h"

using namespace boost::asio::ip;
using namespace std::chrono_literals;
using namespace util::log;

namespace util::syslog {

void UdpSyslogServer::Start() {
  stop_thread_ = false;
  if (Name().empty()) {
    std::ostringstream temp;
    temp << "UDP Syslog Server:" << Port();
    Name(temp.str());
  }
  ISyslogServer::Start();
  try {
    socket_ = std::make_unique<udp::socket>(io_context_, udp::endpoint(udp::v4(), Port()));
    server_thread_ = std::thread(&UdpSyslogServer::ServerThread, this);
    operable_ = true;
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to start receiver thread. Name: " << Name() << ", Error: " << err.what();
    operable_ = false;
  }
}

void UdpSyslogServer::Stop() {
  stop_thread_ = true;
  ISyslogServer::Stop();
  if (socket_) {
    try {
      socket_->shutdown(boost::asio::socket_base::shutdown_both);
      socket_->cancel();
      socket_->close();
    } catch (const std::exception& err) {
      LOG_ERROR() << "Failed to stop. Name: " << Name() << ", Error: " << err.what();
      operable_ = false;
    }
  }
  if (server_thread_.joinable()) {
    server_thread_.join();
  }
  socket_.reset();
}

void UdpSyslogServer::ServerThread() {

  while (!stop_thread_) {
    try {
      std::string data(65000,'\0');
      udp::endpoint remote_endpoint;
      socket_->receive_from(boost::asio::buffer(data), remote_endpoint);
      SyslogMessage msg;
      const auto parse = msg.ParseMessage(data);
      if (parse) {
        AddMsg(msg);
        if (!operable_) {
          operable_ = true;
          LOG_INFO() << "The server seems to be operable again. Name: " << Name();
        }

      }
    } catch (const std::exception& err) {
      if (operable_ && !stop_thread_) {
        operable_ = false;
        LOG_ERROR() << "Failed to receive syslog data. Name: " << Name() << ", Error: " << err.what();
      }
    }
    if (!operable_ && !stop_thread_) {
      std::this_thread::sleep_for(100ms);
    }
  }
}

} // end namespace