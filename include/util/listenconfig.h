/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file listenconfig.h
 * \brief Defines an interface to the listen configuration shared memory.
 *
 * The file implements amn interface against the listen configuration shared memory. This memory
 * is shared among all application a specific computer. It is created by the listen daemon application
 * which must be running.
 */
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace util::log {

/** \struct ListenPortConfig listenconfig.h "util/listenconfig.h"
 * \brief Defines a listen config item in the config memory.
 *
 * Defines one listen config item in the config memory.
 */
struct ListenPortConfig {
  uint16_t port; ///< IP port that client connects to.
  std::string name; ///< Short user friendly name of the item.
  std::string share_name; ///< Share name if this is a proxy item.
  std::string description; ///< Description what the item is used for.

  /** \brief Less function that sort on the name property.
   *
   * Less function that sorts on the name property.
   * @param listen Item to compare
   * @return True if this name is less.
   */
  bool operator < (const ListenPortConfig& listen) const {
    return name < listen.name;
  }
};

[[nodiscard]] std::vector<ListenPortConfig> GetListenConfigList();
void AddListenConfig(const ListenPortConfig& port_config);
void DeleteListenConfig(uint16_t port);

class ListenConfig final {
 public:
  ListenConfig();
  ~ListenConfig();
};

} // end namespace




