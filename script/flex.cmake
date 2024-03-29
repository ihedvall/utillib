# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT


if (NOT FLEX_FOUND)
    find_package(FLEX)
    cmake_print_variables(FLEX_FOUND FLEX_EXECUTABLE FLEX_VERSION FLEX_LIBRARIES FLEX_INCLUDE_DIRS)
endif()