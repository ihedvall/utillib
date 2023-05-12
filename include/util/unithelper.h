/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file unithelper.h
 * \brief Support class when creating unit of measure and quantities groups
 *
 */
#pragma once
#include <string>
#include <vector>

namespace util::string {

/** \brief Unit - quantity -physical dimension relation
 *
 * Quantity has the same physical dimension with one or more units.
 * Example: 'Length' is a quantity while 'm', 'km' and 'miles' are units.
 * The factor and offset defines the conversion between the units. The 'mile'
 * unit has the factor = 1609.344, offset = 0 and physical dimension meter = 1.
 */
struct UnitQuantity {
  std::string Quantity;  ///< Quantity name.
  std::string Unit;      ///< Unit of measure (ASCII not UTF8 coded).
  double Factor = 1;     ///< Factor
  double Offset = 0;     ///< Offset
  int Meter = 0;         ///< Physical dimension (m)
  int Kilogram = 0;      ///< Physical dimension (kg)
  int Seconds = 0;       ///< Physical dimension (s)
  int Kelvin = 0;        ///< Physical dimension (K)
  int Ampere = 0;        ///< Physical dimension (A)
  int Mol = 0;           ///< Physical dimension (mol)
  int Candela = 0;       ///< Physical dimension (cd)
};

/** \brief List of unit quantity physical dimension relation
 *
 */
using UnitQuantityList = std::vector<UnitQuantity>;

/** \brief Support class that wrap useful Unit functions.
 *
 */
class UnitHelper {
 public:
  /** \brief Returns a list of unit/quantity relations.
   *
   * Returns the default unit/quantity list.
   * @return List of UnitQuantity structures.
   */
  static UnitQuantityList DefaultUnitQuantityList();

  /** \brief Returns the default quantity lists.
   *
   * Returns a the default quantity list
   * @return List of quantities
   */
  static std::vector<std::string> QuantityList();

  /** \brief Returns a list of units by a quantity.
   *
   * Returns a list of units with a quantity.
   * @param quantity Name of quantity.
   * @return List of associated units.
   */
  static std::vector<std::string> UnitsByQuantity(const std::string& quantity);

  /** \brief Returns a list of all units.
   *
   * Returns a list of all units. Note that the unit string is ISO-8859-1
   * coded.
   * @return List of all units.
   */
  static std::vector<std::string> UnitList();
};

}  // namespace util::string
