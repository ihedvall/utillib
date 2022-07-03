/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <string>
#include <sstream>
#include <chrono>

#include <gtest/gtest.h>
#include "util/logconfig.h"
#include "util/logstream.h"
#include "syslogscanner.h"
#include "syslogparser.hpp"

using namespace std::chrono_literals;


namespace util::test {

TEST(SysLogFlexer, TestBasicParse) {

  std::istringstream msg("<654>1 2022-10-11T00:03:04Z Olle - PID34 MSG55 [ParId olle=\"Pelle\" olle1=\"pelle1\"][Par@123 Over=\"Under\"] "
                         "'\xEF' '\xBB' '\xBF'"
                         "ÅäÖ was here!");
  syslog::SyslogScanner scanner(msg);
  syslog::SyslogParser parser(scanner);
  scanner.set_debug(1);

  const auto ret = parser.parse();

  std::cout << "parse: " << ret << std::endl;

}

} // end namespace