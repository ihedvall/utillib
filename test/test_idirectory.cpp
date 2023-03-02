/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>
#include <util/idirectory.h>
#include <util/timestamp.h>

#include <any>
#include <filesystem>

using namespace util::log;

namespace {

constexpr std::string_view kRootDir1 = "k:/test/dbc";
constexpr std::string_view kRootDir2 = "x:/test/dbc";
const auto kSkipTest = !std::filesystem::exists(kRootDir1.data());

void PrintDir(const util::log::IDirectory& dir) {  // NOLINT
  const auto& file_list = dir.Files();
  for (const auto& itr_file : file_list) {
    const auto* file = itr_file.second.get();
    if (file == nullptr) continue;
    std::cout << dir.Level() << ":" << dir.Name() << "-" << file->Name()
              << std::endl;
  }

  const auto& dir_list = dir.Directories();
  for (const auto& itr_dir : dir_list) {
    const auto* sub_dir = itr_dir.second.get();
    if (sub_dir == nullptr) continue;
    PrintDir(*sub_dir);
  }
}

}  // namespace

namespace util::test {
TEST(IDirectory, NormalUse) {
  IDirectory dir;
  dir.ParentDir(kRootDir1.data());
  if (kSkipTest) {
    GTEST_SKIP();
  }
  dir.ScanDirectory();

  const auto& file_list = dir.Files();
  EXPECT_FALSE(file_list.empty());

  const auto& dir_list = dir.Directories();
  EXPECT_FALSE(dir_list.empty());

  PrintDir(dir);
}

TEST(IDirectory, IncludeFilter) {
  IDirectory dir;
  dir.ParentDir(kRootDir1.data());
  dir.StringToIncludeList("*.ini");

  const auto& filter_list = dir.IncludeList();
  EXPECT_EQ(filter_list.size(), 1);

  if (kSkipTest) {
    GTEST_SKIP();
  }
  dir.ScanDirectory();

  const auto& file_list = dir.Files();
  EXPECT_FALSE(file_list.empty());

  const auto& dir_list = dir.Directories();
  EXPECT_TRUE(dir_list.empty());

  PrintDir(dir);
}

TEST(IDirectory, ExcludeFilter) {
  IDirectory dir;
  dir.ParentDir(kRootDir1.data());
  dir.StringToExcludeList("*.ini;net");

  const auto& filter_list = dir.ExcludeList();
  EXPECT_EQ(filter_list.size(), 2);

  if (kSkipTest) {
    GTEST_SKIP();
  }
  dir.ScanDirectory();

  const auto& file_list = dir.Files();
  EXPECT_FALSE(file_list.empty());

  PrintDir(dir);
}

TEST(IDirectory, FileProperties) {
  IDirectory dir;
  dir.ParentDir(kRootDir1.data());
  if (kSkipTest) {
    GTEST_SKIP();
  }
  dir.ScanDirectory();
  const auto& file_list = dir.Files();
  for (const auto& itr : file_list) {
    const auto* file = itr.second.get();
    EXPECT_TRUE(file != nullptr);

    const auto name = file->Name();
    EXPECT_FALSE(name.empty());

    const auto& filename = file->Filename();
    EXPECT_FALSE(filename.empty());

    const auto& checksum = file->Checksum();
    EXPECT_TRUE(checksum.empty());

    const auto modified = file->Modified();
    EXPECT_GT(modified, 0);

    const auto file_size = file->FileSize();
    EXPECT_GT(file_size, 0);

    std::cout << "Name: " << name << " (" << filename << ")";
    std::cout << " Time: " << time::NsToLocalIsoTime(modified);
    std::cout << " Size [bytes]: " << file_size << std::endl;
  }
}

TEST(IDirectory, DirectoryProperties) {
  IDirectory dir(kRootDir1.data());

  if (kSkipTest) {
    GTEST_SKIP();
  }
  EXPECT_TRUE(dir.ScanDirectory());
  const auto& dir_list = dir.Directories();
  for (const auto& itr : dir_list) {
    const auto* sub = itr.second.get();
    EXPECT_TRUE(sub != nullptr);

    const auto name = sub->Name();
    EXPECT_FALSE(name.empty());

    const auto& sub_path = sub->ParentDir();
    EXPECT_FALSE(sub_path.empty());

    const auto modified = sub->Modified();
    EXPECT_GT(modified, 0);

    std::cout << "Name: " << name << " (" << sub_path << ")";
    std::cout << " Time: " << time::NsToLocalIsoTime(modified) << std::endl;
  }
}

TEST(IDirectory, InvalidRootDir) {
  IDirectory dir;
  dir.ParentDir(kRootDir2.data());
  EXPECT_FALSE(dir.ScanDirectory());

  const auto& last_error = dir.LastError();
  EXPECT_FALSE(last_error.empty());
  std::cout << "Last Error: " << last_error << std::endl;

  const auto& file_list = dir.Files();
  EXPECT_TRUE(file_list.empty());

  const auto& dir_list = dir.Directories();
  EXPECT_TRUE(dir_list.empty());
}

TEST(IDirectory, AnyTest) {
  auto any_dir1 = std::make_any<IDirectory>();
  auto* dir1 = std::any_cast<IDirectory>(&any_dir1);
  EXPECT_TRUE(dir1 != nullptr);

  IDirectory dir2;
  auto any_dir2 = std::make_any<IDirectory>(dir2);
}

}  // end namespace util::test