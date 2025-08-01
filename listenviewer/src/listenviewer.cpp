/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>

#define BOOST_LOCALE_HIDE_AUTO_PTR

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/locale.hpp>

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/docview.h>

#include <wx/config.h>
#include <wx/utils.h>
#include <wx/cmdline.h>

#include "util/logconfig.h"
#include "util/logstream.h"

#include "listenviewerid.h"
#include "listenviewer.h"
#include "mainframe.h"


using namespace util::log;

namespace {
boost::asio::io_context kIoContext;
} // end namespace

wxIMPLEMENT_APP(util::log::gui::ListenViewer); // NOLINT

namespace util::log::gui {

wxBEGIN_EVENT_TABLE(ListenViewer, wxApp) // NOLINT
    EVT_UPDATE_UI(kIdOpenLogFile, ListenViewer::OnUpdateOpenLogFile)
    EVT_MENU(kIdOpenLogFile, ListenViewer::OnOpenLogFile)
wxEND_EVENT_TABLE()

bool ListenViewer::OnInit() {
  SetAppearance(Appearance::System);
  if (!wxApp::OnInit()) {
    return false;
  }
  // Setup correct localization when formatting date and times
  boost::locale::generator gen;
  std::locale::global(gen(""));

  // Setup system basic configuration
  SetVendorDisplayName("UtilLib");
  SetVendorName("UtilLib");
  SetAppName("ListenViewer");
  SetAppDisplayName("Listen Viewer");

  auto *app_config = wxConfig::Get();
  wxPoint start_pos;
  app_config->Read("/MainWin/X", &start_pos.x, wxDefaultPosition.x);
  app_config->Read("/MainWin/Y", &start_pos.y, wxDefaultPosition.x);
  wxSize start_size;
  app_config->Read("/MainWin/XWidth", &start_size.x, 1200);
  app_config->Read("/MainWin/YWidth", &start_size.y, 800);

  bool maximized = false;
  app_config->Read("/MainWin/Max", &maximized, maximized);

  wxInitAllImageHandlers();

  // Set up the log file.
  // The log file is stored in %ProgramData%/utillib/log//listenviewer.log
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("utillib/log");
  log_config.BaseName("listenviewer");
  log_config.CreateDefaultLogger();
  LOG_INFO() << "Log File created. Path: " << log_config.GetLogFile();

  notepad_ = util::log::FindNotepad();

  auto *frame = new MainFrame(GetAppDisplayName(), start_pos, start_size, maximized);
  frame->Show(true);
  SetTopWindow(frame);
  return true;
}

int ListenViewer::OnExit() {
  LOG_INFO() << "Closing application";
  auto *doc_manager = wxDocManager::GetDocumentManager();
  delete doc_manager;

  auto &log_config = LogConfig::Instance();
  log_config.DeleteLogChain();
  return wxApp::OnExit();
}

void ListenViewer::OnOpenLogFile(wxCommandEvent &event) { //NOLINT
  auto &log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  OpenFile(logfile);
}

void ListenViewer::OnUpdateOpenLogFile(wxUpdateUIEvent &event) {
  if (notepad_.empty()) {
    event.Enable(false);
    return;
  }

  auto &log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  try {
    std::filesystem::path p(logfile);
    const bool exist = std::filesystem::exists(p);
    event.Enable(exist);
  } catch (const std::exception &) {
    event.Enable(false);
  }
}


void ListenViewer::OpenFile(const std::string &filename) const {
  if (!notepad_.empty()) {
    boost::process::process proc(kIoContext, notepad_, {filename});
    proc.detach();
  }
}

} // end namespace


