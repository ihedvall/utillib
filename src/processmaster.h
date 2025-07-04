/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include <boost/asio.hpp>

#include "util/isupervisemaster.h"

namespace util::supervise {

class ISuperviseApplication;
class ProcessApplication;

class ProcessMaster : public ISuperviseMaster {
  friend class ProcessApplication;
public:
  ProcessMaster();
  ~ProcessMaster() override;
  void Start() override;
  void Stop() override;
  ISuperviseApplication* CreateApplication() override;
private:
  std::thread supervise_thread_;
  std::atomic<bool> stop_thread_ = false;;

  boost::asio::io_context io_context_;
  boost::asio::steady_timer poll_timer_;

  void SuperviseThread();
  void DoPollTimer();

};


} // util


