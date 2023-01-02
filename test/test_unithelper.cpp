/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>

#include "util/unithelper.h"

using namespace util::string;

TEST(UnitHelper, QuantityList) {
  const auto list = UnitHelper::QuantityList();
  for (const auto& quantity : list) {
    std::cout << quantity << std::endl;
    const auto unit_list = UnitHelper::UnitsByQuantity(quantity);
    for (const auto& unit : unit_list) {
      std::cout << "\t" << unit << std::endl;
    }
  }
}

TEST(UnitHelper, UnitList) {
  const auto list = UnitHelper::UnitList();
  for (const auto& unit : list) {
    std::cout << unit << std::endl;
  }
}