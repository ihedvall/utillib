/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <ctime>
#include <memory>

#include <boost/process.hpp>

#include "util/isuperviseapplication.h"

namespace util::supervise {

class ProcessMaster;
class ProcessApplication : public ISuperviseApplication {
public:
  ProcessApplication() = delete;
  explicit ProcessApplication(ProcessMaster& master);
  ~ProcessApplication() override;
  void Start() override;
  void Stop() override;
  void Poll() override;
protected:
  ProcessMaster& master_;
  time_t startup_timer_ = 0;
  time_t restart_timer_ = 0;

  [[nodiscard]] virtual bool StartProcess();
  [[nodiscard]] virtual bool IsRunning() const;
  virtual void StopProcess();
private:
  std::unique_ptr<boost::process::process> process_;

  void FindApplication();

  void PollManual();
  void PollOnce();
  void PollAutomatic();

};

} // util::supervise


