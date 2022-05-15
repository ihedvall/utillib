
| [Home](/index.html) | [Log](/log.html) | | [Listen](/listen.html) | [Time](/time.html) | [XML](/xml.html) |


# Log Functions 
The log function is based upon 3 objects.

![Overview Log Functionality](/assets/img/log.png)

## Log Config
The LogConfig class is a singleton class that hold zero or more loggers (ILogger). 
The logger shall simply handle a log messages and send it to its destination.

## Loggers
The user can add its own logger or create one of the 3 predefined logger. The following loggers
are defined in the library.

- **Log To Console**. Sends the messages to the console output.
- **Log To File**. Saves the messages to a file.
- **Log To Listen**. Sends the log messages to an external listen window.

It is possible to have many loggers added to the log chain (list of loggers).

## Log Messages
The Log Messages are added to the code normally by using the LOG_***() macro functions defined in the 
'util/logstream.h' include file. A message consist of time, severity, message text and source file location.

The time and source location is normally set by the software but the message text and severity are normally changed
by the user. The following severity levels exist.

- **Trace**. The message is meant for internal use and rarely used.
- **Debug**. Internal message use and nothing an end-user shall see.
- **Info**. Informational message both for internal and end-users.
- **Warning**. Warning message telling a user that something might fail later on.
- **Error**. Normally internal software error.
- **Critical**. Critical error as device error, that the user should be notified of.
- **Alert**. Alarm error that the user must be notified of.
- **Emergency**. Fatal error is generated when something stopped working.