/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <chrono>
#include <filesystem>


#include <gtest/gtest.h>
#include "util/isuperviseapplication.h"
#include "util/utilfactory.h"
#include "util/logconfig.h"

using namespace util;
using namespace util::supervise;
using namespace util::log;
using namespace std::chrono_literals;
using namespace std::filesystem;

TEST(Supervise, Test_Properties) {
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();


  auto master = UtilFactory::CreateSuperviseMaster(
      TypeOfSuperviseMaster::SuperviseMasterType);
  ASSERT_TRUE(master);

  master->ProgramName("supervise_test");
  EXPECT_EQ("supervise_test", master->ProgramName());

  master->ConfigFile("supervise_test.xml");
  EXPECT_EQ("supervise_test.xml", master->ConfigFile());

  master->EnableMqtt(true);
  EXPECT_TRUE(master->EnableMqtt());
  master->EnableMqtt(false);
  EXPECT_FALSE(master->EnableMqtt());

  auto *app = master->CreateApplication();
  ASSERT_TRUE(app);

  app->Name("Notepad");
  EXPECT_EQ("Notepad", app->Name());

  app->Path("notepad.exe");
  EXPECT_EQ("notepad.exe", app->Path());

  app->Arguments("hello.txt");
  EXPECT_EQ("hello.txt", app->Arguments());

  constexpr std::array<ApplicationStartup, 3> startups = {
      ApplicationStartup::Manual,
      ApplicationStartup::Once,
      ApplicationStartup::Automatic
  };
  for (const auto startup : startups) {
    app->Startup(startup);
    EXPECT_EQ(startup, app->Startup());
  }

  constexpr std::array<ApplicationPriority, 6> priorities = {
      ApplicationPriority::Realtime,
      ApplicationPriority::High,
      ApplicationPriority::AboveNormal,
      ApplicationPriority::Normal,
      ApplicationPriority::BelowNormal,
      ApplicationPriority::Idle
  };
  for (const auto priority : priorities) {
    app->Priority(priority);
    EXPECT_EQ(priority, app->Priority());
  }
  app->ShowWindow(true);
  EXPECT_TRUE(app->ShowWindow());
  app->ShowWindow(false);
  EXPECT_FALSE(app->ShowWindow());

  app->StartupDelay(2);
  EXPECT_EQ(app->StartupDelay(), 2);

  app->MaxRestarts(15);
  EXPECT_EQ(app->MaxRestarts(), 15);

  app->InService(true);
  EXPECT_TRUE(app->InService());
  app->InService(false);
  EXPECT_FALSE(app->InService());

  EXPECT_EQ(app->NofRestarts(), 0);
  EXPECT_EQ(app->Operable(), false);
  EXPECT_EQ(app->State(), ApplicationState::Stopped);

  master->SaveConfig();

  auto copy = UtilFactory::CreateSuperviseMaster(
      TypeOfSuperviseMaster::SuperviseMasterType);
  ASSERT_TRUE(copy);

  copy->ProgramName("supervise_test");
  EXPECT_EQ("supervise_test", copy->ProgramName());

  copy->ConfigFile("supervise_test.xml");
  EXPECT_EQ("supervise_test.xml", copy->ConfigFile());

  copy->ReadConfig();
  EXPECT_FALSE(copy->EnableMqtt());

  ASSERT_EQ(copy->Applications().size(), 1);

  const auto &app1 = copy->Applications().front();
  ASSERT_TRUE(app1);

  EXPECT_EQ("Notepad", app1->Name());
  EXPECT_EQ("notepad.exe", app1->Path());
  EXPECT_EQ("hello.txt", app1->Arguments());
  EXPECT_EQ(ApplicationStartup::Automatic, app1->Startup());
  EXPECT_EQ(ApplicationPriority::Idle, app1->Priority());
  EXPECT_FALSE(app1->ShowWindow());
  EXPECT_EQ(app1->StartupDelay(), 2);
  EXPECT_EQ(app1->MaxRestarts(), 15);
  EXPECT_FALSE(app1->InService());

  EXPECT_EQ(app1->NofRestarts(), 0);
  EXPECT_EQ(app1->Operable(), false);
  EXPECT_EQ(app1->State(), ApplicationState::Stopped);

  log_config.DeleteLogChain();
}

TEST(Supervise, Test_Notepad) {
#ifndef _WIN32
  GTEST_SKIP_("Test only possible on Windows");
#endif
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  auto master = UtilFactory::CreateSuperviseMaster(
      TypeOfSuperviseMaster::ProcessMasterType);
  ASSERT_TRUE(master);

  master->ProgramName("supervise_test");
  EXPECT_EQ("supervise_test", master->ProgramName());

  auto *app = master->CreateApplication();
  ASSERT_TRUE(app);

  app->Name("Notepad");
  app->Path("notepad.exe");
  app->Startup(ApplicationStartup::Manual);
  app->ShowWindow(true);

  master->Start();
  for (size_t times = 0; times < 20; times++) {
    if (times == 3) {
      app->InService(true);
    }
    std::this_thread::sleep_for(100ms);
  }
  master->Stop();

  log_config.DeleteLogChain();
}

TEST(Supervise, Test_Listend) {
  constexpr std::string_view test_file =
      "d:/projects/utillib/cmake-build-release/listend/listend.exe";
  bool exist;
  try {
    exist = exists(test_file);
  } catch (const std::exception &) {
    exist = false;
  }
  if (!exist) {
    GTEST_SKIP_("Test file missing.");
  }
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  auto master = UtilFactory::CreateSuperviseMaster(
      TypeOfSuperviseMaster::ProcessMasterType);
  ASSERT_TRUE(master);

  master->ProgramName("supervise_test");
  EXPECT_EQ("supervise_test", master->ProgramName());

  auto *app = master->CreateApplication();
  ASSERT_TRUE(app);

  app->Name("Listend");
  app->Path(std::string(test_file));
  app->Startup(ApplicationStartup::Automatic);
  app->ShowWindow(true);

  master->Start();
  for (size_t times = 0; times < 30; times++) {
    std::this_thread::sleep_for(100ms);
  }
  master->Stop();

  log_config.DeleteLogChain();
}

TEST(Supervise, Test_ListenWindow) {
  constexpr std::string_view test_file =
      "d:/projects/utillib/cmake-build-release/listenviewer/listenviewer.exe";
  bool exist;
  try {
    exist = exists(test_file);
  } catch (const std::exception &) {
    exist = false;
  }
  if (!exist) {
    GTEST_SKIP_("Test file missing.");
  }
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  auto master = UtilFactory::CreateSuperviseMaster(
      TypeOfSuperviseMaster::ProcessMasterType);
  ASSERT_TRUE(master);

  master->ProgramName("supervise_test");
  EXPECT_EQ("supervise_test", master->ProgramName());

  auto *app = master->CreateApplication();
  ASSERT_TRUE(app);

  app->Name("ListenWindow");
  app->Path(std::string(test_file));
  app->Startup(ApplicationStartup::Manual);
  app->ShowWindow(true);

  master->Start();
  for (size_t times = 0; times < 5; times++) {
    if (app->InService()) {
      app->InService(false);
    } else {
      app->InService(true);
    }
    std::this_thread::sleep_for(2s);
  }
  master->Stop();

  log_config.DeleteLogChain();
}
