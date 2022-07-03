/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "util/ilogger.h"
#include "util/ilisten.h"
#include "listenproxy.h"

namespace util::log::detail {

class ListenLogger : public ILogger {
 public:
  ListenLogger();
  void AddLogMessage(const LogMessage &message) override;
 private:
  ListenProxy listen_proxy_;
};

}



