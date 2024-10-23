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

/** \brief Converts a timestamp (UTC) to a local date and time string in the YYYY-MM-DD hh:mm:ss format.
 *
 * The function converts a chrono system clock timestamp to a common
 * ISO type of local timestamp (YYYY-MM-DD hh:mm:ss). This is a common
 * format in log files that should be read by users. Computers should
 * always use UTC time zone.
 *
 * @param [in] timestamp System clock timestamp (UTC)
 * @return Returns a date and time string.
 */
std::string GetLocalDateTime(TimeStamp timestamp = SystemClock::now());

/** \brief Converts a timestamp (UTC) to a local dat and time string in
 * the YYYY-MM-DD hh:mm:ss.ms format.
 *
 * The function converts a UTC system clock timestamp to a local date and time
 * string including milliseconds. It uses the ISO related format
 * YYYY-MM-DD hh:mm:ss.us. This format commonly used in log files.
 *
 * @param [in] timestamp System clock timestamp (UTC)
 * @return Returns a date and time string with milliseconds.
 */
std::string GetLocalTimestampWithMs(TimeStamp timestamp = SystemClock::now());

/** \brief Converts a UTC timestamp to a local date and time string in the YYYY-MM-DD hh:mm:ss.us
 *
 * The function converts a system clock timestamp (UTC) to a date and time string including microseconds.
 * It uses the YYYY-MM-DD hh:mm:ss.us format.
 *
 * @param [in] timestamp System clock timestamp (UTC)
 * @return Returns a date and time string with microseconds.
 */
std::string GetLocalTimestampWithUs(TimeStamp timestamp = SystemClock::now());

/** \brief Converts a nanosecond since 1970 timestamp to a local ISO date and time string.
 *
 * Converts a nanoseconds since 1970 timestamp (UTC) value. This is a so-called Unix timestamp.
 * The nanosecond timestamp is typical used when computers deal with timestamps.
 * Note that the milliseconds text may be missing in case there is no milliseconds (.000).
 *
 * @param [in] ns_since_1970 Nanosecond since 1970-01-01.
 * @return Return a date and time string in local format YYYY-MM-DD hh:mm:ss.ms
 */
std::string NsToLocalIsoTime(uint64_t ns_since_1970);

/** \brief Converts a nanosecond since 1970 timestamp to a UTC ISO date and time string.
 *
 * The function converts a nanoseconds since 1970 (Unix time) value to a ISO standard YYYY-MM-DDThh:mm:ss.xxxZ.
 * It always converts to a UTC date and time string. The string can be used to stored the timestamp in both user and
 * computer readable format.
 * The user can define the resolution (format).
 * The format is typical used in XML files and in databases that doesn't have date and time formats (SQLite).
 *
 * @param [in] ns_since_1970 Nanosecond since 1970
 * @param [in] format 0: Include seconds, 1: Include ms, 2: include
 * micro-seconds, 3: Include ns
 * @return Return a date and time string in format
 * YYYY-MM-DDThh:mm:ss.sssssssssZ
 */
std::string NsToIsoTime(uint64_t ns_since_1970, int format = 0);

/** \brief Converts an ISO UTC string (YYYY-MM-DD(T)hh:mm:ss to nanosecond since 1970.
 *
 * Converts an ISO date and time string to nano-seconds since 1970.
 * Note that the function also handle ODS date strings 'YYYYMMDDhhmmssxxxxxxxxxx strings.
 * The date and string doesn't need to have more than years in the input string.
 *
 * @param [in] iso_time Time stamp string YYYY-MM-DDThh:mm:ss.sssssssssZ
 * @param [in] local_time Set to true if the ISO time uses local time.
 * @return Returns nanoseconds since 1970
 */
uint64_t IsoTimeToNs(const std::string &iso_time, bool local_time = false);

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
uint64_t OdsDateToNs(const std::string &ods_date);

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
