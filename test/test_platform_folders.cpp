/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>

#include <sago/platform_folders.h>
#include <util/systeminfo.h>

using namespace util::supervise;

TEST(PlatformFolders, Test_Folders) {

  std::cout << "Config: " << sago::getConfigHome() << "\n";
  std::cout << "Data: " << sago::getDataHome() << "\n";
  std::cout << "State: " << sago::getStateDir() << "\n";
  std::cout << "Cache: " << sago::getCacheDir() << "\n";
  std::cout << "Documents: " << sago::getDocumentsFolder() << "\n";
  std::cout << "Desktop: " << sago::getDesktopFolder() << "\n";
  std::cout << "Pictures: " << sago::getPicturesFolder() << "\n";
  std::cout << "Music: " << sago::getMusicFolder() << "\n";
  std::cout << "Video: " << sago::getVideoFolder() << "\n";
  std::cout << "Download: " << sago::getDownloadFolder() << "\n";
  std::cout << "Save Games 1: " << sago::getSaveGamesFolder1() << "\n";
  std::cout << "Save Games 2: " << sago::getSaveGamesFolder2() << "\n";
}

TEST(PlatformFolders, Test_SystemInfo) {
  SystemInfo system("TestUtil");
  EXPECT_EQ(system.ProgramName(), "TestUtil");
  std::cout << "Data Path: " << system.DataPath() << std::endl;
  std::cout << "Config Path: " << system.ConfigPath() << std::endl;
  std::cout << "Cache Path: " << system.CachePath() << std::endl;
  std::cout << "State Path: " << system.StatePath() << std::endl;
  std::cout << "Program Data Path: " << system.ProgramDataPath() << std::endl;

  EXPECT_FALSE(system.ProgramName().empty());
  EXPECT_FALSE(system.DataPath().empty());
  EXPECT_FALSE(system.ConfigPath().empty());
  EXPECT_FALSE(system.CachePath().empty());
  EXPECT_FALSE(system.ProgramDataPath().empty());
}

TEST(PlatformFolders, Test_NoProgram) {
  SystemInfo system;
  EXPECT_EQ(system.ProgramName(), "");
  std::cout << "Data Path: " << system.DataPath() << std::endl;
  std::cout << "Config Path: " << system.ConfigPath() << std::endl;
  std::cout << "Cache Path: " << system.CachePath() << std::endl;
  std::cout << "State Path: " << system.StatePath() << std::endl;
  std::cout << "Program Data Path: " << system.ProgramDataPath() << std::endl;

  EXPECT_TRUE(system.ProgramName().empty());
  EXPECT_FALSE(system.DataPath().empty());
  EXPECT_FALSE(system.ConfigPath().empty());
  EXPECT_FALSE(system.CachePath().empty());
  EXPECT_FALSE(system.ProgramDataPath().empty());
}