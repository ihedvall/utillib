# Copyright 2025 Ingemar Hedvall
# SPDX-License-Identifier: MIT
# The Platform Folders is fetched from the GitHub and built into the
# library

include(FetchContent)
include(CMakePrintHelpers)

FetchContent_Declare(platform-folders
        GIT_REPOSITORY https://github.com/sago007/PlatformFolders.git
        GIT_TAG HEAD
)

set(PLATFORMFOLDERS_BUILD_SHARED_LIBS OFF CACHE INTERNAL "Turn off shared build")
set(PLATFORMFOLDERS_BUILD_TESTING OFF CACHE INTERNAL "Turn off testing")
set(PLATFORMFOLDERS_ENABLE_INSTALL OFF CACHE INTERNAL "Turn off install")

FetchContent_MakeAvailable(platform-folders)

cmake_print_variables(platform-folders_POPULATED platform-folders_SOURCE_DIR platform-folders_BINARY_DIR)
