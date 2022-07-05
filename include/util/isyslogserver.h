/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file isyslogserver.h
 * \brief Defines an interface against a syslog server.
 *
 * Defines an interface against syslog servers. A syslog applications
 * may define many syslog inputs with different protocols and ports. Each
 * of this inputs are individual syslog servers.
 */

#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <atomic>
#include "util/threadsafequeue.h"
#include "util/syslogmessage.h"

namespace util::syslog {

/** \enum SyslogServerType
 * \brief Defines the type of syslog server.
 *
 * Enumerate that defines the protocol the syslog server uses for receiving messages.
 */
enum class SyslogServerType : uint8_t {
  UdpServer = 0, ///< Uses UDP protocol (RFC5426).
  TlsServer = 1, ///< Uses TLS on top o TCP (RFC5425).
  TcpServer = 2  ///< Uses plain TCP protocol (RFC6587).
};

/** \class ISyslogServer isyslogserver.h "util/gnuplot.h"
 * \brief Define a syslog server interface.
 *
 * In a syslog application consist of one or more syslog inputs. Each
 * of these inputs is a syslog server object.
 */
class ISyslogServer {
 public:

  /** \brief Bind address of the server.
   *
   * By default, the server binds to any incoming calls. By changing this value to
   * 127.0.0.1. only local connections (messages) are allowed. This is recommended
   * for UDP connections.
   * @param address
   */
  void Address(const std::string& address) {
    address_ = address;
  }

  [[nodiscard]] const std::string& Address() const { ///< Returns the bind address
    return address_;
  }

  void Name(const std::string& name) { ///< Sets the display name of the server.
    name_ = name;
  }

  const std::string& Name() const { ///< Returns the display name of the server
    return name_;
  }

  void Port(uint16_t port) { ///< Sets the server port
    port_ = port;
  }

  [[nodiscard]] uint16_t Port() const { ///< Returns the server port
    return port_;
  }

  void AddMsg(const SyslogMessage& msg); ///< Adds a syslog message to the internal message queue.

  /** \brief Returns the next message in the queue.
   *
   * The function returns the next message in the internal message queue. The function may block the
   * call until a message exist in the queue.
   *
   * @param block Set tor true if the call should block until a message exist.
   * @return Returns a std::optional syslog message.
   */
  std::optional<SyslogMessage> GetMsg(bool block);

  virtual void Start(); ///< Starts the worker thread in the server.
  virtual void Stop(); ///< Stops the worker thread in the server.

  [[nodiscard]] size_t NofMessages() const { ///< Returns the message queue size
    return msg_queue_ ? msg_queue_->Size() : 0;
  }

  /** \brief Returns true if the receiver works as normal.
   *
   * The operable flag is false if the receiving of messages fails of some reason.
   * The receiver may fail if invalid messages is received.
   * @return Returns false if an error occurs in the server.
   */
  [[nodiscard]] bool IsOperable() const {
    return operable_;
  }
 protected:
  std::atomic<bool> operable_ = true; ///< Operable flag.
 private:
  std::string address_; ///< Bind address. Default is empty i.e. 0.0.0.0
  std::string name_;    ///< Display name of the server.
  uint16_t port_ = 0;   ///< Server port.
  std::unique_ptr<log::ThreadSafeQueue<SyslogMessage>> msg_queue_; ///< Message queue

};

} // end namespace