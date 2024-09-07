/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <util/ilisten.h>

#include <string>

#include "messagequeue.h"
namespace util::log::detail {
class ListenProxy : public IListen {
 public:
  explicit ListenProxy(const std::string &share_name);
  ~ListenProxy() override;
  ListenProxy() = delete;
  ListenProxy(const ListenProxy &) = delete;
  ListenProxy &operator=(const ListenProxy &) = delete;

  [[nodiscard]] bool IsActive() const override;

  void SetLogLevel(size_t log_level) override;
  [[nodiscard]] size_t LogLevel() override;

  void AddMessage(uint64_t nano_sec_1970, const std::string &pre_text,
                  const std::string &text) override;

 private:
  MessageQueue queue_;

};

}  // namespace util::log::detail
