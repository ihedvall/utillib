# Copyright 2024 Ingemar Hedvall
# SPDX-License-Identifier: MIT
# The Hardware Info is fetched from the GitHub and built into the
# library

include(FetchContent)
include(CMakePrintHelpers)

set(HWINFO_SHARED OFF)
set(HWINFO_STATIC ON)

FetchContent_Declare(hwinfo
        GIT_REPOSITORY https://github.com/lfreist/hwinfo.git
        GIT_TAG HEAD
)

FetchContent_MakeAvailable(hwinfo)

cmake_print_variables(hwinfo_POPULATED hwinfo_SOURCE_DIR hwinfo_BINARY_DIR)
