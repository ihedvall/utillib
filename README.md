# Util Library and Application

## Summary

The Util repository consist of a Util library, server and GUI applications. The C++ library is used as a base library
for other libraries and applications. It consist of the following base components.

- **CSV**. Simplifies the usage of creating comma separated data files.
- **Gnuplot**. Implements a wrapper around the third-party gnuplot plotting application.
- **Listen**. Client server and a user interface against the Listen functionality.
- **Logging**. Basic log functions for syslog, file or console logging.
- **XML**. Simple XML (DOM) wrapper around the third-party EXPAT library.
- **String**. Various string manipulations function that is missing in the standard C++.
- **Time and Date**. Various time and date functions.

The following companion applications exist.

- **Service Daemon**. Windows service that start and supervise other executables (daemons).
- **Service Explorer**. Support GUI for configure Windows service daemons.
- **Listen Daemon**. Simple daemon executable that supervise listen servers.
- **Listen Viewer**. GUI application that show listen messages.

## Installation

Installation kit for Windows [UtilLib v2.0.0](https://github.com/ihedvall/utillib/releases/download/v2.0.0/utillib.exe).

## Documentation

The documentation can be found on GitHub Pages: [UtilLib](https://ihedvall.github.io/utillib)

## Building the project

The project uses CMAKE for building. The following third-party libraries are used and
needs to be downloaded and built.

- Boost Library. Set the 'Boost_ROOT' variable to the Boost root path.
- Expat Library. Set the 'EXPAT_ROOT' variable to the expat root path.
- WinFlexBison Library. Required if flex or bison files are modified.
- ZLIB Library. Set the 'ZLIB_ROOT' variable to the ZLIB root path.
- WxWidgets Library. Is required if the GUI applications should be built.
- Doxygen's application. Is required if the documentation should be built.
- Google Test Library. Is required for running and build the unit tests.

### License

The project uses the MIT license. See external LICENSE file in project root.

