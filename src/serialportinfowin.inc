/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <initguid.h>

#include <devguid.h>

#include <ntddmodm.h>

#include <setupapi.h>

#include <winioctl.h>

#include <util/stringutil.h>
#include <array>
#include <boost/locale.hpp>

#include <ranges>
#include <vector>

namespace {

struct GuidFlag {
  GUID guid = {0};
  DWORD flags = 0;
};

std::array<GuidFlag, 4> serial_guid_list = {
    GuidFlag({GUID_DEVCLASS_PORTS}, DIGCF_PRESENT),
    GuidFlag({GUID_DEVCLASS_MODEM}, DIGCF_PRESENT),
    GuidFlag({GUID_DEVINTERFACE_COMPORT},
             DIGCF_PRESENT | DIGCF_DEVICEINTERFACE),
    GuidFlag({GUID_DEVINTERFACE_MODEM}, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE),
};

std::array<GuidFlag, 1> usb_guid_list = {
    GuidFlag({GUID_DEVCLASS_USB}, DIGCF_ALLCLASSES),
};

std::string GetPortName(HDEVINFO device_info, SP_DEVINFO_DATA& device_data) {
  std::string name;
  const auto key = ::SetupDiOpenDevRegKey(
      device_info, &device_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
  if (key == INVALID_HANDLE_VALUE) {
    return {};
  }

  constexpr std::array<std::string_view, 3> lsSubKey = {
      "PortName", "PortNumber", "SymbolicName"};

  for (const auto& sub_key : lsSubKey) {
    DWORD data_size = 0;
    DWORD data_type = REG_SZ;
    auto ret = ::RegQueryValueExA(key, sub_key.data(), nullptr, &data_type,
                                  nullptr, &data_size);
    if (ret != ERROR_SUCCESS || data_size <= 0 || data_type != REG_SZ) {
      continue;
    }
    std::string temp_name(data_size, 0);
    ret = ::RegQueryValueExA(key, sub_key.data(), nullptr, nullptr,
                             reinterpret_cast<unsigned char*>(temp_name.data()),
                             &data_size);
    if (ret != ERROR_SUCCESS) {
      continue;
    }
    while (temp_name.back() == '\0') {
      temp_name.pop_back();
    }
    name = temp_name;
    if (!name.empty()) {
      break;
    }
  }
  ::RegCloseKey(key);
  return name;
}

std::string GetRegistryProperty(HDEVINFO device_info,
                                SP_DEVINFO_DATA device_data, DWORD property) {
  DWORD data_type = 0;
  DWORD data_size = 0;
  ::SetupDiGetDeviceRegistryPropertyW(device_info, &device_data, property,
                                      &data_type, nullptr, 0, &data_size);
  if (data_size == 0) {
    return {};
  }
  std::string value;
  switch (data_type) {
    case REG_SZ: {
      std::wstring temp(data_size, 0);
      auto ret = ::SetupDiGetDeviceRegistryPropertyW(
          device_info, &device_data, property, nullptr,
          reinterpret_cast<PBYTE>(temp.data()), data_size, &data_size);
      if (ret != TRUE || data_size == 0) {
        return {};
      }
      while (temp.back() == '\0') {
        temp.pop_back();
      }
      value = boost::locale::conv::utf_to_utf<char>(temp);
      break;
    }

    case REG_DWORD: {
      DWORD temp = 0;
      auto ret = ::SetupDiGetDeviceRegistryPropertyW(
          device_info, &device_data, property, nullptr,
          reinterpret_cast<PBYTE>(&temp), data_size, &data_size);
      if (ret != TRUE || data_size == 0) {
        return {};
      }
      value = std::to_string(temp);
      break;
    }

    default:
      break;
  }

  return value;
}

std::vector<std::string> GetHardwarePorts() {
  std::vector<std::string> lsName;
  HKEY hKey = nullptr;
  auto ret =
      RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0,
                    KEY_QUERY_VALUE, &hKey);
  if (ret != ERROR_SUCCESS) {
    return lsName;
  }
  DWORD index = 0;
  for (ret = ERROR_SUCCESS; ret == ERROR_SUCCESS; ++index) {
    std::string value_name(260, 0);
    DWORD value_name_size = value_name.size();
    DWORD data_type = REG_SZ;
    DWORD value_size = 0;
    ret = ::RegEnumValueA(hKey, index, value_name.data(), &value_name_size,
                          nullptr, &data_type, nullptr, &value_size);
    if (ret != ERROR_SUCCESS || value_size <= 0 || data_type != REG_SZ) {
      continue;
    }
    value_name_size = value_name.size();
    std::string value(value_size, 0);
    ret = ::RegEnumValueA(
        hKey, index, value_name.data(), &value_name_size, nullptr, nullptr,
        reinterpret_cast<unsigned char*>(value.data()), &value_size);
    if (ret == ERROR_SUCCESS) {
      lsName.emplace_back(value.c_str());
    }
  }
  ::RegCloseKey(hKey);
  return lsName;
}

}  // End namespace

namespace util::serial {

SerialPortList SerialPortInfo::AvailableSerialPorts() {
  SerialPortList port_list;

  for (const auto& item : serial_guid_list) {
    const auto device_info =
        ::SetupDiGetClassDevsW(&item.guid, nullptr, nullptr, item.flags);
    if (device_info == INVALID_HANDLE_VALUE) {
      continue;
    }

    SP_DEVINFO_DATA device_data = {};
    device_data.cbSize = sizeof(device_data);

    DWORD index = 0;
    while (::SetupDiEnumDeviceInfo(device_info, index++, &device_data)) {
      SerialPortInfo di_info;
      di_info.name_ = GetPortName(device_info, device_data);
      if (di_info.name_.empty()) {
        continue;
      }
      const auto exist = port_list.contains(di_info.name_);
      if (exist) {
        continue;
      }
      di_info.location_ = GetRegistryProperty(device_info, device_data,
                                              SPDRP_LOCATION_INFORMATION);
      if (di_info.location_.empty()) {
        std::ostringstream location;
        if (string::IEquals(di_info.name_, "COM", 3)) {
          location << R"(\\.\)" << di_info.name_;
        } else {
          location << di_info.name_;
        }
        di_info.location_ = location.str();
      }

      di_info.type_ =
          GetRegistryProperty(device_info, device_data, SPDRP_ENUMERATOR_NAME);
      di_info.description_ =
          GetRegistryProperty(device_info, device_data, SPDRP_DEVICEDESC);
      di_info.manufacturer_ =
          GetRegistryProperty(device_info, device_data, SPDRP_MFG);
      di_info.display_name_ =
          GetRegistryProperty(device_info, device_data, SPDRP_FRIENDLYNAME);
      di_info.application_type_ =
          GetRegistryProperty(device_info, device_data, SPDRP_CLASS);

      if (di_info.display_name_.empty()) {
        di_info.display_name_ = di_info.description_;
      }

      port_list.emplace(di_info.name_, di_info);
    }
    ::SetupDiDestroyDeviceInfoList(device_info);
  }
  const auto hw_list = GetHardwarePorts();
  for (const auto& hw_name : hw_list) {
    SerialPortInfo hw_info;
    hw_info.name_ = hw_name;
    std::ostringstream location;
    if (string::IEquals(hw_name, "COM", 3)) {
      location << R"(\\.\)" << hw_name;
    } else {
      location << hw_name;
    }
    hw_info.location_ = location.str();
    hw_info.display_name_ = hw_info.name_;

    const auto exist = port_list.contains(hw_info.name_);
    if (!exist) {
      port_list.emplace(hw_info.name_, hw_info);
    }
  }
  return port_list;
}

SerialPortList SerialPortInfo::AvailableUsbDevices() {
  SerialPortList port_list;

  for (const auto& item : usb_guid_list) {
    const auto device_info =
        ::SetupDiGetClassDevsW(&item.guid, nullptr, nullptr, item.flags);
    if (device_info == INVALID_HANDLE_VALUE) {
      continue;
    }

    SP_DEVINFO_DATA device_data = {};
    device_data.cbSize = sizeof(device_data);

    DWORD index = 0;
    while (::SetupDiEnumDeviceInfo(device_info, index++, &device_data)) {
      SerialPortInfo di_info;
      di_info.name_ = GetPortName(device_info, device_data);
      if (di_info.name_.empty()) {
        continue;
      }
      const auto exist = port_list.contains(di_info.name_);
      if (exist) {
        continue;
      }
      di_info.location_ = GetRegistryProperty(device_info, device_data,
                                              SPDRP_LOCATION_INFORMATION);
      if (di_info.location_.empty()) {
        std::ostringstream location;
        if (string::IEquals(di_info.name_, "COM", 3)) {
          location << R"(\\.\)" << di_info.name_;
        } else {
          location << di_info.name_;
        }
        di_info.location_ = location.str();
      }

      di_info.type_ =
          GetRegistryProperty(device_info, device_data, SPDRP_ENUMERATOR_NAME);
      di_info.description_ =
          GetRegistryProperty(device_info, device_data, SPDRP_DEVICEDESC);
      di_info.manufacturer_ =
          GetRegistryProperty(device_info, device_data, SPDRP_MFG);
      di_info.display_name_ =
          GetRegistryProperty(device_info, device_data, SPDRP_FRIENDLYNAME);
      di_info.application_type_ =
          GetRegistryProperty(device_info, device_data, SPDRP_CLASS);

      if (di_info.display_name_.empty()) {
        di_info.display_name_ = di_info.description_;
      }
      // Only USB devices should be included
      if (!util::string::IEquals(di_info.Type(), "USB")) {
        continue;
      }
      port_list.emplace(di_info.name_, di_info);
    }
    ::SetupDiDestroyDeviceInfoList(device_info);
  }
  return port_list;
}
}  // namespace util::serial