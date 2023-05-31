/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "util/serialportinfo.h"
#ifdef _WIN32

#include <windows.h>
#include "serialportinfowin.inc"
#else
namespace util::serial {
SerialPortList SerialPortInfo::AvailableSerialPorts() {
  SerialPortList port_list;
  return port_list;
}

SerialPortList SerialPortInfo::AvailableUsbDevices() {
  SerialPortList port_list;
  return port_list;
}
}  // namespace util::serial
#endif

namespace util::serial {}  // namespace util::serial