/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/ilistenclient.h"

namespace util::log {

IListenClient::IListenClient(const std::string& host_name, uint16_t port)
    : host_name_(host_name), port_(port) {}

}  // namespace util::log