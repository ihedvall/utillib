/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace util {

class ConsoleApp {
 public:
  ConsoleApp();
  int MainFunc(int argc, char* argv[]);
  static void StopMain();
  [[nodiscard]] static bool ShallStop();

  [[nodiscard]] std::string AppFullName() const;
  [[nodiscard]] std::string AppName() const;
  [[nodiscard]] std::string AppPath() const;
  [[nodiscard]] std::vector<std::string> Args() const;

  void ExitCode(int code) { exit_code_ = code; }
  [[nodiscard]] int ExitCode() const { return exit_code_; }

  void PollDelay(uint32_t delay) { poll_delay_ = delay; }
  [[nodiscard]] uint32_t PollDelay() const { return poll_delay_; }

 protected:
  virtual bool OnInit();
  virtual void OnRun();
  virtual void OnPoll();
  virtual void OnExit();

  void RunWindows();
  void RunLinux();

 private:
  std::string app_full_name_;
  std::vector<std::string> arg_list_;
  uint32_t poll_delay_ = 100;  // Delay in ms.
  int exit_code_ = EXIT_SUCCESS;
};

}  // namespace util
