/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file idirectory.h
 * \brief The file defines an interface against directory file tree structure.
 * The IDirectory class is used in application that keeps track files in one
 * or more directories.
 */
#pragma once
#include <util/ifile.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace util::log {

class IDirectory;

/** \brief Sorted list of directories.
 *
 * A list of sub-directories of the IDirectory class. The key (first) is the
 * name of the directory without path and the value (second) is a unique pointer
 * to the directory object.
 */
using DirectoryList = std::map<std::string, std::unique_ptr<IDirectory>>;

/** \brief Sorted list of files in the directory.
 *
 * Sorted list of files in the directory. Note that the key (first) is the
 * filename without path.
 */
using FileList = std::map<std::string, std::unique_ptr<IFile>>;

/** \brief List of name filters.
 *
 * List of name filters. Files are typical filtered by its extension for
 * example '*.txt' while directories are filtered by its name for example
 * 'obj'.
 */
using FilterList = std::vector<std::string>;

/** \brief The class is simple wrapper around a directory/file tree structure.
 * It is typically used in applications that needs to keep track of file changes
 * in a root directory.
 *
 * The class is intended to be used in application that needs to track changes
 * of directories and files. It has a companion IFile class that is a wrapper
 * around a file.
 *
 * Typical usage is to create an IDirectory and attach it to a root directory.
 * Then call the ScanDirectory() function to get a list of all directories. If
 * not all files are wanted, it is possible to set both include and exclude
 * filters.
 *
 */
class IDirectory {
 public:
  IDirectory() = default;                ///< Default constructor
  IDirectory(const IDirectory& source);  ///< Copy constructor

  /** \brief Constructor by supplying a full path to a directory.
   *
   * Constructor by supplying a full path to a directory.
   * @param filename Full path to the directory.
   */
  explicit IDirectory(const std::string& filename);

  virtual ~IDirectory() = default;  ///< Destructor

  /** \brief Returns the name of the directory without path.
   *
   * Returns the name of the directory without its path a.k.a the stem.
   * @return Name of the directory
   */
  [[nodiscard]] std::string Name() const;

  /** \brief Sets the directory.
   *
   * Sets the directory.
   * @param dir Full path to the parent directory.
   */
  void Directory(const std::string& dir);

  /** \brief Returns the  directory.
   *
   * Return the full path to the parent directory.
   * @return Full path to the parent directory.
   */
  [[nodiscard]] const std::string& Directory() const { return directory_; }

  /** \brief Sets the last modified time.
   *
   * Sets the last modified time. Note that the user should normally not use
   * this function.
   * @param ns1970 Nano-seconds since 1970 (UTC)
   */
  void Modified(uint64_t ns1970) { modified_ = ns1970; }

  /** \brief Return last change time for the directory.
   *
   * Returns the last change time of the directory. This indicate adding or
   * deleting of files.
   * @return Nano-seconds since 1970-01-01 (UTC)
   */
  [[nodiscard]] uint64_t Modified() const { return modified_; }

  /** \brief Set the directory level.
   *
   * Sets the relative directory deep to the root directory.
   * See also the ScanDirectory() function. It is default set to 0.
   * @param level Relative directory deep level.
   */
  void Level(size_t level) { level_ = level; }

  /** \brief Returns the relative directory depth level.
   *
   * Returns the relative directory depth.
   * @return Relative directory depth.
   */
  [[nodiscard]] size_t Level() const { return level_; }

  /** \brief Returns the sub-directory list.
   *
   * Returns a list of sub-directories. Note that it returns a reference to an
   * internal list.
   * @return List of directories.
   */
  [[nodiscard]] const DirectoryList& Directories() const { return dir_list_; }

  /** \brief Returns a list files in the directory.
   *
   * Returns a sorted list of files in the directory.
   * @return
   */
  [[nodiscard]] const FileList& Files() const { return file_list_; }

  /** \brief Returns a list of name filters.
   *
   * Returns a list if name filters. Names that match any of the filter
   * will be excluded from the list of files.
   * @return List of name filters.
   */
  [[nodiscard]] const FilterList& ExcludeList() const { return exclude_list_; }

  /** \brief Returns a list of name filters.
   *
   * Returns a list if name filters. Names that match any of the filter
   * will be excluded from the list of files. The non-const function is
   * used when the filter list should be changed.
   * @return List of name filters.
   */
  [[nodiscard]] FilterList& ExcludeList() { return exclude_list_; }

  /** \brief Converts a list into a text string.
   *
   * Returns the exclude list as a string. The function is used when
   * exporting the list to a configuration file.
   * @return A string describing the list.
   */
  [[nodiscard]] std::string ExcludeListToString() const;

  /** \brief Convert a string into a exclude list.§
   *
   * Convert a string into an exclude list
   * @param text The input string
   */
  void StringToExcludeList(const std::string& text);

  /** \brief Returns a list of name filters.
   *
   * Returns a list if name filters. Names that match any of the filter
   * will be included in the list of files.
   * @return List of name filters.
   */
  [[nodiscard]] const FilterList& IncludeList() const { return include_list_; }

  /** \brief Returns a list of name filters.
   *
   * Returns a list if name filters. Names that match any of the filter
   * will be included in the list of files. The non-const function is
   * used when the filter list should be changed.
   * @return List of name filters.
   */
  [[nodiscard]] FilterList& IncludeList() { return include_list_; }

  /** \brief Converts a list into a text string.
   *
   * Returns the include list as a string. The function is used when
   * exporting the list to a configuration file.
   * @return A string describing the list.
   */
  [[nodiscard]] std::string IncludeListToString() const;

  /** \brief Convert a string into a include list.
   *
   * Convert a string into an exclude list
   * @param text The input string
   */
  void StringToIncludeList(const std::string& text);

  /** \brief Scan the directory amd updated file and sub-directory lists.
   *
   * The function scans the directory and update its sub-directory and file
   * lists. The top-most directory i.e. the root directory level is set to 0
   * while its sub-directories level are set to 1.
   * @return True if the scan was successful.
   */
  virtual bool ScanDirectory();

  /** \brief Returns last error text.
   *
   * Returns the last error text.
   * @return The last error text.
   */
  [[nodiscard]] const std::string& LastError() const { return last_error_; }

 protected:
  /** \brief Checks if the name is matching anything in the include list.
   *
   * Returns true if the name matching any filter in the include list.
   * @param name Name to test.
   * @return True if matching any filter else false.
   */
  [[nodiscard]] bool IncludeFile(const std::string& name) const;

  /** \brief Checks if the name is matching anything in the exclude list.
   *
   * Returns true if the name matching any filter in the exclude list.
   * @param name Name to test.
   * @return True if matching any filter else false.
   */
  [[nodiscard]] bool ExcludeFile(const std::string& name) const;

 private:
  size_t level_ = 0;         ///< Holds the directory level.
  std::string directory_;    ///< Full path to the directory.
  DirectoryList dir_list_;   ///< List if sud-directories.
  FileList file_list_;       ///< List of files in the directory.
  FilterList exclude_list_;  ///< Name and extensions that should be excluded.
  FilterList include_list_;  ///< Name and extensions that should be included.
  uint64_t modified_ = 0;    ///< Last change time of the directory.
  std::string last_error_;   ///< Last error text
};

}  // namespace util::log
