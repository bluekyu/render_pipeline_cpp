#.rst:
# FindSpdlog
# --------------
#
# FindSpdlog.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2017-01-18
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   Spdlog_INCLUDE_DIRS     - Include directories for Spdlog
#   Spdlog_FOUND            - True if Spdlog has been found and can be used
#
# and the following imported targets::
#
#   Spdlog::Spdlog  - The Spdlog library

cmake_minimum_required(VERSION 3.2)

find_path(Spdlog_INCLUDE_DIR
    NAMES "spdlog/spdlog.h"
    HINTS ${SPDLOG_INCLUDEDIR} "${SPDLOG_ROOT}/include"
)

# Set Spdlog_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Spdlog
    FOUND_VAR Spdlog_FOUND
    REQUIRED_VARS Spdlog_INCLUDE_DIR
)
if(Spdlog_FOUND)
    set(Spdlog_INCLUDE_DIRS ${Spdlog_INCLUDE_DIR})
endif()

mark_as_advanced(
    Spdlog_INCLUDE_DIRS
)
