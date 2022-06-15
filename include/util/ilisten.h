/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file ilisten.h
 * \brief Defines an interface against the listen functionality.
 *
 * The listen interface object defines a debugging interface that
 * can be used in production code.
 *
 * The listen object is inactive as long as nobody has opened
 * a listen window GUI application against this listen object.
 */
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
namespace util::log {


/** \class IListen ilisten.h "util/ilisten.h
 * \brief An interface class that hides the actual implementation of the object.
 *
 * The IListen class defines an interface against various listen implementation objects. The class
 * hides the implementation classes.
 */
class IListen {
 public:
  /** \brief Creates a listen object.
   *
   * Creates a listen object. It only exist 2 basic object to create.
   *
   * The 'ListenProxy' object sends messages through a shared memory to a common server object. This
   * type of listen object is normal when several executable should share a listen 'channel'. Typically
   * is when debugging database calls.
   *
   * The 'ListenServer' shall be used when the application want its own listen 'channel'. This is
   * typically used for protocol drivers.
   * @param type Select between 'ListenProxy' or 'ListenServer'
   * @param share_name Unique share name or empty string.
   * @return A smart pointer to a listen object.
   */
  static std::unique_ptr<IListen> CreateListen(const std::string& type, const std::string& share_name);

  virtual ~IListen() = default; ///< Default destructor

  /** \brief Display name.
   *
   * User friendly short name that mainly is used when selecting a listen
   * object.
   * @return Display name.
   */
  [[nodiscard]] std::string Name() const;

  /** \brief Sets the display name.
   *
   * Sets the display name. The name should be short.
   * @param name Short display name.
   */
  void Name(const std::string& name);

  /** \brief Description of the listen object.
   *
   * Descriptive text of the listen object. Mainly used for
   * displaying purpose.
   * @return Description of the listen object.
   */
  [[nodiscard]] std::string Description() const;

  /** \brief Sets the description for the listen object.
   *
   * Sets the description of the listen object.
   * @param description
   */
  void Description(const std::string& description);

  /** \brief Text that always starts a text line.
   *
   * The pre-text is a short text that is added to each listen
   * text lines. This property defines the default text.
   * @return The default pre-text.
   */
  [[nodiscard]] std::string PreText() const;

  /** \brief Sets the default pre-text that starts each line.
   *
   * Sets the pre-text string that is added to each text line.
   * The pre-text should be short. Note that this property
   * sets the default text.
   * @param pre_text Short pre-text before each text line.
   */
  void PreText(const std::string& pre_text);

  /** \brief Host name for the object.
   *
   * Host name for the listen object. Only valid for listen servers.
   */
  [[nodiscard]] std::string HostName() const;

  /** \brief Sets the host name for the listen object.
   *
   * Sets the host name of the listen object. Only valid for listen
   * servers.
   *
   * Default is localhost (127.0.0.1).
   * @param host_name Host name or IP port.
   */
  void HostName(const std::string& host_name);

  /** \brief TCP/IP port
   *
   * Returns the TCP/IP port. Only servers.
   * @return TCP/IP port
   */
  [[nodiscard]] uint16_t Port() const;

  /** \brief Sets the TCP/IP port.
   *
   * Sets the TCP/IP port for listen servers.
   * @param port TCP/IP port
   */
  void Port(uint16_t port);

   /** \brief Sets the log level menu texts.
    *
    * Sets the a log levels menu text. Note that level 0 should
    * always be defined and it will be the default level.
    * @param level Log level number (index).
    * @param menu_text Menu text
    */
  void SetLogLevelText(uint64_t level, const std::string& menu_text);

  /** \brief Returns all log levels and their menu texts.
   *
   * Returns a list of leg levels and their menu texts
   * @return Sorted list of log levels in use.
   */
  [[nodiscard]] const std::map<uint64_t,std::string>& LogLevelList() const;

  /** \brief Generates a listen text line.
   *
   * Generates a listen text line with the default pre-text and now as time stmp.
   * @param format_text C-style text format
   * @param ... Ellipse function.
   */
  virtual void ListenText(const char* format_text, ... );

  /** \brief Generate a user defined text line.
   *
   * Generates a text line with user defined time stamp and pre-text.
   * @param ns1970 Time stamp nanoseconds since 1970.
   * @param pre_text Pre-text string.
   * @param format_text C-style text format.
   * @param ... Ellipse function.
   */
  virtual void ListenTextEx(uint64_t ns1970, const std::string& pre_text, const char* format_text, ... );

  /** \brief Generates a hex string text from a byte buffer
   *
   * Function used when parsing byte buffer typically in protocol applications. By default
   * it converts the byte buffer to a hexadecimal string.
   * @param ns1970 Time stamp nanoseconds since 1970.
   * @param pre_text Pre-text string.
   * @param buffer Byte buffer.
   * @param hint Optional hint
   */
  virtual void ListenTransmit(uint64_t ns1970, const std::string& pre_text, const std::vector<uint8_t>& buffer, void* hint);

  /** \brief Generates a hex string text from a byte buffer
   *
   * Function used when parsing byte buffer typically in protocol applications. By default
   * it converts the byte buffer to a hexadecimal string.
   * @param ns1970 Time stamp nanoseconds since 1970.
   * @param pre_text Pre-text string.
   * @param buffer Byte buffer.
   * @param hint Optional hint
   */
  virtual void ListenReceive(uint64_t ns1970, const std::string& pre_text, const std::vector<uint8_t>& buffer, void* hint);

  /** \brief Indicate if any listen window is open.
   *
   * Is true if any listen window is connected to this listen object.
   * @return True if the listen object should generate text lines.
   */
  [[nodiscard]] virtual bool IsActive() const = 0;

  /** \brief Returns the current log level.
   *
   * Returns the current log level
   * @return
   */
  [[nodiscard]] virtual size_t LogLevel() = 0;

  /** \brief Starts the listen object.
   *
   * Starts the listen object.
   * @return True if started.
   */
  virtual bool Start();

  /** \brief Stops the listen object.
   *
   * Stops the listen object.
   * @return
   */
  virtual bool Stop();

 protected:
  std::string share_name_; ///< Share memory name
  std::string name_;  ///< Display name.
  std::string description_; ///< Description of the functionality.
  std::string pre_text_; ///< Small text between time and text string.
  std::string host_name_ = "127.0.0.1"; ///< Host name
  uint16_t port_ = 0; ///< IP-port to listen on.
  std::map<uint64_t,std::string> log_level_list_; ///< Log level index and

  IListen() = default; ///< Default constructor

  /** \brief Adds a listen message.
   *
   * All text lines generates a listen message that is sent to a TCP/IP server or via
   * a proxy server (shared memory).
   * @param nano_sec_1970 Time stamp nanoseconds since 1970
   * @param pre_text Pre-text string
   * @param text Message text
   */
  virtual void AddMessage(uint64_t nano_sec_1970, const std::string& pre_text, const std::string& text) = 0;

  /** \brief Parses a byte buffer into a hexadecimal string.
   *
   * @param buffer
   * @return
   */
  static std::string ParseHex(const std::vector<uint8_t>& buffer);


 private:

};



}



