/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <gtest/gtest.h>


namespace util::test {

class TestSyslog : public ::testing::Test {
 protected:
  static void SetUpTestCase();
  static void TearDownTestCase();
};
} // namespace util::test}