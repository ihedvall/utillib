/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>

#include "util/consoleapp.h"

namespace {
class TestApp : public util::ConsoleApp {
 public:
  [[nodiscard]] size_t PollCount() const { return poll_count_; }
  void OnPoll() override {
    ++poll_count_;
    if (poll_count_ > 10) {
      StopMain();
    }
  }

 private:
  size_t poll_count_ = 0;
};

}  // namespace

namespace util::test {

TEST(ConsoleApp, TestProperties) {
  TestApp app;
  EXPECT_FALSE(app.ShallStop());
  TestApp::StopMain();  // We do not want the run function to run.
  EXPECT_TRUE(app.ShallStop());

  // Simulate that the main function uses char pointers not const char pointers
  std::vector<std::string> args;
  args.emplace_back("Program/Path/ExeName.exe");
  args.emplace_back("Arg0");
  args.emplace_back("Arg1");
  int argc = static_cast<int>(args.size());

  char* argv[3] = {args[0].data(), args[1].data(), args[2].data()};
  app.MainFunc(argc, argv);  // This should normally hang here

  EXPECT_EQ(app.AppFullName(), args[0]);
  EXPECT_EQ(app.AppName(), "ExeName");
  EXPECT_EQ(app.AppPath(), "Program/Path");

  const auto arg_list = app.Args();
  ASSERT_EQ(arg_list.size(), 2);
  EXPECT_EQ(arg_list[0], "Arg0");
  EXPECT_EQ(arg_list[1], "Arg1");

  app.ExitCode(EXIT_SUCCESS);
  EXPECT_EQ(app.ExitCode(), EXIT_SUCCESS);
  app.ExitCode(EXIT_FAILURE);
  EXPECT_EQ(app.ExitCode(), EXIT_FAILURE);

  EXPECT_EQ(app.PollDelay(), 100);
  app.PollDelay(60'000);
  EXPECT_EQ(app.PollDelay(), 60'000);
}

TEST(ConsoleApp, TestRun) {
  TestApp app;
  std::vector<std::string> args;
  args.emplace_back("Program/Path/ExeName.exe");
  int argc = static_cast<int>(args.size());
  char* argv[1] = {args[0].data()};
  app.MainFunc(argc, argv);
  EXPECT_EQ(app.PollCount(), 11);
}

}  // namespace util::test