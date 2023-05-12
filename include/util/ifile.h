/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <string>

namespace util::log {

/** \brief Class that wraps a file object in a directory.
 *
 * The IFile class is a simple wrapper around a file in an IDirectory object.
 * The IFile objects are normally created when calling the
 * IDirectory::ScanDirectory() function.
 */
class IFile {
 public:
  IFile() = default;  ///< Default constructor

  /** \brief Constructor with a filename as input.
   *
   * Constructor with a filename as input.
   * @param filename Filename with ful path.
   */
  explicit IFile(const std::string& filename);
  IFile(const IFile& source) = default;  ///< Default copy constructor
  virtual ~IFile();                      ///< Destructor

  [[nodiscard]] std::string Name()
      const;  ///< Name (stem) without path and extension.

  /** \brief Sets the filename.
   *
   * Sets the filename.
   * @param filename File name with full path.
   */
  void Filename(const std::string& filename);

  /** \brief Returns the filename including the path.
   *
   * Returns the filename including the path.
   * @return
   */
  [[nodiscard]] const std::string& Filename() const { return filename_; }

  /** \brief Sets a file checksum.
   *
   * Sets a file checksum for example MD%. File checksum is a fast way of
   * detecting if the file has changed.
   * @param checksum Checksum hexadecimal string
   */
  void Checksum(const std::string& checksum) { checksum_ = checksum; }

  /** \brief Returns the file checksum.
   *
   * Returns the file checksum string. Note that the file checksum
   * is not populated by this class as it take some time to do
   * the calculation. The NDF library have this functionality.
   * @return File checksum string.
   */
  [[nodiscard]] const std::string& Checksum() const { return checksum_; };

  /** \brief Sets the last change time for the file.
   *
   * Sets the last change time for the file. This class actual set this
   * property when the file is scanned by the IDirectory::ScanDirectory().
   * @param ns1970 Nano-second since 1970.
   */
  void Modified(uint64_t ns1970) { modified_ = ns1970; }

  /** \brief Returns the last change time.
   *
   * Returns the last change time. This class sets this time when the class
   * is created or the file name is set.
   * @return Name-second since 1970.
   */
  [[nodiscard]] uint64_t Modified() const { return modified_; }

  /** \brief Sets the file size.
   *
   * Setas the file size in bytes.This class sets this property when the class
   * is created or when the file name is set.
   * @param size File size in bytes.
   */
  void FileSize(uint64_t size) { file_size_ = size; }

  /** \brief Return the file size in bytes.
   *
   * Returns the file size in bytes.
   * @return File size in bytes.
   */
  [[nodiscard]] uint64_t FileSize() const { return file_size_; }

 private:
  std::string filename_;    ///< Full path to the file.
  std::string checksum_;    ///< Optional checksum (typical MD5).
  uint64_t modified_ = 0;   ///< Last modification time (ns since 1970).
  uint64_t file_size_ = 0;  ///< File size in bytes
};

}  // namespace util::log
