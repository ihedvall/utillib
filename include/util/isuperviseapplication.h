/*
* Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file isuperviseapplication.h
 * @brief Interface against a task that supervise that an executable is running.
 *
 * Interface against a task that start, supervise and stop an executable.
 * It belongs to a Supervise Master which is a list of applications that
 * should be supervised.
 *
 */
#pragma once

#include <algorithm>
#include <atomic>
#include <string>
#include <string_view>
#include <vector>

namespace util::xml {
class IXmlNode;
}

namespace util::supervise {

/**
 * @brief The supervisor state.
 */
enum class ApplicationState : uint8_t {
  Stopped = 0, ///< Application is (manually) stopped.
  Running = 1, ///< Application is running.
  Failing = 2, ///< Application is failing (not running).
  Ready = 3,   ///< Application is ready (not running).
};

/**
 * @brief Defines the type of startup for tha tas.
 *
 * The startup mode is set to each application and
 * defines how it start, stop and being supervised.
 *
 * In the Manual mode, the application is started when
 * it is taken in-service by a remote application.
 * If it is taken out-of-service, it stops.
 * If it exit before it is taken out-of-service,
 * the state is set to ApplicationState::Ready.
 *
 * In the Once mode, the application only run once
 * at startup.
 * It is very similar to the Manual mode but it
 * is set to in-service at startup.
 *
 * In the Automatic mode, the application is
 * started at startup and then checked that it
 * is running.
 * If the application stops, it will be restarted.
 * Note that the restart is delayed 2-10 seconds.
 * If number of restarts reach  a maximum limit,
 * it will stop restarting and a critical log
 * event will be generated.
 */
enum class ApplicationStartup : uint8_t {
  Manual = 0, ///< Manual start and stop of the application.
  Once = 1, ///< Starts only one time.
  Automatic = 2, ///< Starts and restarts if stopped.
};

/**
 * @brief Defines the priority uses when it is running.
 *
 * Defines the priority the application uses when it is
 * running.
 * Background task should use Idle priority or High priority.
 * GUI task should use Normal priority.
 * Realtime should not be used as this may hang the computer.
 */
enum class ApplicationPriority : uint8_t {
  Realtime, ///< Realtime task. Avoid to use this priority.
  High, ///< High priority.
  AboveNormal, ///< Above normal. Questionable usage.
  Normal, ///< Typical GUI applications uses this priority.
  BelowNormal, ///< Below normal. Questionable usage.
  Idle ///< Typical for most background task. Recommended usage.
};

/**
 * @brief Supervise an executable application.
 *
 * The class supervise an external executable application.
 * It start, supervise and stop the application.
 * The application startup mode controls how the application
 * is supervised.
 *
 * In the Manual mode, the application is started when
 * it is taken in-service by a remote application.
 * If it is taken out-of-service, it stops.
 * If it exit before it is taken out-of-service,
 * the state is set to ApplicationState::Ready.
 *
 * In the Once mode, the application only run once
 * at startup.
 * It is very similar to the Manual mode but it
 * is set to in-service at startup.
 *
 * In the Automatic mode, the application is
 * started at startup and then checked that it
 * is running.
 * If the application stops, it will be restarted.
 * Note that the restart is delayed 2-10 seconds.
 * If number of restarts reach  a maximum limit,
 * it will stop restarting and a critical log
 * event will be generated.
 */
class ISuperviseApplication {
 public:
  /** \brief Default destructor */
  virtual ~ISuperviseApplication() = default;

  /**
   * @brief Name of the application.
   *
   * The name is a property that is used internally for
   * identification of the application.
   * The name is used when generating logs and events.
   * @param name Name of the application.
   */
  void Name(std::string name) { name_ = std::move(name); }

  /**
   * @brief Name of the application.
   * @return Name of the application.
   */
  [[nodiscard]] const std::string &Name() const { return name_; }

  /**
   * @brief Path to the executable.
   *
   * The path to the executable.
   * If only the executable name is set, the class will search
   * for the executable in known paths.
   * @param path Path to the executable.
   */
  void Path(std::string path) {
    path_ = std::move(path);
  }

  /**
   * @brief Path to the executable.
   * @return Path to the executable.
   */
  [[nodiscard]] const std::string &Path() const {
    return path_;
  }

  /**
   * @brief Command line arguments to use at startup.
   * @param arguments Command line arguments
   */
  void Arguments(std::string arguments) {
    arguments_ = std::move(arguments);
  }

  /**
   * @brief Command line arguments.
   * @return Command line arguments.
   */
  [[nodiscard]] const std::string &Arguments() const {
    return arguments_;
  }

  /**
   * @brief Startup mode for the application.
   *
   * The startup mode is set to each application and
   * defines how it start, stop and being supervised.
   *
   * In the Manual mode, the application is started when
   * it is taken in-service by a remote application.
   * If it is taken out-of-service, it stops.
   * If it exit before it is taken out-of-service,
   * the state is set to ApplicationState::Ready.
   *
   * In the Once mode, the application only run once
   * at startup.
   * It is very similar to the Manual mode but it
   * is set to in-service at startup.
   *
   * In the Automatic mode, the application is
   * started at startup and then checked that it
   * is running.
   * If the application stops, it will be restarted.
   * Note that the restart is delayed 2-10 seconds.
   * If number of restarts reach  a maximum limit,
   * it will stop restarting and a critical log
   * event will be generated.
   * @param startup
   */
  void Startup(ApplicationStartup startup) { startup_ = startup; }

  /**
   * @brief Startup mode for the application.
   * @return Startup mode enumerate.
   */
  [[nodiscard]] ApplicationStartup Startup() const { return startup_; }


  /**
   * @brief Defines the priority for the executable.
   * @param priority Executable priority.
   */
  void Priority(ApplicationPriority priority) { priority_ = priority; }

  /**
   * @brief Returns the executable priority.
   * @return Executable priority.
   */
  [[nodiscard]] ApplicationPriority Priority() const { return priority_; }

  /**
   * @brief Sets to true if a window should be shown.
   *
   * Set to true if the window should be shown.
   * In reality, this option is dependent on which user that
   * was logged in.
   * For example, a service user doesn't own any desktop to
   * show any window in.
   *
   * @param show True if window should be shown.
   */
  void ShowWindow(bool show) { show_window_ = show; }

  /**
   * @brief True if window should be shown.
   * @return True if window should be shown.
   */
  [[nodiscard]] bool ShowWindow() const { return show_window_; }

  /**
   * @brief Initial startup delay in seconds.
   *
   * Delay in seconds that the first start is delayed.
   * @param delay Startup delay in seconds.
   */
  void StartupDelay(uint32_t delay) { startup_delay_ = delay; }

  /**
   * @brief Returns the startup delay in seconds.
   * @return Startup delay in seconds.
   */
  [[nodiscard]] uint32_t StartupDelay() const { return startup_delay_; }

  /**
   * @brief Sets maximum number of restarts.
   * @param max_restarts Max restarts.
   */
  void MaxRestarts(uint32_t max_restarts) { max_restarts_ = max_restarts; }

  /**
   * @brief Maximum number of restarts.
   * @return Max restarts.
   */
  [[nodiscard]] uint32_t MaxRestarts() const { return max_restarts_; }

  /**
   * @brief Starts or stops the executable.
   * @param in_service If true, starts the executable.
   * False stops the executable.
   */
  void InService(bool in_service) { in_service_ = in_service; }

  /**
   * @brief Returns true if the executable should be started.
   * @return True if the executable should be started.
   */
  [[nodiscard]] bool InService() const { return in_service_; }

  /**
   * @brief Returns number of restarts.
   * @return Number of restarts.
   */
  [[nodiscard]] uint32_t NofRestarts() const { return nof_restarts_; }

  /**
   * @brief Returns true if the executable is running.
   * @return True if the application is running normally.
   */
  [[nodiscard]] bool Operable() const { return operable_; }

  /**
   * @brief Returns the state of the supervisor.
   * @return State of the supervisor.
   */
  [[nodiscard]] ApplicationState State() const { return state_; }

  /**
   * @brief Initialize and start the supervisor.
   */
  virtual void Start();

  /**
   * @brief Stops the supervisor.
   */
  virtual void Stop();

  /**
   * @brief Poll the supervisor.
   */
  virtual void Poll();

  /**
   * @brief Reads in the configuration.
   * @param application_node The application XML node.
   */
  void ReadConfig(const xml::IXmlNode &application_node);

  /**
   * @brief Saves the application configuration.
   * @param root_node The root XML node.
   */
  void SaveConfig(xml::IXmlNode &root_node) const;

 protected:

  /**
   * @brief Increment number of restarts.
   */
  void IncrementNofRestarts() { ++nof_restarts_; }

  /**
   * @brief Reset number of restarts..
   */
  void ResetNofRestarts() { nof_restarts_ = 0; }

  /**
   * @brief Sets the operable property.
   * @param operable True if operable.
   */
  void Operable(bool operable) { operable_ = operable; }

  /**
   * @brief Sets the supervisor state.
   * @param state New state.
   */
  void State(ApplicationState state) { state_ = state; }

  /**
   * @brief Convert the command line into an array of arguments.
   * @return List of arguments.
   */
  std::vector<std::string> ParseCommandLine() const;

 private:
  std::string name_; ///< Name of application.
  std::string path_; ///< Path to executable.
  std::string arguments_; ///< Command line arguments.

  bool show_window_ = true; ///< Shoe window property.
  uint32_t startup_delay_ = 0; ///< Startup delay in seconds.
  uint32_t max_restarts_ = 100; ///< Max number of restarts.

  std::atomic<ApplicationStartup> startup_ = ApplicationStartup::Manual;
  ApplicationPriority priority_ = ApplicationPriority::Normal;

  std::atomic<uint32_t> nof_restarts_ = 0;
  std::atomic<bool> in_service_ = false;
  std::atomic<bool> operable_ = false;
  std::atomic<ApplicationState> state_ = ApplicationState::Stopped;

/**
   * @brief Sets the startup mode by a string.
   * @param startup Startup string.
   */
  void StartupAsString(const std::string &startup);

  /**
   * @brief Returns the startup mode as string.
   * @return Startup mode as string.
   */
  [[nodiscard]] std::string_view StartupAsString() const;

  /**
   * @brief Sets the priority from a string.
   * @param priority Priority as a string.
   */
  void PriorityAsString(const std::string &priority);

  /**
   * @brief Returns the priority as a string
   * @return Priority as a string.
   */
  [[nodiscard]] std::string_view PriorityAsString() const;

};

} // util


