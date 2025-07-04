/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <chrono>

#include <boost/system.hpp>


#include "processmaster.h"
#include "processapplication.h"
#include "windowsapplication.h"
#include "util/logstream.h"

using namespace std::chrono_literals;
using namespace boost::system;
using namespace boost::asio;
using namespace util::log;

namespace util::supervise {

ProcessMaster::ProcessMaster()
: io_context_(),
  poll_timer_(io_context_) {

}

ProcessMaster::~ProcessMaster() {
  ProcessMaster::Stop();
}

void ProcessMaster::Start() {
  Stop();
  if (io_context_.stopped()) {
    io_context_.restart();
  }
  stop_thread_ = false;
  ISuperviseMaster::Start();
  DoPollTimer();
  supervise_thread_ = std::thread(&ProcessMaster::SuperviseThread, this);

}

void ProcessMaster::Stop() {
  stop_thread_ = true; // Actually supresses log messages
  ISuperviseMaster::Stop(); // Stops all processes

  if (!io_context_.stopped()) {
    io_context_.stop();
  }
  if (supervise_thread_.joinable()) {
    supervise_thread_.join();
  }
}

ISuperviseApplication* ProcessMaster::CreateApplication() {
#ifdef _WIN32
  auto application = std::make_unique<WindowsApplication>(*this);
#else
  auto application = std::make_unique<ProcessApplication>(*this);
#endif
  applications_.push_back(std::move(application));
  auto& app_ptr = applications_.back();
  return app_ptr.get();
}

void ProcessMaster::SuperviseThread() {
  io_context_.run();
}

void ProcessMaster::DoPollTimer() {
  poll_timer_.expires_after(500ms);
  poll_timer_.async_wait([&] (const error_code& ec) -> void {
    if (stop_thread_) {
      return;
    }
    if (ec.failed()) {
      LOG_ERROR() << "Timer error. Error: " << ec.message();
    }
    for (auto& application : applications_) {
      if (!application) {
        continue;
      }
      application->Poll();
    }
    DoPollTimer();
  });
}
} // util::supervise