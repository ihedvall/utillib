/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "listenproxy.h"

#include <sstream>
namespace util::log::detail {

ListenProxy::ListenProxy(const std::string &share_name)
    : IListen(), queue_(false, share_name) {}

ListenProxy::~ListenProxy() { queue_.Stop(); }

bool ListenProxy::IsActive() const { return queue_.IsActive(); }

size_t ListenProxy::LogLevel() { return queue_.LogLevel(); }

void ListenProxy::AddMessage(uint64_t nano_sec_1970,
                             const std::string &pre_text,
                             const std::string &text) {
  if (!IsActive()) {
    return;
  }

  // Need to adjust pre_text and text so the fit into the shared memory messages
  std::string temp = pre_text;
  while (temp.size() >= sizeof(SharedListenMessage::pre_text)) {
    temp.pop_back();
  }

  // Check if the message fit into
  if (text.size() < sizeof(SharedListenMessage::text) - 1) {
    SharedListenMessage msg;
    msg.ns1970 = nano_sec_1970;
    strcpy(msg.pre_text, temp.c_str());
    strcpy(msg.text, text.c_str());
    queue_.Add(msg);
    return;
  }

  // Split into more messages
  size_t msg_count = 0;
  size_t count = 0;
  std::ostringstream out;

  for (const char in_char : text) {
    bool good_line_break = false;
    switch (in_char) {
      case ' ':
      case '\n':
      case '\r':
      case '\t':good_line_break = true;
        break;

      default:break;
    }

    if (count >= sizeof(SharedListenMessage::text) - 1) {
      // Send message with ugly line wrap
      SharedListenMessage msg;
      msg.ns1970 =
          msg_count == 0 ? nano_sec_1970 : 0;  // Indicate wrapped messages
      strcpy(msg.pre_text, temp.c_str());
      strcpy(msg.text, out.str().c_str());
      queue_.Add(msg);
      ++msg_count;

      out.str("");
      out.clear();
      count = 0;
    } else if (good_line_break && count >= 80) {
      // Send message with nice line wrap
      SharedListenMessage msg;
      msg.ns1970 =
          msg_count == 0 ? nano_sec_1970 : 0;  // Indicate wrapped messages
      strcpy(msg.pre_text, temp.c_str());
      strcpy(msg.text, out.str().c_str());
      queue_.Add(msg);
      ++msg_count;

      out.str("");
      out.clear();
      count = 0;
    }

    out << in_char;
    ++count;
  }
  if (!out.str().empty()) {
    SharedListenMessage msg;
    msg.ns1970 =
        msg_count == 0 ? nano_sec_1970 : 0;  // Indicate wrapped messages
    strcpy(msg.pre_text, temp.c_str());
    strcpy(msg.text, out.str().c_str());
    queue_.Add(msg);
  }
}
void ListenProxy::SetLogLevel(size_t log_level) {
  queue_.SetLogLevel(static_cast<uint8_t>(log_level));
}

}  // namespace util::log::detail
