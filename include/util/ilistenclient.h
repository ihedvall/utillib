/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>

#include "util/threadsafequeue.h"
namespace util::log {

namespace detail {
class ListenMessage;
}

class IListenClient {
 public:
  IListenClient(const std::string& host_name, uint16_t port);
  virtual ~IListenClient() = default;

  IListenClient() = delete;
  IListenClient(const IListenClient&) = delete;
  IListenClient& operator=(const IListenClient&) = delete;

  [[nodiscard]] const std::string& HostName() const { return host_name_; }

  [[nodiscard]] uint16_t Port() const { return port_; }

  void Name(const std::string& name) { name_ = name; }
  [[nodiscard]] std::string Name() const { return name_; }

  void Description(const std::string& description) {
    description_ = description;
  }
  const std::string& Description() const { return description_; }

  void Active(bool active) { active_ = active; }
  [[nodiscard]] bool Active() const { return active_; }

  [[nodiscard]] bool IsConnected() const { return connected_; }

  [[nodiscard]] virtual bool GetMsg(
      std::unique_ptr<detail::ListenMessage>& message) = 0;

  virtual void SendLogLevel(uint64_t level) = 0;

 protected:
  std::atomic<bool> active_ =
      true;  ///< Indicate if the message should be used or not
  std::atomic<bool> connected_ = false;

 private:
  std::string host_name_ = "127.0.0.1";  ///< By default set to local host.
  uint16_t port_ = 0;        ///< IP Port. Use port range 49152-65535.
  std::string name_;         ///< Display name. Only used internally.
  std::string description_;  ///< Description. Only used internally.
};

}  // namespace util::log
