/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/consoleapp.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <filesystem>
#include <mutex>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std::filesystem;
using namespace std::chrono_literals;
namespace {

std::atomic<bool> kStopMain = false;
std::condition_variable kStopCondition;
std::mutex kStopMutex;

void StopMainHandler(int) {
  kStopMain = true;
  kStopCondition.notify_all();
  kStopMain = true;
  kStopCondition.notify_all();
  // Wait for OnExit finalize before exit
  for (size_t timeout = 0; kStopMain && timeout < 20; ++timeout) {
    std::this_thread::sleep_for(100ms);
  }
}

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
  switch (fdwCtrlType) {
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_C_EVENT:
      kStopMain = true;
      kStopCondition.notify_all();
      // Wait for OnExit finalize before exit
      for (size_t timeout = 0; kStopMain && timeout < 20; ++timeout) {
        std::this_thread::sleep_for(100ms);
      }
      return TRUE;

    default:
      return FALSE;
  }
}
#endif

}  // namespace

namespace util {
ConsoleApp::ConsoleApp() {
  kStopMain = false;

  signal(SIGTERM, StopMainHandler);
  signal(SIGABRT, StopMainHandler);
  signal(SIGINT, StopMainHandler);
#if (_WIN32)
  signal(SIGABRT_COMPAT, StopMainHandler);
  signal(SIGBREAK, StopMainHandler);
  signal(SIGINT, StopMainHandler);
  SetConsoleCtrlHandler(CtrlHandler, TRUE);
#endif
}

int ConsoleApp::MainFunc(int argc, char* argv[]) {
  for (int index = 0; index < argc; ++index) {
    if (index == 0) {
      app_full_name_ = argv[0];
    } else {
      arg_list_.emplace_back(argv[index]);
    }
  }

  const bool init = OnInit();

  if (init) {
    OnRun();
    OnExit();
  }
  return ExitCode();
}

void ConsoleApp::StopMain() {
  kStopMain = true;
  kStopCondition.notify_all();
}

bool ConsoleApp::ShallStop() { return kStopMain; }

std::string ConsoleApp::AppFullName() const { return app_full_name_; }

std::string ConsoleApp::AppName() const {
  std::string name;
  try {
    path fullname(app_full_name_);
    name = fullname.stem().string();
  } catch (const std::exception&) {
    name = "InvalidName";
  }
  return name;
}

std::string ConsoleApp::AppPath() const {
  std::string parent_path;
  try {
    path fullname(app_full_name_);
    parent_path = fullname.parent_path().string();
  } catch (const std::exception&) {
    parent_path = "InvalidPath";
  }
  return parent_path;
}

std::vector<std::string> ConsoleApp::Args() const { return arg_list_; }

bool ConsoleApp::OnInit() { return true; }

void ConsoleApp::OnRun() {
#ifdef _WIN32
  RunWindows();
#else
  RunLinux();
#endif
}

void ConsoleApp::OnExit() { kStopMain = false; }

void ConsoleApp::OnPoll() {}

void ConsoleApp::RunWindows() {
  while (!kStopMain) {
#ifdef _WIN32
    MSG msg = {};
    if (const BOOL peek_msg = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        peek_msg) {
      switch (msg.message) {
        case WM_QUIT:
        case WM_CLOSE:
          // This is a normal stop.
          kStopMain = true;
          continue;

        default:
          break;
      }
    }
#endif
    OnPoll();
    std::unique_lock lock(kStopMutex);
    kStopCondition.wait_for(lock, std::chrono::milliseconds(poll_delay_),
                            []() -> bool { return kStopMain.load(); });
  }
}

void ConsoleApp::RunLinux() {
  while (!kStopMain) {
    OnPoll();
    std::unique_lock lock(kStopMutex);
    kStopCondition.wait_for(lock, std::chrono::milliseconds(poll_delay_),
                            []() -> bool { return kStopMain.load(); });
  }
}

}  // namespace util