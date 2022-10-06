/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file timestamp.h
 * \brief Contains various conversion between different time formats.
 *
 * The central time is an uint64_t UTC nanoseconds since 1970-01-01. This is an
 * extension of the ols time_t that is seconds since 1970-01-01. In general the
 * std::chrono::system_clock is used as reference.
 *
 * The nanosecond timestamp is used in communications and a general timestamp.
 * This due to the fact that different system used different epoch for system
 * clock. Note most system uses 1970-01-01 as epoch and is called POSIX time or
 * UNIX time.
 *
 * \note
 * The std::chrono library have many new functions in C++20. Unfortunately not
 * many compiler have implemented these function so most most of the
 * implementation uses C++17 functionality.
 */
#pragma once
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace util::time {
/** \typedef SystemClock
 * \brief Convenience type definition for std::chrono::system_clock;
 */
using SystemClock = std::chrono::system_clock;

/** \typedef TimeStamp
 * \brief Convenience type definition for
 * std::chrono::time_point<std::chrono::system_time>;
 */
using TimeStamp = std::chrono::time_point<SystemClock>;

/** \brief returns a ISO date and time string.
 *
 * The function is intended to be used for time stamp in log files.
 *
 * @param [in] timestamp System clock timestamp
 * @return Returns a date and time string.
 */
std::string GetLocalDateTime(TimeStamp timestamp = SystemClock::now());

/** \brief returns a ISO date and time string.
 *
 * The function is intended to be used for time stamp in log files.
 *
 * @param [in] timestamp System clock timestamp
 * @return Returns a date and time string.
 */
std::string GetLocalTimestampWithMs(TimeStamp timestamp = SystemClock::now());

/** \brief returns a ISO date and time string.
 *
 * The function is intended to be used for time stamp in log files.
 *
 * @param [in] timestamp System clock timestamp
 * @return Returns a date and time string.
 */
std::string GetLocalTimestampWithUs(TimeStamp timestamp = SystemClock::now());

/** \brief Converts a nanosecond since 1970 to a local ISO date and time string.
 *
 * @param [in] ns_since_1970 Nanosecond since 1970
 * @return Return a date and time string in format YYYY-MM-DD hh:mm:ss.ms
 */
std::string NsToLocalIsoTime(uint64_t ns_since_1970);

/** \brief Converts a nanosecond since 1970 to a UTC ISO date and time string.
 *
 * @param [in] ns_since_1970 Nanosecond since 1970
 * @param [in] format 0: Include seconds, 1: Include ms, 2: include
 * micro-seconds, 3: Include ns
 * @return Return a date and time string in format
 * YYYY-MM-DDThh:mm:ss.sssssssssZ
 */
std::string NsToIsoTime(uint64_t ns_since_1970, int format = 0);

/** \brief Converts an ISO UTC string to nanosecond since 1970.
 *
 * Converts an ISO Date/Time string to nano-seconds since 1970. Note that the
 * function also handle ODS date strings 'YYYYMMDD..'.
 * @param [in] iso_time Time stamp string YYYY-MM-DDThh:mm:ss.sssssssssZ
 * @param [in] local_time Set to true if the ISO time uses local time.
 * @return Returns nanoseconds since 1970
 */
uint64_t IsoTimeToNs(const std::string& iso_time, bool local_time = false);

/** \brief Converts system clock to ns since midnight 1970.
 *
 *
 * @param timestamp System clock time stamp
 * @return Nanoseconds since midnight 1970
 */
uint64_t TimeStampToNs(TimeStamp timestamp = SystemClock::now());

/** \brief Formats a date string according to the local date format.
 *
 * Formats a date string according to the local date format.
 * @param ns_since_1970 Number of nanoseconds since 1970
 * @return Localized date string
 */
std::string NsToLocalDate(uint64_t ns_since_1970);

/** \brief Formats a time string according to the local time format.
 *
 * Formats a time string with optional ms..ns. The output format is according to
 * the local time format.
 * @param ns_since_1970 Number of nanoseconds since 1970
 * @param format 0 = Show only seconds, 1 = Show ms, 2 = Show microseconds, 3 =
 * Show ns
 * @return Localized time string
 */
std::string NsToLocalTime(uint64_t ns_since_1970, int format = 0);

/** \brief Converts an ODS date and time string to nanoseconds since 1970.
 *
 * Converts an ODS date and time string to nanoseconds since 1970. The ODS
 * DT_DATE string have the format 'YYYYMMDDhhmmsslllcccnnn'. Note that the
 * function also handle ISO time formats as 'YYYY-MM-DD hh:mm:ss(.*)'
 * @param ods_date ODS date string
 * @return Nanoseconds since 1970
 */
uint64_t OdsDateToNs(const std::string& ods_date);

/** \brief Converts from std::filesystem file_clock to ns since 1970.
 *
 * Converts to a file time to nano-seconds since 1970. Note that this function
 * actually hides a problem between MSC and GCC implementation.
 *
 * @param time Time point of a file clock
 * @return Nano-seconds since 1970
 */
uint64_t FileTimeToNs(std::filesystem::file_time_type time);

}  // namespace util::time
