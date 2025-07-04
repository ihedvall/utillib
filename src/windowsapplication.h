/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#ifdef _WIN32

#include <boost/asio.hpp>

#include "processapplication.h"

namespace util::supervise {

class WindowsApplication : public ProcessApplication {
 public:
  WindowsApplication() = delete;

  explicit WindowsApplication(ProcessMaster &master);

  ~WindowsApplication() override;

 protected:
  [[nodiscard]] bool StartProcess() override;

  [[nodiscard]] bool IsRunning() const override;

  void StopProcess() override;

 private:
  PROCESS_INFORMATION process_info_;

  static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

  void WaitOnStart(DWORD dMaxDelay) const;

  DWORD PriorityFlag() const;
};

} // util::supervise
#endif

