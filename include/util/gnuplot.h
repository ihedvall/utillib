/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file gnuplot.h
 * \brief Simple wrapper to simplify plotting using the gnuplot application.
 *
 * The gnuplot fetch all its information from a CSV file, so the trick is
 * to add all necessary information to that CSV file.
 *
 * The plot support 2 Y-axis and one X-axis. The X-axis shall be the first
 * column in the CSV file followed by the Y-axis columns.
 *
 * It may be more than 2 Y-values on each axis as long as they have the same
 * unit.
 */
#pragma once
#include <string>

#include "util/csvwriter.h"

namespace util::plot {

/** \enum GnuTerminal
 * \brief Types of gnuplot output devices
 *
 */
enum class GnuTerminal {
  wxt = 0  ///< wxWidgets window
};

/** \class GnuPlot gnuplot.h "util/gnuplot.h
 * \brief Simple wrapper around the gnuplot application that do plotting.
 *
 * Simple wrapper around the gnuplot application. The gnuplot do basic
 * plotting and fetch its values from a CSV file, so the trick is to do the
 * CSV file.
 */
class GnuPlot {
 public:
  /** \brief Constructor
   *
   * The constructor search for the GnuPlot application on the computer. If
   * the application not in its default path, it needs to be in the computer
   * path environment. There is a checkbox for this when installing gnuplot.
   */
  GnuPlot();
  virtual ~GnuPlot() = default;  ///< Destructor

  /** \brief Returns the full path to the gnuplot application.
   *
   * Full path to the gnuplot application.
   * @return Full path to the gnuplot application.
   */
  [[nodiscard]] const std::string& ExePath() const { return exe_path_; }

  /** \brief Name of the gnuplot report
   *
   * This name is used to name the gnuplot and CSV files if they are saved.
   * @return Name of the report.
   */
  [[nodiscard]] const std::string& Name() const { return name_; }

  /** \brief Name of the gnuplot report
   * If the plot report shall be saved, this defines the name of the gnuplot
   * file and its CSV file. The name shall be without any path and extension.
   * The Path property defines where these reports are saved.
   * @param name Name of report without path or extension.
   */
  void Name(const std::string& name) { name_ = name; }

  /** \brief Path where reports are saved
   *
   * Directory path where reports are saved.
   * @return
   */
  [[nodiscard]] const std::string& Path() const { return path_; }

  /** \brief Path where report are saved
   *
   * Directory path where reports are saved.
   * @param path
   */
  void Path(const std::string& path) { path_ = path; }

  /** \brief Returns the full path to the gnuplot script file.
   *
   * Returns the full name of the gnuplot script file.
   * @return Gnuplot script file.
   */
  [[nodiscard]] std::string FileName() const;

  /** \brief Returns the full path of the CSV file.
   *
   * Returns the full path of the CSV file with all data.
   * @return CSV data file.
   */
  [[nodiscard]] std::string CsvFileName() const;

  /** \brief Creates a script string.
   *
   * Creates a script string for temporary plotting and viewing.
   * @return Gnuplot script as a string.
   */
  [[nodiscard]] std::string CreateScript() const;

  /** \brief Creates and saves the gnuplot script in a file.
   *
   * Create and saves the gnuplot script in a file.
   */
  void SaveScript();

  /** \brief Shows the plot on the screen.
   *
   * Shows the gnuplot on the screen.
   */
  void Show();

  /** \brief Returns a reference to a CSV writer object.
   *
   * Returns a reference to a CSV writer object.
   * @return
   */
  CsvWriter& CsvFile() { return data_file_; }

 private:
  std::string exe_path_;  ///< Full path to the gnuplot executable.
  std::string name_;  ///< Name on CSV and gnuplot files, No path or extension.
  std::string
      path_;  ///< Directory where the gnuplot and its data files are stored.

  GnuTerminal terminal_ = GnuTerminal::wxt;

  std::string title_;
  size_t x_size_ = 1200;
  size_t y_size_ = 800;

  std::string x_label_;
  std::string y_label_;
  std::string x2_label_;
  std::string y2_label_;

  CsvWriter data_file_;

  void MakeScript(std::ostream& script) const;
};

}  // namespace util::plot