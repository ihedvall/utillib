/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <chrono>
#include <boost/asio.hpp>
#include "util/timestamp.h"
#include "util/logstream.h"
#include "syslog.h"
#include "util/syslogmessage.h"

using namespace std::chrono_literals;
using namespace boost::asio;
using namespace util::syslog;
namespace util::log::detail {

Syslog::Syslog(const std::string &remote_host, uint16_t port)
    : remote_host_(remote_host),
      port_(port) {
  // Turn of debug and trace messages. Info level is questionable.
  EnableSeverityLevel(LogSeverity::kTrace, false);
  EnableSeverityLevel(LogSeverity::kDebug, false);
  StartWorkerThread();
}

void Syslog::StartWorkerThread() {
  stop_thread_ = false;
  worker_thread_ = std::thread(&Syslog::WorkerThread, this);
}

void Syslog::WorkerThread() {
  io_context context;
  ip::udp::resolver resolver(context);
  bool in_service = true; // Suppresses event and alarms

  // We have the messages in a queue, so there is no hurry to transfer them
  // to the syslog server.
  do {
    std::unique_lock<std::mutex> lock(locker_);
    condition_.wait_for(lock, 2s, [&] {
      return stop_thread_.load();
    });

    while(!message_list_.empty()) {
        // Connect to the syslog server
      LogMessage m = message_list_.front();
      message_list_.pop();
      SyslogMessage msg(m, ShowLocation());
      const auto data = msg.GenerateMessage();
      const auto buffer = boost::asio::buffer(data);

      lock.unlock();
      try {
        auto end_points = resolver.resolve(ip::udp::v4(), remote_host_, std::to_string(port_));
        ip::udp::socket socket(context);
        socket.open(ip::udp::v4());
        const auto points = end_points.size();
        socket.send_to(buffer, *end_points.begin());
        if (!in_service) {
          LOG_INFO() << "Syslog client is in service.";
        }
        in_service = true;
      } catch (const std::exception& err) {
         // If something wrong clear the message buffer
         if (in_service) {
           LOG_ERROR() << "Syslog is out-of-service. Error: " << err.what()
                       << ", Remote: " << remote_host_ << ":" << port_;
         }
         in_service = false;
      }
      lock.lock();
    }
  } while (!stop_thread_);
}



/**
 * Adds a log message to the internal message queue. The queue is saved to a file by a worker thread.
 * @param [in] message Message to handle.
 */
void Syslog::AddLogMessage(const LogMessage &message) {
  if (stop_thread_ || !IsSeverityLevelEnabled(message.severity)) {
    return;
  }
  {
    std::lock_guard<std::mutex> lock(locker_);
    if (stop_thread_) {
      return;
    }
    message_list_.push(message);
  }
  condition_.notify_one();
}

/**
 * Stops the working thread. This means that all messages in the queue is saved to the file.
 */
void Syslog::Stop() {
  stop_thread_ = true;
  if (worker_thread_.joinable()) {
    condition_.notify_one();
    worker_thread_.join();
  }
  stop_thread_ = false;
}



}