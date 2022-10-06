/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/structureddata.h"

#include <sstream>

namespace util::syslog {

void StructuredData::Identity(const std::string& identity) {
  // Parse out the enterprise number <stem>@<enterprise number>
  bool use_stem = true;
  std::ostringstream stem;
  std::ostringstream number;

  auto temp = identity;
  // Remove '=',' ',']','\\' and '\"' form the input. Replace with underscore
  for (char& input : temp) {
    switch (input) {
      case '=':
      case ' ':
      case ']':
      case '\\':
      case '\"':
        input = '_';
        if (use_stem) {
          stem << input;
        } else {
          number << input;
        }
        break;

      case '@':
        use_stem = false;
        break;

      default:
        if (use_stem) {
          stem << input;
        } else {
          number << input;
        }
        break;
    }
  }

  identity_ = temp;
  stem_ = stem.str();
  enterprise_id_ = number.str();
}

void StructuredData::AddParameter(const std::string& name,
                                  const std::string& value) {
  // Remove '=',' ',']','\\' and '\"' form the input.
  auto temp_name = name;
  for (char& input : temp_name) {
    switch (input) {
      case '=':
      case ' ':
      case ']':
      case '[':
      case '\"':
        input = '_';
        break;

      case '\\':
        input = '/';
        break;

      default:
        break;
    }
  }

  // Remove ']','\\' and '\"' form the input.
  auto temp_value = value;
  for (char& input1 : temp_value) {
    switch (input1) {
      case '[':  // Looking better if followed by a ']'
        input1 = '(';
        break;

      case ']':
        input1 = ')';
        break;

      case '\\':
        input1 = '/';
        break;

      case '\"':
        input1 = '\'';
        break;

      default:
        break;
    }
  }
  if (!temp_name.empty()) {
    parameter_list_.emplace_back(temp_name, temp_value);
  }
}

}  // namespace util::syslog