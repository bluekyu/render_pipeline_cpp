#.rst:
# FindOpenVR
# --------------
#
# FindOpenVR.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   OpenVR_INCLUDE_DIRS     - Include directories for OpenVR
#   OpenVR_LIBRARIES        - Libraries to link against OpenVR
#   OpenVR_FOUND            - True if OpenVR has been found and can be used
#   OpenVR_VERSION          - Library version for OpenVR
#   OpenVR_VERSION_STRING   - Library version for OpenVR
#   OpenVR_VERSION_MAJOR    - Library major version for OpenVR
#   OpenVR_VERSION_MINOR    - Library minor version for OpenVR
#   OpenVR_VERSION_PATCH    - Library patch version for OpenVR
#
# and the following imported targets::
#
#   OpenVR::OpenVR  - The OpenVR library

cmake_minimum_required(VERSION 3.2)

set(PLATFORM_PREFIX "")
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM_PREFIX "win64")
    else()
        set(PLATFORM_PREFIX "win32")
    endif()
elseif(UNIX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM_PREFIX "linux64")
    else()
        set(PLATFORM_PREFIX "linux32")
    endif()
else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}")
endif()

find_path(OpenVR_INCLUDE_DIR
    NAMES "openvr.h"
    HINTS ${OPENVR_INCLUDEDIR} "${OPENVR_ROOT}/headers"
)
find_library(OpenVR_LIBRARY
    NAMES openvr_api
    HINTS ${OPENVR_LIBRARYDIR} "${OPENVR_ROOT}/lib/${PLATFORM_PREFIX}"
)

# Set OpenVR_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVR
    FOUND_VAR OpenVR_FOUND
    REQUIRED_VARS OpenVR_LIBRARY OpenVR_INCLUDE_DIR
    VERSION_VAR OpenVR_VERSION
)

if(OpenVR_FOUND)
    if(OpenVR_LIBRARY)
        set(OpenVR_LIBRARIES    ${OpenVR_LIBRARY})
    endif()
    set(OpenVR_INCLUDE_DIRS ${OpenVR_INCLUDE_DIR})
endif()

mark_as_advanced(
    OpenVR_INCLUDE_DIRS
    OpenVR_LIBRARIES
)
