/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#ifdef _WIN32

#include <sstream>
#include <string>
#include <cstring>
#include <chrono>

#include "windowsapplication.h"

#include "util/logstream.h"

using namespace util::log;
using namespace std::chrono_literals;
namespace {
std::string GetWindowsError() {
  LPVOID lpMsgBuf = nullptr;
  const DWORD nResult = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, nullptr);
  if (nResult == 0) {
    return {};
  }
  std::ostringstream temp;
  temp << static_cast<const char *>(lpMsgBuf);
  LocalFree(lpMsgBuf);
  return temp.str();
}
}

namespace util::supervise {
WindowsApplication::WindowsApplication(ProcessMaster &master)
    : ProcessApplication(master),
      process_info_{} {

}

WindowsApplication::~WindowsApplication() {
  ProcessApplication::Stop();
}

bool WindowsApplication::StartProcess() {
  std::ostringstream temp;
  temp << "\"" << Path() << "\" " << Arguments();
  std::string command_line = temp.str();
  std::memset(&process_info_, 0, sizeof(process_info_));

  STARTUPINFOA startup_info = {};
  startup_info.cb = sizeof(startup_info);
  if (ShowWindow()) {
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_SHOWDEFAULT;
  } else {
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_HIDE;
  }
  const DWORD creation_flags = PriorityFlag();
  const BOOL ret_val = CreateProcessA(Path().c_str(),
                                      command_line.data(),
                                      nullptr,
                                      nullptr,
                                      FALSE,
                                      creation_flags,
                                      nullptr,
                                      nullptr,
                                      &startup_info,
                                      &process_info_);
  bool start = true;
  if (ret_val == TRUE) {
    if (ShowWindow()) {
      WaitOnStart(1000);
    }
    //        CloseHandle( myProcessInfo.hProcess );
    CloseHandle(process_info_.hThread);
    LOG_TRACE() << "Started Windows application. Name: " << Name();
  } else {
    LOG_ERROR() << "Failed to start the application. Name: " << Name()
                << ", Error: " << GetWindowsError();
    start = false;
  }
  return start;
}

bool WindowsApplication::IsRunning() const {
  if (process_info_.hProcess == nullptr) {
    return false;
  }
  const DWORD nResult = ::WaitForSingleObject(process_info_.hProcess, 0);
  return nResult == WAIT_TIMEOUT;
}

void WindowsApplication::StopProcess() {
  if (process_info_.dwThreadId != 0) {
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
    PostThreadMessageA(process_info_.dwThreadId, WM_CLOSE, 0, 0);
    PostThreadMessageA(process_info_.dwThreadId, WM_QUIT, 0, 0);
  }
  if (process_info_.hProcess != nullptr) {
    const DWORD nResult = ::WaitForSingleObject(process_info_.hProcess, 4000);
    if (nResult != WAIT_TIMEOUT) {
      ::CloseHandle(process_info_.hProcess);
      process_info_.hProcess = nullptr;
      LOG_TRACE() << "Stopped Windows application. Name: " << Name();
    } else {
      LOG_ERROR()
          << "Couldn't stop application normally. Kill in progress. Name: "
          << Name();
      const BOOL terminate = ::TerminateProcess(process_info_.hProcess, EXIT_FAILURE);
      if (!terminate) {
        LOG_ERROR() << "Terminate failed. Error: " << GetWindowsError();
      }
      ::CloseHandle(process_info_.hProcess);
      memset(&process_info_, 0, sizeof(process_info_));
    }
  }
}

BOOL CALLBACK WindowsApplication::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  const auto *app = reinterpret_cast< WindowsApplication * > ( lParam );
  if (app == nullptr) return FALSE;

  DWORD dProcessId = 0;
  DWORD ret = ::GetWindowThreadProcessId(hwnd, &dProcessId);
  if (ret == 0) {
    LOG_TRACE() << "EnumWindowsProc returned invalid handle";
    return FALSE;
  }

  if (dProcessId == app->process_info_.dwProcessId) {
    for (size_t retry = 0; retry < 10; ++retry) {
      ::PostMessage(hwnd, WM_QUIT, 0, 0);
      if (app->IsRunning()) {
        std::this_thread::sleep_for(100ms);
      } else {
        break;
      }
    }
  }
  return TRUE;
}

void WindowsApplication::WaitOnStart(DWORD dMaxDelay) const {
  const DWORD dResult = ::WaitForInputIdle(process_info_.hProcess, dMaxDelay);
  switch (dResult) {
    case 0: // Normal start of the app.
      return;

    case WAIT_TIMEOUT:LOG_TRACE()
          << "Application didn't start within the timeout. Application: "
          << Name();
      break;

    case WAIT_FAILED: // Most likely it doesn't have a GUI
      LOG_TRACE() << "Application doesn't have a GUI. Application: "
                  << Name();;
      break;

    default:break;
  }
}

DWORD WindowsApplication::PriorityFlag() const {
  switch (Priority()) {
    case ApplicationPriority::Realtime: return REALTIME_PRIORITY_CLASS;
    case ApplicationPriority::High: return HIGH_PRIORITY_CLASS;
    case ApplicationPriority::AboveNormal: return ABOVE_NORMAL_PRIORITY_CLASS;
    case ApplicationPriority::Normal: return NORMAL_PRIORITY_CLASS;
    case ApplicationPriority::BelowNormal: return BELOW_NORMAL_PRIORITY_CLASS;

    case ApplicationPriority::Idle:
    default:break;
  }
  return IDLE_PRIORITY_CLASS;
}


} // util::supervise

#endif
