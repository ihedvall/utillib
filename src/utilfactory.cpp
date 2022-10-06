/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/utilfactory.h"

#include "listenconsole.h"
#include "listenlogger.h"
#include "listenproxy.h"
#include "listenserver.h"
#include "logconsole.h"
#include "logfile.h"
#include "syslog.h"
#include "udpsyslogserver.h"
#include "util/stringutil.h"
using namespace util::syslog;
using namespace util::log;
using namespace util::string;

namespace util {

std::unique_ptr<ISyslogServer> UtilFactory::CreateSyslogServer(
    SyslogServerType type) {
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

std::unique_ptr<log::ILogger> UtilFactory::CreateLogger(
    log::LogType type, std::vector<std::string> &arg_list) {
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
      const auto port =
          std::stoul(arg_list.size() < 2 ? std::string("514") : arg_list[1]);
      logger = std::make_unique<detail::Syslog>(remote_host,
                                                static_cast<uint16_t>(port));
      break;
    }

    default:
      break;
  }
  return logger;
}

std::unique_ptr<IListen> UtilFactory::CreateListen(
    const std::string &type, const std::string &share_name) {
  std::unique_ptr<IListen> listen;
  if (IEquals(type, "ListenProxy") && !share_name.empty()) {
    listen = std::make_unique<detail::ListenProxy>(share_name);
  } else if (IEquals(type, "ListenServer")) {
    listen = std::make_unique<detail::ListenServer>(share_name);
  } else if (IEquals(type, "ListenConsole")) {
    listen = std::make_unique<detail::ListenConsole>(share_name);
  }
  return listen;
}
}  // namespace util