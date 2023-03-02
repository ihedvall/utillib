/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <string>

namespace util::log {

class IFile {
 public:
  IFile() = default;
  explicit IFile(const std::string& filename);
  IFile(const IFile& source) = default;
  virtual ~IFile();

  [[nodiscard]] std::string Name()
      const;  ///< Name (stem) without path and extension.

  void Filename(const std::string& filename);
  [[nodiscard]] const std::string& Filename() const { return filename_; }

  void Checksum(const std::string& checksum) { checksum_ = checksum; }
  [[nodiscard]] const std::string& Checksum() const { return checksum_; };

  void Modified(uint64_t ns1970) { modified_ = ns1970; }
  [[nodiscard]] uint64_t Modified() const { return modified_; }

  void FileSize(uint64_t size) { file_size_ = size; }
  [[nodiscard]] uint64_t FileSize() const { return file_size_; }

 private:
  std::string filename_;    ///< Full path to the file.
  std::string checksum_;    ///< Optional checksum (typical MD5).
  uint64_t modified_ = 0;   ///< Last modification time (ns since 1970).
  uint64_t file_size_ = 0;  ///< File size in bytes
};

}  // namespace util::log
