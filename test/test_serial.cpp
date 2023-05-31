/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>

#include "util/serialportinfo.h"

namespace util::test {

TEST(SerialPortInfo, ListSerialPorts) {
  const auto port_list = serial::SerialPortInfo::AvailableSerialPorts();
  for (const auto& port : port_list) {
    std::cout << "Name: " << port.second.Name() << std::endl;
    std::cout << "Display: " << port.second.DisplayName() << std::endl;
    std::cout << "Desc: " << port.second.Description() << std::endl;
    std::cout << "Loc: " << port.second.Location() << std::endl;
    std::cout << "Man: " << port.second.Manufacturer() << std::endl;
    std::cout << "Type: " << port.second.Type() << std::endl;
    std::cout << "AppType: " << port.second.ApplicationType() << std::endl;
    std::cout << std::endl;
  }
}

TEST(SerialPortInfo, ListUsbDevices) {
  const auto usb_list = serial::SerialPortInfo::AvailableUsbDevices();
  for (const auto& port : usb_list) {
    std::cout << "Name: " << port.second.Name() << std::endl;
    std::cout << "Display: " << port.second.DisplayName() << std::endl;
    std::cout << "Desc: " << port.second.Description() << std::endl;
    std::cout << "Loc: " << port.second.Location() << std::endl;
    std::cout << "Man: " << port.second.Manufacturer() << std::endl;
    std::cout << "Type: " << port.second.Type() << std::endl;
    std::cout << "AppType: " << port.second.ApplicationType() << std::endl;
    std::cout << std::endl;
  }
}
}  // namespace util::test