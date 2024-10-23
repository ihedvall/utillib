/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <limits>
#include <string>

#include "util/timestamp.h"
using namespace util::time;
using namespace std::chrono;
using namespace std::chrono_literals;
namespace {

const std::string kDateTime = "YYYY-MM-DD hh:mm:ss";           // NOLINT
const std::string kTimestampMs("YYYY-MM-DD hh:mm:ss.xxx");     // NOLINT
const std::string kTimestampUs("YYYY-MM-DD hh:mm:ss.xxxyyy");  // NOLINT
const std::string kMdf3Date("DD:MM:YYYY");                     // NOLINT
const std::string kMdf3Time("HH:MM:SS");                       // NOLINT

}  // namespace
namespace util::test {

TEST(Timestamp, TestEpoch) {  // NOLINT
  {
    time_t time_t_epoch = 0;
    struct tm *bt = gmtime(&time_t_epoch);
    std::ostringstream date_time;
    date_time << std::put_time(bt, "%Y-%m-%d %H:%M:%S");
    std::cout << "Time_t Epoch: " << date_time.str() << std::endl;
  }

  {
    const auto timestamp_1970 = SystemClock::from_time_t(0);
    const auto offset_epoch = timestamp_1970.time_since_epoch();
    std::cout << "System Clock Epoch [ns]: " << offset_epoch.count()
              << std::endl;
  }

  {
    const time_t max_time = std::numeric_limits<time_t>::max() / 300'000'000;
    struct tm *bt = gmtime(&max_time);
    std::ostringstream date_time;
    date_time << std::put_time(bt, "%Y-%m-%d %H:%M:%S");
    std::cout << "Max Time_t: " << date_time.str() << std::endl;
  }

  {
    const uint64_t max = std::numeric_limits<uint64_t>::max();
    const auto max_time = static_cast<time_t>(max / 1'000'000'000);
    struct tm *bt = gmtime(&max_time);
    std::ostringstream date_time;
    date_time << std::put_time(bt, "%Y-%m-%d %H:%M:%S");
    std::cout << "Max (ns) Time: " << date_time.str() << std::endl;
  }
}

TEST(Timestamp, GetCurrentDateTime)  // NOLINT
{
  const auto date_time = GetLocalDateTime();
  EXPECT_EQ(date_time.size(), kDateTime.size()) << date_time;
}

TEST(Timestamp, GetCurrentTimestampWithMs)  // NOLINT
{
  const auto timestamp = GetLocalTimestampWithMs();
  EXPECT_EQ(timestamp.size(), kTimestampMs.size()) << timestamp;
}

TEST(Timestamp, GetCurrentTimestampWithUs)  // NOLINT
{
  const auto timestamp = GetLocalTimestampWithUs();
  EXPECT_EQ(timestamp.size(), kTimestampUs.size()) << timestamp;
}

TEST(Timestamp, NsToLocalIsoTime)  // NOLINT
{
  const auto iso_time1 = NsToLocalIsoTime(0);
  EXPECT_FALSE(iso_time1.empty()) << iso_time1;

  const auto iso_time2 = NsToLocalIsoTime(TimeStampToNs());
  EXPECT_FALSE(iso_time2.empty()) << iso_time2;
  std::cout << "Local ISO Time: " << iso_time2 << std::endl;
}

TEST(Timestamp, NsToIsoTime)  // NOLINT
{
  const auto iso_time1 = NsToIsoTime(0);
  EXPECT_FALSE(iso_time1.empty()) << iso_time1;
  std::cout << "ISO Time: " << iso_time1 << std::endl;

  const auto iso_time2 = NsToIsoTime(TimeStampToNs());
  EXPECT_FALSE(iso_time2.empty()) << iso_time2;
  std::cout << "ISO Time: " << iso_time2 << std::endl;

  const auto iso_time3 = NsToIsoTime(TimeStampToNs(), 1);
  EXPECT_FALSE(iso_time3.empty()) << iso_time3;
  std::cout << "ISO Time: " << iso_time3 << std::endl;

  const auto iso_time4 = NsToIsoTime(TimeStampToNs(), 2);
  EXPECT_FALSE(iso_time4.empty()) << iso_time4;
  std::cout << "ISO Time: " << iso_time4 << std::endl;

  const auto iso_time5 = NsToIsoTime(TimeStampToNs(), 3);
  EXPECT_FALSE(iso_time5.empty()) << iso_time5;
  std::cout << "ISO Time: " << iso_time5 << std::endl;
}

TEST(Timestamp, IsoTimeToNs)  // NOLINT
{
  const uint64_t time1 = 0;
  const auto iso_time1 = NsToIsoTime(time1);
  EXPECT_EQ(time1, IsoTimeToNs(iso_time1));

  const uint64_t time2 = TimeStampToNs();
  const auto iso_time2 = NsToIsoTime(time2, 1);
  EXPECT_EQ(time2 / 1'000'000, IsoTimeToNs(iso_time2) / 1'000'000);

  const uint64_t time3 = TimeStampToNs();
  const auto iso_time3 = NsToIsoTime(time3, 2);
  EXPECT_EQ(time3 / 1'000, IsoTimeToNs(iso_time3) / 1'000);

  const uint64_t time4 = TimeStampToNs();
  const auto iso_time4 = NsToIsoTime(time4, 3);
  EXPECT_EQ(time4, IsoTimeToNs(iso_time4));

  constexpr std::string_view format1 = "2024-01-01T01:02:03Z";
  const uint64_t ref_time1 = IsoTimeToNs(format1.data());
  EXPECT_GT(ref_time1, 0);

  // Now test the same time strings with different format.
  constexpr std::string_view format2 = "2024-01-01 01:02:03";
  const uint64_t ref_time2 = IsoTimeToNs(format2.data());
  EXPECT_EQ(ref_time2, ref_time1);

  constexpr std::string_view format3 = "2024-01-01 01:02:03.000";
  const uint64_t ref_time3 = IsoTimeToNs(format3.data());
  EXPECT_EQ(ref_time3, ref_time1);

  constexpr std::string_view format4 = "2024-01-01T01:02:03.000Z";
  const uint64_t ref_time4 = IsoTimeToNs(format4.data());
  EXPECT_EQ(ref_time4, ref_time1);

  constexpr std::string_view format5 = "2024-01-01T01:02:03.000+00:00Z";
  const uint64_t ref_time5 = IsoTimeToNs(format5.data());
  EXPECT_EQ(ref_time5, ref_time1);

  // Test with some time < 1970
  constexpr std::string_view formatInvalid = "1900-01-01T01:02:03.000Z";
  const uint64_t invalid_time = IsoTimeToNs(formatInvalid.data());
  EXPECT_EQ(invalid_time, 0);
}

TEST(Timestamp, TimeStampToNs)  // NOLINT
{
  {
    const auto timestamp = SystemClock::now();
    const auto system_time = SystemClock::to_time_t(timestamp);
    auto ns_1970 = TimeStampToNs(timestamp);
    std::cout << "TimeStampToNs (now): " << ns_1970 << std::endl;
    EXPECT_EQ(system_time, static_cast<time_t>(ns_1970 / 1'000'000'000));
  }
  {
    const auto timestamp = SystemClock::from_time_t(0);
    const auto system_time = SystemClock::to_time_t(timestamp);
    auto ns_1970 = TimeStampToNs(timestamp);
    std::cout << "TimeStampToNs (epoch): " << ns_1970 << std::endl;
    EXPECT_EQ(system_time, static_cast<time_t>(ns_1970 / 1'000'000'000));
  }
}

}  // namespace util::test