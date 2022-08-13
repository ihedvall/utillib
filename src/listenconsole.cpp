/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include "listenconsole.h"

namespace util::log::detail {

ListenConsole::ListenConsole(const std::string &share_name) {
  Name("ListenConsole");
  share_name_ = share_name;
  if (share_name_.empty()) {
    share_mem_queue_.reset();
  } else {
    share_mem_queue_ = std::make_unique<MessageQueue>(true, share_name_);
    share_mem_queue_->SetActive(false);
  }
}

ListenConsole::~ListenConsole() {
  ListenConsole::Stop();
}

bool ListenConsole::Start() {
  if (share_mem_queue_) {
    share_mem_queue_->SetActive(true);
  }
  stop_thread_ = false;
  worker_thread_ = std::thread(&ListenConsole::WorkerTask, this);
  return true;
}

bool ListenConsole::Stop() {
  stop_thread_ = true;
  if (share_mem_queue_) {
    share_mem_queue_->Stop(); // This returns the blocking get in worker thread.
  }
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
  return IListen::Stop();
}

void ListenConsole::WorkerTask() {
  while (!stop_thread_) {
    SharedListenMessage msg;
    const auto get = share_mem_queue_->Get(msg, true);
    if (get) {
      AddMessage(msg.ns1970, msg.pre_text,msg.text);
    }
  }
}

bool ListenConsole::IsActive() const {
  return share_mem_queue_ ? share_mem_queue_->IsActive() : false;
}

size_t ListenConsole::LogLevel() {
  return share_mem_queue_ ? share_mem_queue_->LogLevel() : 0;
}

void ListenConsole::AddMessage(uint64_t nano_sec_1970, const std::string &pre_text, const std::string &text) {
  const auto time = time::NsToLocalTime(nano_sec_1970,1);
  std::cout << time << " " << pre_text << " " << text << "" << std::endl;
}

void ListenConsole::SetActive(bool active) {
  if (share_mem_queue_) {
    share_mem_queue_->SetActive(active);
  }
}

void ListenConsole::SetLogLevel(size_t log_level) {
  if (share_mem_queue_) {
    share_mem_queue_->SetLogLevel(static_cast<uint8_t>(log_level));
  }
}


} // end namespace