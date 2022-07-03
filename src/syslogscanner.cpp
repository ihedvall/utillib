/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "syslogscanner.h"
namespace util::syslog {

SyslogScanner::SyslogScanner(std::istringstream& message)
: yyFlexLexer(&message),
  yylval(nullptr)
{
}


} // end namespace