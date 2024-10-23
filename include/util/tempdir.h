/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file tempdir.h
 *  \brief Helper class that creates a directory in the system temporary path.
 */
#pragma once
#include <string>
namespace util::log {
/**
 * The class creates a directory on the temporary path on the computer. The
 * directory can be used for storing temporary files.
 *
 * The directory is removed
 * when the destructor is called.
 */
class TempDir {
 public:
  TempDir() = delete;
  /** \brief Creates a temporary directory.
   *
   * Creates a temporary directory in the default temporary directory. Normally
   * the new directory name would be the application name. If multiple
   * application using this function, are running on this computer, it is
   * recommended that the unique_dir flag is set.
   *
   * A unique directory is named 'sub_dir_XXXX' where X is a random hexadecimal
   * digit. If the unique_dir flag is false, the director is named 'sub_dir'
   *
   * The temporary directory and its files are removed when the destructor is
   * called.
   *
   * @param sub_dir Recommended sub-directory base name. If empt a directory
   * 'utillib' is created.
   * @param unique_dir If set to true, a unique directory name is created.
   */
  TempDir(const std::string &sub_dir, bool unique_dir);

  /** \brief The destructor removes the temporary directory.
   *
   * The destructor removes the created directory and its files
   */
  virtual ~TempDir();

  /** \brief Full path to the temporary directory.
   *
   * Returns full path to the temporary directory.
   * @return Path to temporary directory.
   */
  [[nodiscard]] const std::string &Path() const { return temp_dir_; }

  /** \brief Generates a temporary and optional unique file name.
   *
   * Generates a temporary file name with path and extension.
   * @param stem Base name of the file.
   * @param extension File extension
   * @param unique_file Set to true if a unique file name should be generated.
   * @return File name with path and extension.
   */
  [[nodiscard]] std::string TempFile(const std::string &stem,
                                     const std::string &extension,
                                     bool unique_file) const;

 private:
  std::string temp_dir_;
};

}  // end namespace util::log
