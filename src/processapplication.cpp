/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <ctime>
#include <chrono>
#include <thread>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>

#include "util/logstream.h"

#include "processapplication.h"
#include "processmaster.h"

using namespace util::log;
using namespace boost::process;
using namespace boost::process::environment;
using namespace boost::filesystem;
using namespace std::chrono_literals;

namespace util::supervise {

ProcessApplication::ProcessApplication(ProcessMaster& master)
  : master_(master) {

}

ProcessApplication::~ProcessApplication() {
  ProcessApplication::Stop();
}

void ProcessApplication::Start() {
  Stop();
  ResetNofRestarts();
  FindApplication();
  if (Startup() == ApplicationStartup::Manual) {
    InService(false);
  } else {
    InService(true);
  }
  const time_t now = time(nullptr);
  startup_timer_ = now + StartupDelay();

}

void ProcessApplication::Stop() {
  StopProcess();
  State(ApplicationState::Stopped);
  Operable(false);
  InService(false);
}

void ProcessApplication::Poll() {
  const time_t now = time(nullptr);
  if (now < startup_timer_) {
    // Wait for startup
    return;
  }

  switch (Startup()) {
    case ApplicationStartup::Manual:
      PollManual();
      break;

    case ApplicationStartup::Once:
      PollOnce();
      break;

    case ApplicationStartup::Automatic:
    default:
      PollAutomatic();
      break;
  }
}

void ProcessApplication::FindApplication() {
  try {
    if (exists(Path())) {
      Operable(true);
      return;
    }
    const path fullname(Path());
    const path filename = fullname.filename();
    const path exe_path = find_executable(filename);
    if (!exe_path.empty()) {
      LOG_TRACE() << "Found executable path. Path: " << exe_path;
      Path(exe_path.string());
      Operable(true);
      return;
    }

    LOG_ERROR() << "Failed to find application. Exe Path: " << Path();
    Operable(false);
  } catch ( const std::exception& err ) {
    LOG_ERROR() << "Failed to find application. Error: " << err.what();
    Operable(false);
  }
}

bool ProcessApplication::StartProcess() {

  if (process_) {
    StopProcess();
    LOG_ERROR() << "Process seems to be running. Name:" << Name();
  }
  bool start = false;
  try {
    path exe_path(Path());
    const auto arg_list = ParseCommandLine();
    process_ = std::make_unique<process>(master_.io_context_, exe_path,
      arg_list);
    start = true;;
    for (size_t timeout = 0; timeout < 20; ++timeout) {
      if (process_->running()) {
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
    if (!process_->running()) {
      LOG_ERROR() << "Slow start of executable. Name:" << Name();
    } else {
      LOG_INFO() << "Started application. Name: " << Name();
    }
  } catch (const std::exception& err) {
    // Failed to start
    LOG_ERROR() << "Failed to start process. Process: " << Name()
        << ", Error: " << err.what();
    Operable(false);
  }
  return start;
}

bool ProcessApplication::IsRunning() const {
  return process_ && process_->running();
}

void ProcessApplication::StopProcess() {

  if (!process_) {
    return;
  }
  try {
    process_->interrupt();
    process_->request_exit();
    for (size_t timeout = 0; timeout < 40; ++timeout) {
      if (!process_->running()) {
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
    if (process_->running()) {
      LOG_ERROR() << "Application not stopped within 4 ssecond. Name:" << Name();
      process_->terminate();
    }
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to stop process. Error: " << err.what();
  }
  process_.reset();

}

void ProcessApplication::PollManual() {
  const time_t now = time(nullptr);
  switch (State()) {
    case ApplicationState::Stopped:
      if (InService() && now >= startup_timer_) {
        const bool start = StartProcess();;
        State(start ? ApplicationState::Running : ApplicationState::Failing);
      } else {
        Operable(false);
      }
      break;

      case ApplicationState::Running:
      if (!InService() || !IsRunning()) {
        // Manual normal stop
        StopProcess();
        State(ApplicationState::Ready);
        Operable(false);
      } else {
        Operable(true);
      }
      break;

      case ApplicationState::Ready:
      case ApplicationState::Failing:
      default:
        // Manual shouldn't be in this state.
      StopProcess();
      Operable(false);
      if (!InService()) {
        State(ApplicationState::Stopped);
      }
      break;
  }
}

void ProcessApplication::PollOnce() {
  const time_t now = time(nullptr);
  switch (State()) {
    case ApplicationState::Stopped:
      if (InService() && now >= startup_timer_) {
        const bool start = StartProcess();;
        State(start ? ApplicationState::Running : ApplicationState::Failing);
      } else {
        Operable(false);
      }
      break;

      case ApplicationState::Running:
      if (!InService()) {
        LOG_TRACE() << "Manual stop requested. Name: " << Name();
        StopProcess();
        State(ApplicationState::Stopped);
        Operable(false);
      } else if (!IsRunning()) {
        LOG_TRACE() << "The application stopped. Name: " << Name();
        StopProcess();
        State(ApplicationState::Ready);
        Operable(false);
      } else {
        Operable(true);
      }
      break;

      case ApplicationState::Ready:
      case ApplicationState::Failing:
      default:
      StopProcess();
      Operable(false);
      if (!InService()) {
        State(ApplicationState::Stopped);
      }
      break;
  }
}

void ProcessApplication::PollAutomatic() {
  const time_t now = time(nullptr);
  switch (State()) {
    case ApplicationState::Stopped:
      if (InService() &&  now >= startup_timer_) {
        const bool start = StartProcess();
        State(start ? ApplicationState::Running : ApplicationState::Failing);
      } else {
        Operable(false);
      }
      break;

    case ApplicationState::Running:
      if (!InService()) {
        // Manual stop
        StopProcess();
        State(ApplicationState::Stopped);
        Operable(false);
        LOG_INFO() << "Manual stop in automatic mode. Name: " << Name();
      } else if (!IsRunning()) {
        // Failing
        StopProcess();
        State(ApplicationState::Failing);
        Operable(false);
        LOG_ERROR() << "Application stopped/failing in automatic mode. Name: " << Name();
      } else {
        Operable(true);
      }
      break;

    case ApplicationState::Ready:
    case ApplicationState::Failing:
    default:
      // Manual shouldn't be in this state.
      StopProcess();
      Operable(false);
      if (InService()) {

        if (NofRestarts() < 1) {
          startup_timer_ = now + 2;
          State(ApplicationState::Stopped);
          IncrementNofRestarts();
          LOG_INFO() << "Fast restart of application. Name: " << Name();
        } else if (NofRestarts() < 10) {
          startup_timer_ = now + 10;
          State(ApplicationState::Stopped);
          IncrementNofRestarts();
          LOG_INFO() << "Slow restart of application. Name: " << Name();
        }
      } else {
        ResetNofRestarts();
        State(ApplicationState::Stopped);
      }
      break;
  }
}
} // util