/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "util/stringutil.h"
#include <cmath>
#include <cstring>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


namespace util::string {
bool IEquals(const std::string &s1, const std::string &s2, size_t nChar) {
#if (_MSC_VER)
  return nChar == 0 ? stricmp(s1.c_str(), s2.c_str()) == 0
                    : strnicmp(s1.c_str(), s2.c_str(), nChar) == 0;
#else
  return nChar == 0 ? strcasecmp(s1.c_str(), s2.c_str()) == 0
                    : strncasecmp(s1.c_str(), s2.c_str(), nChar) == 0;
#endif
}

bool IEquals(const std::wstring &s1, const std::wstring &s2, size_t nChar) {
#if (_WIN32)
  return nChar == 0 ? wcsicmp(s1.c_str(), s2.c_str()) == 0
                    : wcsnicmp(s1.c_str(), s2.c_str(), nChar) == 0;
#else
  return nChar == 0 ? wcscasecmp(s1.c_str(), s2.c_str()) == 0
                    : wcsncasecmp(s1.c_str(), s2.c_str(), nChar) == 0;
#endif
}

bool IgnoreCase::operator()(const std::string &s1,
                            const std::string &s2) const {
  return boost::algorithm::ilexicographical_compare(s1, s2);
}

bool IgnoreCase::operator()(const std::wstring &s1,
                            const std::wstring &s2) const {
  return boost::algorithm::ilexicographical_compare(s1, s2);
}

void Trim(std::string &text) { boost::algorithm::trim(text); }

std::string Trim(const std::string &text) {
  std::string temp(text);
  boost::algorithm::trim(temp);
  return temp;
}

std::string FormatDouble(double value, uint8_t decimals, bool fixed,
                         const std::string &unit) {
  // Maximize it to 20 decimals
  if (decimals > 20) {
    decimals = 20;
  }

  // If no decimals, the fixed must be false
  if (decimals == 0) {
    fixed = false;
  }

  std::string text;
  auto value_int = static_cast<int64_t>(value);
  if (value == value_int && !fixed) {
    // If the value actually is an integer then just show it as an integer
    text = std::to_string(value_int);
  } else if (decimals == 0) {
    // Fixed round of float
    value_int = static_cast<int64_t>(std::floor(value + 0.5));
    text = std::to_string(value_int);
  } else {
    char format[20]{};
    sprintf(format, "%%.%df", static_cast<int>(decimals));
    char temp[200]{};
    sprintf(temp, format, value);
    text = temp;
  }

  // If the value is producing to many digits, then convert it to a string with
  // exponent
  const auto size = text.size();
  if (size > static_cast<size_t>(6 + decimals)) {
    char temp[200]{};
    sprintf(temp, "%G", value);
    text = temp;
  }

  // fill or delete trailing '0' but not if using exponent
  const bool have_decimal = text.find('.') != std::string::npos &&
                            text.find('E') == std::string::npos;
  if (!fixed && have_decimal) {
    // Remove trailing zeros
    // Check that it is a decimal point in the string
    while (text.back() == '0') {
      text.pop_back();
    }
  } else if (fixed && have_decimal) {
    const size_t dec_pos = text.find('.');
    const std::string dec = text.substr(dec_pos + 1);
    for (size_t nof_dec = dec.size(); nof_dec < decimals; ++nof_dec) {
      text.push_back('0');
    }
  }

  // We don't want to display '22.' instead of 22
  if (text.back() == '.') {
    text.pop_back();
  }

  if (!unit.empty()) {
    text += " ";
    text += unit;
  }
  return text;
}

bool WildcardMatch(const std::string &text, const std::string &wildcard,
                   bool ignore_case) {
  // Fast return in normal filter cases
  if (wildcard.empty() || wildcard == "*s") {
    return true;
  }

  const char *star = strchr(wildcard.c_str(), '*');
  const char *amper = strchr(wildcard.c_str(), '?');
  if (star == nullptr && amper == nullptr) {
    return ignore_case
               ? IEquals(text, wildcard, wildcard.size())
               : strncmp(text.c_str(), wildcard.c_str(), wildcard.size()) == 0;
  }
  const auto *text_ptr = text.c_str();
  const auto *wild_ptr = wildcard.c_str();
  while (*text_ptr != '\0' && *wild_ptr != '*') {
    if (ignore_case) {
      if (tolower(*wild_ptr) != tolower(*text_ptr) && *wild_ptr != '?') {
        return false;
      }
    } else {
      if (*wild_ptr != *text_ptr && *wild_ptr != '?') {
        return false;
      }
    }
    ++wild_ptr;
    ++text_ptr;
  }
  const char *text_temp = nullptr;
  const char *wild_temp = nullptr;

  while (*text_ptr != '\0') {
    if (*wild_ptr == '*') {
      if (!*++wild_ptr) {
        return true;
      }
      wild_temp = wild_ptr;
      text_temp = text_ptr + 1;
    } else if (ignore_case &&
               (tolower(*wild_ptr) == tolower(*text_ptr) || *wild_ptr == '?')) {
      ++wild_ptr;
      ++text_ptr;
    } else if (!ignore_case && (*wild_ptr == *text_ptr || *wild_ptr == '?')) {
      ++wild_ptr;
      ++text_ptr;
    } else {
      wild_ptr = wild_temp;
      text_ptr = text_temp++;
    }
  }

  while (*wild_ptr == '*') {
    ++wild_ptr;
  }
  return *wild_ptr == '\0';
}

std::string FloatToString(float value) {
  return boost::lexical_cast<std::string>(value);
}

std::string DoubleToString(double value) {
  return boost::lexical_cast<std::string>(value);
}
}  // namespace util::string
