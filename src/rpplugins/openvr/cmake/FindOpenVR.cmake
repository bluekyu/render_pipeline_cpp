#.rst:
# FindOpenVR
# --------------
#
# FindOpenVR.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-19
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   OpenVR_FOUND            - True if OpenVR has been found and can be used
#
# and the following imported targets::
#
#   OpenVR                  - The OpenVR library

cmake_minimum_required(VERSION 3.6)

set(OpenVR_PLATFORM_PREFIX "")
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OpenVR_PLATFORM_PREFIX "win64")
    else()
        set(OpenVR_PLATFORM_PREFIX "win32")
    endif()
elseif(UNIX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OpenVR_PLATFORM_PREFIX "linux64")
    else()
        set(OpenVR_PLATFORM_PREFIX "linux32")
    endif()
elseif(APPLE)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OpenVR_PLATFORM_PREFIX "osx64")
    else()
        set(OpenVR_PLATFORM_PREFIX "osx32")
    endif()
else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}")
    return()
endif()

find_path(OpenVR_INCLUDE_DIR
    NAMES "openvr.h"
    HINTS "${OpenVR_ROOT}/headers"
)

find_library(OpenVR_LIBRARY
    NAMES openvr_api
    HINTS "${OpenVR_ROOT}/lib"
    PATH_SUFFIXES ${OpenVR_PLATFORM_PREFIX}
)

# Set OpenVR_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVR
    FOUND_VAR OpenVR_FOUND
    REQUIRED_VARS OpenVR_LIBRARY OpenVR_INCLUDE_DIR
)

if(OpenVR_FOUND)
    message(STATUS "Found the OpenVR")

    add_library(OpenVR UNKNOWN IMPORTED)

    if(EXISTS "${OpenVR_LIBRARY}")
        set_target_properties(OpenVR PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${OpenVR_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_LOCATION "${OpenVR_LIBRARY}"
        )
    endif()

    # Make variables changeable to the advanced user
    mark_as_advanced(
        OpenVR_INCLUDE_DIR
        OpenVR_LIBRARY
    )
endif()
