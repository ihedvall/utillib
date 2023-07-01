/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/unithelper.h"
#define BOOST_NO_AUTO_PTR
#include <boost/locale.hpp>
#include <numbers>
#include <set>

#include "util/stringutil.h"

using namespace std::numbers;
using namespace boost::locale::conv;

namespace {
//    Quantity, Unit, Factor, Offset, m, kg, s, K,,A, mol, cd
util::string::UnitQuantityList default_unit_list = {
    {"No unit", "", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"No unit", "-", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"No unit", "---", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Length", "m", 1, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "inch", 0.0254, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "ft", 0.3048, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "yd", 0.9144, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "mm", 0.001, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length",
     "\xB5"
     "m",
     1.0 / 1E6, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "um", 1.0 / 1E6, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "dm", 0.1, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "km", 1000, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Length", "mile", 1609.344, 0, 1, 0, 0, 0, 0, 0, 0},
    {"Mass", "kg", 1, 0, 0, 1, 0, 0, 0, 0, 0},
    {"Mass", "hg", 1.0 / 10, 0, 0, 1, 0, 0, 0, 0, 0},
    {"Mass", "g", 1.0 / 1E3, 0, 0, 1, 0, 0, 0, 0, 0},
    {"Time", "s", 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Time", "ms", 1.0 / 1E3, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Time", "msec", 1.0 / 1E3, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Time", "us", 1 / 1E6, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Time",
     "\xB5"
     "s",
     1 / 1E6, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Time", "ns", 1 / 1E9, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Time", "h", 3600, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Time", "min", 60, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Temperature", "K", 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {"Temperature",
     "\xB0"
     "C",
     1, 273.15, 0, 0, 0, 1, 0, 0, 0},
    {"Temperature", "Grad C", 1, 273.15, 0, 0, 0, 1, 0, 0, 0},
    {"Temperature",
     "\xB0"
     "F",
     5.0 / 9, 459.67, 0, 0, 0, 1, 0, 0, 0},
    {"Electric Current", "A", 1, 0, 0, 0, 0, 0, 1, 0, 0},
    {"Electric Current", "mA", 0.001, 0, 0, 0, 0, 0, 1, 0, 0},
    {"Amount of Substance", "mol", 1, 0, 0, 0, 0, 0, 0, 1, 0},
    {"Luminous Intensity", "cd", 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {"Area", "m\xB2", 1, 0, 2, 0, 0, 0, 0, 0, 0},
    {"Area", "dm\xB2", 1.0 / 1E2, 0, 2, 0, 0, 0, 0, 0, 0},
    {"Area", "cm\xB2", 1.0 / 1E4, 0, 2, 0, 0, 0, 0, 0, 0},
    {"Volume", "m\xB3", 1, 0, 3, 0, 0, 0, 0, 0, 0},
    {"Volume", "dm\xB3", 1.0 / 1E3, 0, 3, 0, 0, 0, 0, 0, 0},
    {"Volume", "dm3", 1.0 / 1E3, 0, 3, 0, 0, 0, 0, 0, 0},
    {"Volume", "mm\xB3", 1.0 / 1E9, 0, 3, 0, 0, 0, 0, 0, 0},
    {"Volume", "l", 1.0 / 1E3, 0, 3, 0, 0, 0, 0, 0, 0},
    {"Volume", "cm\xB3", 1.0 / 1E6, 0, 3, 0, 0, 0, 0, 0, 0},
    {"Volume", "lambda", 1.0 / 1E9, 0, 3, 0, 0, 0, 0, 0, 0},
    {"Speed", "m/s", 1.0, 0.0, 1, 0, -1, 0, 0, 0, 0},
    {"Speed", "mm/s", 1.0 / 1E3, 0, 1, 0, -1, 0, 0, 0, 0},
    {"Speed", "km/h", 1000 / 3600.0, 0, 1, 0, -1, 0, 0, 0, 0},
    {"Speed", "mph", 1609.344 / 3600, 0, 1, 0, -1, 0, 0, 0, 0},
    {"Wave number", "1/m", 1, 0, -1, 0, 0, 0, 0, 0, 0},
    {"Density", "kg/m\xB3", 1, 0, -3, 1, 0, 0, 0, 0, 0},
    {"Density",
     "kg/dm"
     "\xB3",
     1000, -3, 1, 0, 0, 0, 0, 0},
    {"Inertia", "kgm\xB2", 1, 0, 2, 1, 0, 0, 0, 0, 0},
    {"Rotational Speed", "rps", 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Rotational Speed", "rpm", 1.0 / 60, 0, 0, 0, 1, 0, 0, 0, 0},
    {"Order", "ord", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Acceleration", "m/s\xB2", 1, 0, 1, 0, -2, 0, 0, 0, 0},
    {"Angular Acceleration",
     "deg/s"
     "\xB2",
     pi / 180, 0, 0, 0, -2, 0, 0, 0, 0},
    {"Angular Acceleration",
     "rad/s"
     "\xB2",
     1, 0, 0, 0, -2, 0, 0, 0, 0},
    {"Angular Velocity", "rad/s", 1, 0, 0, 0, -1, 0, 0, 0, 0},
    {"Angular Velocity", "deg/s", pi / 180, 0, 0, 0, -1, 0, 0, 0, 0},
    {"Plane Angle", "rad", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Solid Angle", "sr", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Plane Angle", "deg", pi / 180, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Plane Angle", "\xB0", pi / 180, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Percent", "%", 1.0 / 100, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Percent", "%*100", 1.0 / 10000, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Per mille", "\x89", 1.0 / 1000, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Stiffness", "N/m", 1, 0, 0, 1, -2, 0, 0, 0, 0},
    {"Pressure",
     "N/m"
     "\xB2",
     1, 0, -1, 1, -2, 0, 0, 0, 0},
    {"Pressure", "%/hPa", 10.0 / 100, 0, -1, 1, -2, 0, 0, 0, 0},
    {"Pressure", "hPa", 0.1, 0, -1, 1, -2, 0, 0, 0, 0},
    {"Pressure", "bar", 1E5, 0, -1, 1, -2, 0, 0, 0, 0},
    {"Pressure", "Pa", 1, 0, -1, 1, -2, 0, 0, 0, 0},
    {"Pressure", "kPa", 1E3, 0, -1, 1, -2, 0, 0, 0, 0},
    {"Pressure", "MPa", 1E6, 0, -1, 1, -2, 0, 0, 0, 0},
    {"Frequency", "Hz", 1, 0, 0, 0, -1, 0, 0, 0, 0},
    {"Force", "N", 1, 0, 1, 1, -2, 0, 0, 0, 0},
    {"Specific Volume", "m\xB3/kg", 1, 0, 3, -1, 0, 0, 0, 0, 0},
    {"Current Density", "A/m\xB2", 1, 0, -2, 0, 0, 0, 1, 0, 0},
    {"Magnetic Field Strength", "A/m", 1, 0, -1, 0, 0, 0, 1, 0, 0},
    {"Substance Concentration", "mol/m\xB3", 1, 0, -3, 0, 0, 0, 0, 1, 0},
    {"Luminance", "cd/m\xB2", 1, 0, -2, 0, 0, 0, 0, 0, 1},
    {"Energy", "J", 1, 0, 2, 1, -2, 0, 0, 0, 0},
    {"Energy", "kJ", 1E3, 0, 2, 1, -2, 0, 0, 0, 0},
    {"Energy", "Nm", 1, 0, 2, 1, -2, 0, 0, 0, 0},
    {"Energy", "% Nm", 1.0 / 100, 0, 2, 1, -2, 0, 0, 0, 0},
    {"Power", "W", 1, 0, 2, 1, -3, 0, 0, 0, 0},
    {"Power", "kW", 1E3, 0, 2, 1, -3, 0, 0, 0, 0},
    {"Power", "MW", 1E6, 0, 2, 1, -3, 0, 0, 0, 0},
    {"Electric Charge", "C", 1, 0, 0, 0, 1, 0, 1, 0, 0},
    {"Electric Voltage", "V", 1, 0, 2, 1, -3, 0, -1, 0, 0},
    {"Electric Voltage", "mV", 0.001, 0, 2, 1, -3, 0, -1, 0, 0},
    {"Electric Voltage", "kV", 1E3, 0, 2, 1, -3, 0, -1, 0, 0},
    {"Electric Voltage", "VDC", 1, 0, 2, 1, -3, 0, -1, 0, 0},
    {"Electric Load", "Ah", 3600, 0, 0, 0, 1, 0, 1, 0, 0},
    {"Capacitance", "F", 1, 0, 0, 0, 1, 0, 1, 0, 0},
    {"Mass Fraction", "kg/kg", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Resistance", "ohm", 1, 0, 2, 1, -3, 0, -2, 0, 0},
    {"Resistance", "Ohm", 1, 0, 2, 1, -3, 0, -2, 0, 0},
    {"Conductance", "S", 1, 0, -2, -1, 3, 0, 2, 0, 0},
    {"Flux", "Wb", 1, 0, 2, 1, -2, 0, -1, 0, 0},
    {"Flux", "Vs", 1, 0, 2, 1, -2, 0, -1, 0, 0},
    {"Flux Density",
     "Wb/m"
     "\xB2",
     1, 0, 0, 1, -2, 0, -1, 0, 0},
    {"Flux Density", "T", 1, 0, 0, 1, -2, 0, -1, 0, 0},
    {"Inductance", "H", 1, 0, 2, 1, -2, 0, -2, 0, 0},
    {"Inductance", "Wb/A", 1, 0, 2, 1, -2, 0, -2, 0, 0},
    {"Luminous Flux", "lm", 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {"Illuminations", "lx", 1, 0, -2, 0, 0, 0, 0, 0, 1},
    {"Illuminations", "lm/m\xB2", 1, 0, -2, 0, 0, 0, 0, 0, 1},
    {"Activity", "Bq", 1, 0, 0, 0, -1, 0, 0, 0, 0},
    {"Dose", "Gy", 1, 0, 2, 0, -2, 0, 0, 0, 0},
    {"Dose", "J/kg", 1, 0, 2, 0, -2, 0, 0, 0, 0},
    {"Dose", "MJ/kg", 1E6, 0, 2, 0, -2, 0, 0, 0, 0},
    {"Dose", "Sv", 1, 0, 2, 0, -2, 0, 0, 0, 0},
    {"Catalytic Activity", "kat", 1, 0, 0, 0, -1, 0, 0, 1, 0},
    {"Cycle", "Cycles", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Cycle", "cycle", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Cycle", "pulses", 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Size", "ppm", 1.0 / 1E6, 0, 0, 0, 0, 0, 0, 0, 0},
    {"Logarithmic Unit", "dB", 1, 0, 0, 0, 0, 0, 0, 0, 0},
};

}  // namespace

namespace util::string {

UnitQuantityList UnitHelper::DefaultUnitQuantityList() {
  UnitQuantityList list(default_unit_list);
  for (auto& temp : list) {
    std::string unit;
    try {
      unit = to_utf<char>(temp.Unit, "ISO-8859-8");
    } catch (const std::exception&) {
      unit = temp.Unit;
    }
    temp.Unit = unit;
  }
  return list;
}

std::vector<std::string> UnitHelper::QuantityList() {
  std::multiset<std::string, IgnoreCase> temp;
  for (const auto& unit : default_unit_list) {
    temp.insert(unit.Quantity);
  }
  std::vector<std::string> list;
  for (auto& quantity : temp) {
    list.emplace_back(quantity);
  }
  return list;
}

std::vector<std::string> UnitHelper::UnitsByQuantity(
    const std::string& quantity) {
  std::vector<std::string> list;
  for (const auto& temp : default_unit_list) {
    if (IEquals(temp.Quantity, quantity)) {
      std::string unit;
      try {
        unit = to_utf<char>(temp.Unit, "WINDOWS-1252");
      } catch (const std::exception&) {
        unit = temp.Unit;
      }
      list.emplace_back(unit);
    }
  }
  return list;
}

std::vector<std::string> UnitHelper::UnitList() {
  std::multiset<std::string, IgnoreCase> sorted_list;
  for (const auto& temp : default_unit_list) {
    std::string unit;
    try {
      unit = to_utf<char>(temp.Unit, "WINDOWS-1252");
    } catch (const std::exception&) {
      unit = temp.Unit;
    }
    sorted_list.insert(unit);
  }
  std::vector<std::string> list;
  for (auto& utf8 : sorted_list) {
    list.emplace_back(utf8);
  }
  return list;
}

}  // namespace util::string