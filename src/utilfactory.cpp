/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/utilfactory.h"

#include "listenclient.h"
#include "listenconsole.h"
#include "listenlogger.h"
#include "listenproxy.h"
#include "listenserver.h"
#include "logconsole.h"
#include "logfile.h"
#include "syslog.h"
#include "syslogpublisher.h"
#include "syslogsubscriber.h"
#include "tcpsyslogserver.h"
#include "udpsyslogserver.h"
#include "processmaster.h"
#include "util/stringutil.h"
#include "util/logstream.h"

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
    case SyslogServerType::TcpServer: {
      auto tcp_server = std::make_unique<TcpSyslogServer>();
      server = std::move(tcp_server);
      break;
    }
    case SyslogServerType::TcpPublisher: {
      auto tcp_publisher = std::make_unique<SyslogPublisher>();
      server = std::move(tcp_publisher);
      break;
    }
    case SyslogServerType::TcpSubscriber: {
      auto tcp_subscriber = std::make_unique<SyslogSubscriber>();
      server = std::move(tcp_subscriber);
      break;
    }
    case SyslogServerType::TlsServer:
    default:
      break;
  }
  return server;
}

std::unique_ptr<log::ILogger> UtilFactory::CreateLogger(
    log::LogType type, const std::vector<std::string> &arg_list) {
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

std::unique_ptr<IListen> UtilFactory::CreateListen( TypeOfListen type,
    const std::string &share_name) {
  std::unique_ptr<IListen> listen;
  switch (type) {
    case TypeOfListen::ListenProxyType:
      if (!share_name.empty()) {
        listen = std::make_unique<detail::ListenProxy>(share_name);
      } else {
        LOG_ERROR() << "ListenProxy requires a share memory name.";
      }
      break;

    case TypeOfListen::ListenServerType:
      listen = std::make_unique<detail::ListenServer>(share_name);
      break;

    case TypeOfListen::ListenConsoleType:
      listen = std::make_unique<detail::ListenConsole>(share_name);
      break;

    default:
      LOG_ERROR() << "Unknown listen type: " << static_cast<int>(type);
      break;
  }
  return listen;
}

std::unique_ptr<log::IListenClient> UtilFactory::CreateListenClient(
    const std::string &host, uint16_t port) {
  auto temp = std::make_unique<log::detail::ListenClient>(host, port);
  return temp;
}

std::unique_ptr<supervise::ISuperviseMaster> UtilFactory::CreateSuperviseMaster(
    supervise::TypeOfSuperviseMaster type) {
  std::unique_ptr<supervise::ISuperviseMaster> master;
  switch (type) {
    case supervise::TypeOfSuperviseMaster::SuperviseMasterType: {
      master = std::make_unique<supervise::ISuperviseMaster>();
      break;
    }

    case supervise::TypeOfSuperviseMaster::ProcessMasterType: {
      master = std::make_unique<supervise::ProcessMaster>();
      break;
    }
    default: {
      LOG_ERROR() << "Unknown supervise master. Type: "
        << static_cast<int>(type);
      break;
    }
  }
  return master;
}

}  // namespace util