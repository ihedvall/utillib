/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "listenproxy.h"
#include "util/ilisten.h"
#include "util/ilogger.h"

namespace util::log::detail {

class ListenLogger : public ILogger {
 public:
  ListenLogger();
  void AddLogMessage(const LogMessage &message) override;

 private:
  ListenProxy listen_proxy_;
};

}  // namespace util::log::detail
