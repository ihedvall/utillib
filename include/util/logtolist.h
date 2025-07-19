/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>

#include "util/ilogger.h"
#include "util/logmessage.h"

namespace util::log {

class LogToList : public ILogger {
 public:
  LogToList() = delete;
  explicit LogToList(std::string name);

  [[nodiscard]] const std::string& Name() const { return name_; }

  void MaxSize(size_t max_size);
  [[nodiscard]] size_t MaxSize() const;

  void AddLogMessage(const LogMessage& message) override;
  [[nodiscard]] bool HasLogFile() const override;

  [[nodiscard]] size_t Size() const;
  [[nodiscard]] size_t ChangeNumber() const;
  [[nodiscard]] LogMessage GetLogMessage(size_t index) const;

 private:
  std::string name_;
  size_t max_size_ = 10'000;

  mutable std::mutex list_mutex_;
  std::deque<LogMessage> log_list_;
  size_t change_number_ = 0;
};

}  // namespace util::log
