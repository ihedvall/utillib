/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include "util/ipubsubclient.h"
#include "util/stringutil.h"

using namespace util::string;

namespace util::mqtt {

ITopic *IPubSubClient::GetTopic(const std::string &topic_name) {
  auto itr = std::ranges::find_if(topic_list_,[&] (const auto& topic) {
    return topic && topic_name == topic->Topic();
  });
  return itr == topic_list_.end() ? nullptr : itr->get();
}

ITopic *IPubSubClient::GetITopic(const std::string &topic_name) {
  auto itr = std::ranges::find_if(topic_list_,[&] (const auto& topic) {
    return topic && IEquals(topic_name,topic->Topic());
  });
  return itr == topic_list_.end() ? nullptr : itr->get();
}
} // end namespace mqtt