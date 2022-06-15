/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file stringparser.h
 * \brief Helper class that splits a string into various tags
 *
 * Support class that splits a string into tags based upon an expression patter.
 */
#pragma once

#include <string>
#include <vector>
namespace util::string {

/** \class StringParser stringparser.h "util/stringparser.h"
 * \brief Helper class that splits a string.
 *
 * Support class that splits a string into tags depending on an expression.
 */
class StringParser {
 public:
  explicit StringParser(const std::string &expression); ///< Constructor
  bool Parse(const std::string& text); ///< Parses the string

  [[nodiscard]] bool ExistTag(const std::string& tag) const; ///< Test if a tag exist
  [[nodiscard]] std::string GetTagValue(const std::string& tag) const; ///< Returns the tag value

 private:
  struct ParserElement {
    bool is_tag = false;
    std::string name;
    std::string value;
  };
  std::vector<ParserElement> element_list_;
};

} // end namespace




