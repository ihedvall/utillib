/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/logtolist.h"

namespace util::log {

LogToList::LogToList(std::string name) : name_(std::move(name)) {}

void LogToList::AddLogMessage(const LogMessage& message) {
  bool shrink = false;
  std::scoped_lock lock(list_mutex_);
  if (log_list_.size() >= max_size_) {
    log_list_.pop_back();
    shrink = true;
  }
  log_list_.push_front(message);
  if (shrink) {
    // Reduce the amount of memory it uses
    log_list_.shrink_to_fit();
  }
  ++change_number_;
}

bool LogToList::HasLogFile() const { return false; }

size_t LogToList::Size() const {
  std::scoped_lock lock(list_mutex_);
  return log_list_.size();
}

uint64_t LogToList::ChangeNumber() const {
  std::scoped_lock lock(list_mutex_);
  return change_number_;
}

LogMessage LogToList::GetLogMessage(size_t index) const {
  LogMessage msg;
  {
    std::scoped_lock lock(list_mutex_);
    if (index < log_list_.size()) {
      msg = log_list_[index];
    }
  }
  return msg;
}

void LogToList::MaxSize(size_t max_size) {
  std::scoped_lock lock(list_mutex_);
  max_size_ = max_size;
}

size_t LogToList::MaxSize() const {
  std::scoped_lock lock(list_mutex_);
  return max_size_;
}

}  // namespace util::log