/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include "util/ilisten.h"
#include "messagequeue.h"

namespace util::log::detail {

class ListenConsole : public IListen {
 public:
  explicit ListenConsole(const std::string& share_name);
  ~ListenConsole() override;

  ListenConsole() = delete;

  bool Start() override;
  bool Stop() override;
  void SetActive(bool active) override;
  [[nodiscard]] bool IsActive() const override;

  void SetLogLevel(size_t log_level) override;
  size_t LogLevel() override;
 protected:
  void AddMessage(uint64_t nano_sec_1970, const std::string &pre_text, const std::string &text) override;

 private:
  std::unique_ptr<MessageQueue> share_mem_queue_;
  std::atomic<bool> stop_thread_ = false;

  std::thread worker_thread_;
  void WorkerTask();
};

}