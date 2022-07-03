/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file structureddata.h
 * \brief Syslog structured data interface.
 *
 * Reporting alarm and location information in a syslog message, requires
 * using so-called structure data. The data consist of a data identity and one
 * or more name value pair.
 */
#pragma once
#include <string>
#include <vector>

namespace util::syslog {

/** \class StructuredData structureddata.h "uril/structureddata.h"
 * \brief Defines a simple interface against structured data item.
 *
 * A structured data item is used to tag extra information onto a
 * syslog message. The item has a unique identifier and one or more
 * name value pairs.
 */
class StructuredData {
 public:
 /** \typedef ParameterList
 * The list holds name value pairs. The name and values are always strings.
 */
  using ParameterList = std::vector<std::pair<std::string, std::string>>;

  /** \brief Sets the identifier for the data item;
   *
   * Sets the identifier for the structure data.
   * @param identity String
   */
  void Identity(const std::string& identity);

  /** \brief Returns the data raw identifier.
   *
   * @return Raw message structured data name.
   */
  [[nodiscard]] const std::string& Identity() const {
    return identity_;
  }

  /** \brief Return the main (stem) part of the identifier.
   *
   * The identifier may be split into 2 parts. The at symbol ('@'), is used as
   * token that divide the stem and the enterprise ID.
   * @return The stem part  of the identifier.
   */
  [[nodiscard]] const std::string& IdentityStem() const {
    return stem_;
  }

  /** \brief Returns the enterprise ID.
   *
   * The identifier may be split into 2 parts. The at symbol ('@'), is used as
   * token that divide the stem and the enterprise ID.
   * @return The enterprise ID.
   */
  [[nodiscard]] const std::string& EnterpriseId() const {
    return enterprise_id_;
  }

  /** \brief Returns a list of name value pairs.
   *
   * Returns the list of name value pairs belonging to this
   * data item.
   * @return List of name value pairs
   */
  [[nodiscard]] const ParameterList& Parameters() const {
    return parameter_list_;
  }

  /** \brief Adds a name value pair.
   *
   * Adds a new name value pair. The name and values are always strings.
   * @param name Name of value.
   * @param value The value.
   */
  void AddParameter(const std::string& name, const std::string& value);

 private:
  std::string identity_; ///< Identity As in message.
  std::string stem_;     ///< Identity without any '@'.
  std::string enterprise_id_; ///< Is actually an IANA enterprise ID (number).
  ParameterList parameter_list_; ///< List of name value pairs.
};

} // end namespace
