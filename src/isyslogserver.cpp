/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/isyslogserver.h"

namespace util::syslog {

void ISyslogServer::Address(const std::string &address) { address_ = address; }

void ISyslogServer::Start() {
  msg_queue_ = std::make_unique<log::ThreadSafeQueue<SyslogMessage>>();
}

void ISyslogServer::Stop() { msg_queue_.reset(); }

void ISyslogServer::AddMsg(const SyslogMessage &msg) {
  if (msg_queue_) {
    auto temp = std::make_unique<SyslogMessage>(msg);
    msg_queue_->Put(temp);
  }
}

std::optional<SyslogMessage> ISyslogServer::GetMsg(bool block) {
  std::unique_ptr<SyslogMessage> msg;
  const auto get = msg_queue_ ? msg_queue_->Get(msg, block) : false;
  return get && msg ? *msg : std::optional<SyslogMessage>();
}

size_t ISyslogServer::NofConnections() const { return 0; }

}  // namespace util::syslog