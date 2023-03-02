/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <util/ifile.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace util::log {
class IDirectory;
using DirectoryList = std::map<std::string, std::unique_ptr<IDirectory>>;
using FileList = std::map<std::string, std::unique_ptr<IFile>>;
using FilterList = std::vector<std::string>;

class IDirectory {
 public:
  IDirectory() = default;
  IDirectory(const IDirectory& source);
  explicit IDirectory(const std::string& filename);
  virtual ~IDirectory() = default;

  [[nodiscard]] std::string Name() const;  ///< Name (stem) without path.

  void ParentDir(const std::string& dir);
  [[nodiscard]] const std::string& ParentDir() const { return parent_dir_; }

  void Modified(uint64_t ns1970) { modified_ = ns1970; }
  [[nodiscard]] uint64_t Modified() const { return modified_; }

  void Level(size_t level) { level_ = level; }
  [[nodiscard]] size_t Level() const { return level_; }

  [[nodiscard]] const DirectoryList& Directories() const { return dir_list_; }
  [[nodiscard]] const FileList& Files() const { return file_list_; }

  [[nodiscard]] const FilterList& ExcludeList() const { return exclude_list_; }
  [[nodiscard]] FilterList& ExcludeList() { return exclude_list_; }

  [[nodiscard]] std::string ExcludeListToString() const;
  void StringToExcludeList(const std::string& text);

  [[nodiscard]] const FilterList& IncludeList() const { return include_list_; }
  [[nodiscard]] FilterList& IncludeList() { return include_list_; }

  [[nodiscard]] std::string IncludeListToString() const;
  void StringToIncludeList(const std::string& text);

  virtual bool ScanDirectory();

  [[nodiscard]] const std::string& LastError() const { return last_error_; }

 protected:
  [[nodiscard]] bool IncludeFile(const std::string& name) const;
  [[nodiscard]] bool ExcludeFile(const std::string& name) const;

 private:
  size_t level_ = 0;        ///< Holds the directory level
  std::string parent_dir_;  ///< Full path to the parent directory
  DirectoryList dir_list_;
  FileList file_list_;
  FilterList exclude_list_;  ///< Name and extensions that should be excluded
  FilterList include_list_;  ///< Name and extensions that should be included
  uint64_t modified_ = 0;
  std::string last_error_;
};

}  // namespace util::log
