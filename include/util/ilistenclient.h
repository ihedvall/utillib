/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file ilistenclient.h
 * \brief Defines a client interface.
 *
 * The file defines an interface to a listen client that connects to
 * IListen TCP server object.
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

/** \brief Class that define a TCP/IP client.
 *
 * Class that defines an TCP/IP client that connect to a IListen TCP/IP
 * server.
 */
class IListenClient {
 public:
  /** \brief Constructor
   *
   * Constructor that connects to a server with host name and port
   * @param host_name Host name
   * @param port TCP/IP port
   */
  IListenClient(const std::string& host_name, uint16_t port);

  virtual ~IListenClient() = default;  ///< Default destructor

  IListenClient() = delete;
  IListenClient(const IListenClient&) = delete;
  IListenClient& operator=(const IListenClient&) = delete;

  /** \brief Returns the host name.
   *
   * Returns the host name.
   * @return Host name
   */
  [[nodiscard]] const std::string& HostName() const { return host_name_; }

  /** \brief Return the TCP/IP port.
   *
   * Returns the TCP/IP port in use.
   * @return TCP/IIP port number.
   */
  [[nodiscard]] uint16_t Port() const { return port_; }

  /** \brief Sets a name for the client.
   *
   * Optional a name can be set to the client.
   * @param name Name of the client.
   */
  void Name(const std::string& name) { name_ = name; }

  /** \brief Name of the client.
   *
   * Return the name of the client. This property is optional.
   * @return
   */
  [[nodiscard]] std::string Name() const { return name_; }

  /** \brief Description of the client.
   *
   * Optional description of the client. Only for display purpose.
   * @param description Descriptive string.
   */
  void Description(const std::string& description) {
    description_ = description;
  }

  /** \brief Returns the cleint description.
   *
   * Optional descriptive text of the client.
   * @return Descriptive text.
   */
  [[nodiscard]] const std::string& Description() const { return description_; }

  /** \brief Active or deactivates the client.
   *
   * Activates or deactivates the client interface.
   * @param active True or false
   */
  void Active(bool active) { active_ = active; }

  /** \brief Returns if the interface is activated or deactivated.
   *
   * Returns if the client interface is active i.e. receives listen messages
   * from the server.
   * @return True if activated interface.
   */
  [[nodiscard]] bool Active() const { return active_; }

  /** \brief Returns true if the client is connected to a server.
   *
   * Returns true if the client is connected to a server.
   * @return True if connected.
   */
  [[nodiscard]] bool IsConnected() const { return connected_; }

  /** \brief Returns the next listen message.
   *
   * Retuns the next ListenMessage
   * @param message Reference to ListenMessage object.
   * @return True if a listen message ws returned.
   */
  [[nodiscard]] virtual bool GetMsg(
      std::unique_ptr<detail::ListenMessage>& message) = 0;

  /** \brief Send log level to the server.
   *
   * Sets the log level in the server.
   * @param level The log level is dependent on the server functionality
   */
  virtual void SendLogLevel(uint64_t level) = 0;

 protected:
  std::atomic<bool> active_ =
      true;  ///< Indicate if the message should be used or not
  std::atomic<bool> connected_ = false;  ///< True if connected

 private:
  std::string host_name_ = "127.0.0.1";  ///< By default set to local host.
  uint16_t port_ = 0;        ///< IP Port. Use port range 49152-65535.
  std::string name_;         ///< Display name. Only used internally.
  std::string description_;  ///< Description. Only used internally.
};

}  // namespace util::log
