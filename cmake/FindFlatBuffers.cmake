#.rst:
# FindFlatBuffers
# --------------
#
# FindFlatBuffers.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2017-07-07
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   FlatBuffers_FOUND            - True if FlatBuffers has been found and can be used
#
# and the following imported targets::
#
#   flatbuffers        - FlatBuffers library

cmake_minimum_required(VERSION 3.6)

find_path(FlatBuffers_INCLUDE_DIR
    NAMES "flatbuffers/flatbuffers.h"
    PATHS "${FlatBuffers_ROOT}"
    PATH_SUFFIXES include
)

find_library(FlatBuffers_LIBRARY
    NAMES flatbuffers
    PATHS "${FlatBuffers_ROOT}"
    PATH_SUFFIXES lib
)

find_program(FlatBuffers_flatc
    flatc
    PATHS "${FlatBuffers_ROOT}/bin"
)

# Set FlatBuffers_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FlatBuffers
    FOUND_VAR FlatBuffers_FOUND
    REQUIRED_VARS FlatBuffers_LIBRARY FlatBuffers_INCLUDE_DIR
)

if(FlatBuffers_FOUND)
    message(STATUS "Found the FlatBuffers library")

    if(NOT TARGET flatbuffers)
        add_library(flatbuffers UNKNOWN IMPORTED)
        set_target_properties(flatbuffers PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${FlatBuffers_INCLUDE_DIR}"
            IMPORTED_LOCATION "${FlatBuffers_LIBRARY}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        )

        # Make variables changeable to the advanced user
        mark_as_advanced(FlatBuffers_LIBRARY)
    endif()

    if(FlatBuffers_flatc)
        mark_as_advanced(FlatBuffers_flatc)
    endif()

    # Make variables changeable to the advanced user
    mark_as_advanced(FlatBuffers_INCLUDE_DIR)
endif()
