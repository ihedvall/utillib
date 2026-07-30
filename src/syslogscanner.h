/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <sstream>
#include <string>
#include <variant>

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

#include "syslogparser.hpp"
#include "util/syslogmessage.h"
namespace util::syslog {

class SyslogScanner : public yyFlexLexer, public util::syslog::SyslogMessage {
 public:
  explicit SyslogScanner(std::istringstream& in);
  int yylex(SyslogParser::value_type* yylval);

  const std::string& Origin() const { return input_text_;}
 private:
  std::string input_text_;
  SyslogParser::semantic_type* yylval = nullptr;
};

}  // namespace util::syslog
