/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file serialportinfo.h
 * \brief The serial port information is used to list COM and USB ports.
 *
 */
#pragma once
#include <map>
#include <string>
namespace util::serial {
class SerialPortInfo;
/** \brief Sorted list of serial ports. Sorted by the name. */
using SerialPortList = std::map<std::string, SerialPortInfo>;

/** \brief The serial port information class is an interface against a serial
 * port and CAN devices.
 *
 * The serial port information is used to fix the missing function in boost
 * ASIO library that list all available serial ports on the computer. The ASIO
 * only uses the name part or location part of the object. The remaining
 * properties are not used.
 */
class SerialPortInfo {
 public:
  /** \brief Returns the computer friendly name of the serial port. */
  [[nodiscard]] const std::string& Name() const { return name_; }
  /** \brief Returns the user friendly name of the serial port. **/
  [[nodiscard]] const std::string& DisplayName() const { return display_name_; }
  /** \brief Returns the device location string. */
  [[nodiscard]] const std::string& Location() const { return location_; }
  /** \brief Returns the description of the serial port. */
  [[nodiscard]] const std::string& Description() const { return description_; }
  /** \brief Returns the manufacturer of the serial device. */
  [[nodiscard]] const std::string& Manufacturer() const {
    return manufacturer_;
  }
  /** \brief Type of device ex. USB. */
  [[nodiscard]] const std::string& Type() const { return type_; }

  /** \brief Device application type */
  [[nodiscard]] const std::string& ApplicationType() const {
    return application_type_;
  }
  /** \brief Returns a sorted list of serial ports (not USB). */
  [[nodiscard]] static SerialPortList AvailableSerialPorts();

  /** \brief Returns a sorted list of USB ports. */
  [[nodiscard]] static SerialPortList AvailableUsbDevices();

 protected:
  std::string name_;              ///< Name of port (ex. COM3).
  std::string display_name_;      ///< Friendly name.
  std::string location_;          ///< Device location/name.
  std::string description_;       ///< Descriptive text.
  std::string manufacturer_;      ///< Manufacturer.
  std::string type_;              ///< Type of device.
  std::string address_;           ///< Device address.
  std::string application_type_;  ///< Device application type.
};

}  // namespace util::serial
