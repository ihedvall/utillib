/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include "util/ilogger.h"
#include "util/logmessage.h"
namespace util::log::detail {

class Syslog : public ILogger {
 public:
  Syslog() = default;
  Syslog(const std::string& remote_host, uint16_t port);

  void AddLogMessage(const LogMessage &message) override; ///< Handle a log message
  void Stop() override; ///< Stops the working thread.
 private:
  std::mutex locker_;
  std::queue<LogMessage> message_list_;
  std::thread worker_thread_;
  std::atomic<bool> stop_thread_ = false;
  std::condition_variable condition_;

  std::string remote_host_ = "localhost";
  uint16_t port_ = 514;

  void StartWorkerThread();
  void WorkerThread();

};

} // end namespace