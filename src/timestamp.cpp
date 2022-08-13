/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <boost/endian/buffers.hpp>
#include "util/timestamp.h"

namespace util::time {

std::string GetLocalDateTime(std::chrono::time_point<std::chrono::system_clock> timestamp) {
  const auto utc = std::chrono::system_clock::to_time_t(timestamp);
  struct tm bt{};
  localtime_s(&bt, &utc);
  std::ostringstream date_time;
  date_time << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
  return date_time.str();
}

std::string GetLocalTimestampWithMs(std::chrono::time_point<std::chrono::system_clock> timestamp) {
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()) % 1000;
  const auto timer = std::chrono::system_clock::to_time_t(timestamp);
  struct tm bt{};
  localtime_s(&bt, &timer);

  std::ostringstream text;
  text << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
  return text.str();
}

std::string GetLocalTimestampWithUs(std::chrono::time_point<std::chrono::system_clock> timestamp) {
  const auto us = std::chrono::duration_cast<std::chrono::microseconds>(timestamp.time_since_epoch()) % 1000'000;
  const auto timer = std::chrono::system_clock::to_time_t(timestamp);
  struct tm bt{};
  localtime_s(&bt, &timer);

  std::ostringstream text;
  text << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(6) << us.count();
  return text.str();
}

uint64_t TimeStampToNs(TimeStamp timestamp) {
  // 1.na since midnight
  // 2 sec since 1970
  // 3 Add them
  const auto ns_midnight = std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch()) % 1000'000'000;
  const auto sec_1970 = std::chrono::system_clock::to_time_t(timestamp);
  uint64_t ns = sec_1970;
  ns *= 1'000'000'000;
  ns += ns_midnight.count();
  return ns;
}

std::string NsToLocalIsoTime(uint64_t ns_since_1970) {
  const auto ms_sec = (ns_since_1970 / 1'000'000) % 1'000;
  const auto system_time = static_cast<std::time_t>(ns_since_1970 / 1'000'000'000);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream text;
  text << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
  if (ms_sec > 0) {
    text << '.' << std::setfill('0') << std::setw(3) << ms_sec;
  }
  return text.str();
}

std::string NsToIsoTime(uint64_t ns_since_1970, int format) {
  const auto system_time = static_cast<std::time_t>(ns_since_1970 / 1'000'000'000);
  struct tm bt{};
  gmtime_s(&bt, &system_time);

  std::ostringstream text;
  text << std::put_time(&bt, "%Y-%m-%dT%H:%M:%S");
  switch (format) {
    case 3: {
      const auto ns_sec = ns_since_1970 % 1'000'000'000;
      text << '.' << std::setfill('0') << std::setw(9) << ns_sec;
      break;
    }

    case 2: {
      const auto us_sec = (ns_since_1970 / 1'000) % 1'000'000;
      text << '.' << std::setfill('0') << std::setw(6) << us_sec;
      break;
    }

    case 1: {
      const auto ms_sec = (ns_since_1970 / 1'000'000)  % 1'000;
      text << '.' << std::setfill('0') << std::setw(3) << ms_sec;
      break;
    }

    case 0:
    default:
      break;
  }

  text << "Z";
  return text.str();
}

uint64_t IsoTimeToNs(const std::string& iso_time, bool local_time) {
  if (iso_time.empty()) {
    return 0;
  }
  const auto ods_time = strchr(iso_time.c_str(), '-') == nullptr;
  if (ods_time) {
    return OdsDateToNs(iso_time);
  }

  std::vector<uint64_t> temp_list;
  uint8_t ns_count = 0;
  uint64_t temp = 0;
  for (char input : iso_time) {
    if (std::isdigit(input)) {
      temp *= 10;
      temp += input - '0';
      if (temp_list.size() == 6) {
        ++ns_count;
      }
    } else  {
      temp_list.push_back(temp);
      temp = 0;
    }
  }
  if (temp > 0) {
    temp_list.push_back(temp);
  }
  uint64_t nano_sec = 0;
  struct tm bt{};
  bt.tm_year = 70;
  bt.tm_mon = 0;
  bt.tm_mday = 1;
  bt.tm_hour = 0;
  bt.tm_min = 0;
  bt.tm_sec = 0;

  for (size_t index = 0; index < temp_list.size(); ++index) {
    int value = static_cast<int>(temp_list[index]);
    switch (index) {
      case 0:
        if (value < 1970) {
          value = 1970;
        }
        bt.tm_year = value - 1900; // Years since 1900
        break;

      case 1:
        bt.tm_mon = value - 1; // Month 0..11
        break;

      case 2:
        bt.tm_mday = value; // Day 1..31
        break;

      case 3:
        bt.tm_hour = value;
        break;

      case 4:
        bt.tm_min = value;
        break;

      case 5:
        bt.tm_sec = value;
        break;

      case 6: {
        nano_sec = temp_list[6];
        for (; ns_count < 9; ++ns_count) {
          nano_sec *= 10;
        }
        break;
      }

      default:
        break;
    }
  }
  auto ns_1970 = static_cast<uint64_t>(local_time ? mktime(&bt) : _mkgmtime(&bt));
  ns_1970 *= 1'000'000'000;
  ns_1970 += nano_sec;
  return ns_1970;
}

std::string NsToLocalDate(uint64_t ns_since_1970) {
  const auto system_time = static_cast<std::time_t>(ns_since_1970 / 1'000'000'000);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream text;
  text << std::put_time(&bt, "%x");
  return text.str();
}

std::string NsToLocalTime(uint64_t ns_since_1970, int format) {
  const auto system_time = static_cast<std::time_t>(ns_since_1970 / 1'000'000'000);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream text;
  text << std::put_time(&bt, "%X");

  std::ostringstream extra;
  switch (format) {
    case 1: { // Show ms
      const auto ms_sec = (ns_since_1970 / 1'000'000) % 1'000;
      extra << '.' << std::setfill('0') << std::setw(3) << ms_sec;
      break;
    }

    case 2: { // Show us
      const auto us_sec = (ns_since_1970 / 1'000) % 1'000'000;
      extra << '.' << std::setfill('0') << std::setw(6) << us_sec;
      break;
    }

    case 3: { // Show ns
      const auto ns_sec = (ns_since_1970) % 1'000'000'000;
      extra << '.' << std::setfill('0') << std::setw(9) << ns_sec;
      break;
    }

    case 0: // Show seconds
    default:
      return text.str();
  }

  const std::string input = text.str();
  std::ostringstream output;
  bool is_am_pm = false;
  bool is_blank = false;
  for (const char char_in : input) {
    if (std::isalpha(char_in) && !is_am_pm) { // Assume AM PM and insert ms
      output << extra.str();
      if (is_blank) {
        output << " ";
        is_blank = false;
      }
      is_am_pm = true;
      output << char_in;
    } else if (iswspace(char_in)) {
      is_blank = true;
    } else {
      if (is_blank) {
        output << " ";
        is_blank = false;
      }
      output << char_in;
    }
  }
  if (!is_am_pm) {
    output << extra.str();
  }
  return output.str();
}

uint64_t OdsDateToNs(const std::string &ods_date) {
  if (ods_date.empty()) {
    return 0;
  }
  const auto iso_time = strchr(ods_date.c_str(), '-') != nullptr;
  if (iso_time) {
    return IsoTimeToNs(ods_date);
  }

  std::vector<uint64_t> temp_list;
  uint8_t ns_count = 0; // Counter for nof digits
  uint64_t temp = 0; // Holder for the value
  for (char input : ods_date) {
    if (!std::isdigit(input)) {
      continue;
    }
    temp *= 10;
    temp += input - '0';
    ++ns_count;
    switch (temp_list.size()) {
      case 0: // YYYY
        if (ns_count >= 4) {
          temp_list.push_back(temp);
          temp = 0;
          ns_count = 0;
        }
        break;


      case 1: // MM
      case 2: // DD
      case 3: // hh
      case 4: // mm
      case 5: // ss
        if (ns_count >= 2) {
          temp_list.push_back(temp);
          temp = 0;
          ns_count = 0;
        }
        break;
      default:
        if (ns_count >= 3) {
          temp_list.push_back(temp);
          temp = 0;
          ns_count = 0;
        }
        break;
    }
  }
  uint64_t nano_sec = 0;
  struct tm bt{};
  for (size_t index = 0; index < temp_list.size(); ++index) {
    int value = static_cast<int>(temp_list[index]);
    switch (index) {
      case 0:
        if (value < 1970) {
          value = 1970;
        }
        bt.tm_year = value - 1900; // Years since 1900
        break;

      case 1:
        bt.tm_mon = value - 1; // Month 0..11
        break;

      case 2:
        bt.tm_mday = value; // Day 1..31
        break;

      case 3:
        bt.tm_hour = value;
        break;

      case 4:
        bt.tm_min = value;
        break;

      case 5:
        bt.tm_sec = value;
        break;

      case 6: { // ms
        auto val = static_cast<uint64_t>(value);
        val *= 1'000'000;
        nano_sec += val;
        break;
      }

      case 7: { // us
        auto val = static_cast<uint64_t>(value);
        val *= 1'000;
        nano_sec += val;
        break;
      }

      case 8: { // ns
        auto val = static_cast<uint64_t>(value);
        nano_sec += val;
        break;
      }

      default:
        break;
    }
  }
  auto ns_1970 = static_cast<uint64_t>(_mkgmtime(&bt));
  ns_1970 *= 1'000'000'000;
  ns_1970 += nano_sec;
  return ns_1970;

}

uint64_t FileTimeToNs(std::filesystem::file_time_type time) {
#if (_MSC_VER)
  const auto sys_time = std::chrono::clock_cast<std::chrono::system_clock>(time);
  return TimeStampToNs(sys_time);
#else
  const auto sys_time = std::chrono::file_clock::to_sys(time);
  return TimeStampToNs(sys_time);
#endif
}

}
