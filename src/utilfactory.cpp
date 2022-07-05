/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/utilfactory.h"
#include "udpsyslogserver.h"
#include "logconsole.h"
#include "logfile.h"
#include "listenlogger.h"
#include "syslog.h"

using namespace util::syslog;
using namespace util::log;
namespace util {

std::unique_ptr<ISyslogServer> UtilFactory::CreateSyslogServer(SyslogServerType type) {
  std::unique_ptr<ISyslogServer> server;

  switch (type) {
    case SyslogServerType::UdpServer: {
      auto udp_server = std::make_unique<UdpSyslogServer>();
      server = std::move(udp_server);
      break;
    }
    case SyslogServerType::TlsServer:
    case SyslogServerType::TcpServer:
    default:
      break;
  }
  return server;
}

std::unique_ptr<log::ILogger> UtilFactory::CreateLogger(log::LogType type, std::vector<std::string> &arg_list) {
  std::unique_ptr<ILogger> logger;

  switch (type) {
    case LogType::LogToConsole:
      logger = std::make_unique<util::log::detail::LogConsole>();
      break;

    case LogType::LogToFile: {
      const auto base_name = arg_list.empty() ? "default" : arg_list[0];
      logger = std::make_unique<util::log::detail::LogFile>(base_name);
      break;
    }

    case LogType::LogToListen:
      logger = std::make_unique<detail::ListenLogger>();
      break;

    case LogType::LogToSyslog: {
      const auto remote_host = arg_list.empty() ? "localhost" : arg_list[0];
      const auto port = std::stoul(arg_list.size() < 2 ?  std::string("514") : arg_list[1]);
      logger = std::make_unique<detail::Syslog>(remote_host, static_cast<uint16_t>(port));
      break;
    }

    default:
      break;
  }
  return logger;
}

} // util