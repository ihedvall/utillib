/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <util/consoleapp.h>
#include <util/ilisten.h>
#include <util/listenconfig.h>
#include <memory>

namespace util {

class ListenApp : public ConsoleApp {
 public:
 protected:
  bool OnInit() override;
  void OnExit() override;

 private:
  std::vector<std::unique_ptr<log::IListen>> kServerList;
  uint16_t kFreePort = 42511;

  std::unique_ptr<log::ListenConfig> master;

  void AddAllKnownServers();
};

}  // namespace util
