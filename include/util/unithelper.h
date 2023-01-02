/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <vector>

namespace util::string {

struct UnitQuantity {
  std::string Quantity;
  std::string Unit;
  double Factor = 1;
  double Offset = 0;
  int Meter = 0;
  int Kilogram = 0;
  int Seconds = 0;
  int Kelvin = 0;
  int Ampere = 0;
  int Mol = 0 ;
  int Candela = 0;
};

using UnitQuantityList = std::vector<UnitQuantity>;

class UnitHelper {
 public:
  static UnitQuantityList DefaultUnitQuantityList();
  static std::vector<std::string> QuantityList();
  static std::vector<std::string> UnitsByQuantity(const std::string& quantity);
  static std::vector<std::string> UnitList();
};

}  // namespace util::string
