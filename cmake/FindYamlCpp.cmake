#.rst:
# FindYamlCpp
# --------------
#
# FindYamlCpp.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   YamlCpp_INCLUDE_DIRS     - Include directories for yaml-cpp
#   YamlCpp_LIBRARIES        - Libraries to link against yaml-cpp
#   YamlCpp_FOUND            - True if yaml-cpp has been found and can be used
#   YamlCpp_VERSION          - Library version for yaml-cpp
#   YamlCpp_VERSION_STRING   - Library version for yaml-cpp
#   YamlCpp_VERSION_MAJOR    - Library major version for yaml-cpp
#   YamlCpp_VERSION_MINOR    - Library minor version for yaml-cpp
#   YamlCpp_VERSION_PATCH    - Library patch version for yaml-cpp
#
# and the following imported targets::
#
#   YamlCpp::YamlCpp  - The yaml-cpp library

cmake_minimum_required(VERSION 3.2)

set(YamlCpp_LIB_PREFIX "")
set(YamlCpp_MULTITHREADED "")
if(YAMLCPP_USE_STATIC_LIBS)
    set(YamlCpp_LIB_PREFIX "lib")
    set(YamlCpp_MULTITHREADED "mt")
    if(YAMLCPP_USE_MULTITHREADED)
        set(YamlCpp_MULTITHREADED "md")
    endif()
endif()

find_path(YamlCpp_INCLUDE_DIR
    NAMES "yaml-cpp/yaml.h"
    HINTS ${YAMLCPP_INCLUDEDIR} "${YAMLCPP_ROOT}/include"
    PATH_SUFFIXES yaml-cpp
)
find_library(YamlCpp_LIBRARY
    NAMES ${YamlCpp_LIB_PREFIX}yaml-cpp${YamlCpp_MULTITHREADED}
    HINTS ${YAMLCPP_LIBRARYDIR} "${YAMLCPP_ROOT}/lib"
    PATH_SUFFIXES lib64 lib
)
find_library(YamlCpp_LIBRARY_DEBUG
    NAMES ${YamlCpp_LIB_PREFIX}yaml-cpp${YamlCpp_MULTITHREADED}d
    HINTS ${YAMLCPP_LIBRARYDIR} "${YAMLCPP_ROOT}/lib"
)

# Set YamlCpp_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YamlCpp
    FOUND_VAR YamlCpp_FOUND
    REQUIRED_VARS YamlCpp_LIBRARY YamlCpp_INCLUDE_DIR
    VERSION_VAR YamlCpp_VERSION
)

if(YamlCpp_FOUND)
    # found both
    if(YamlCpp_LIBRARY AND YamlCpp_LIBRARY_DEBUG)
        set(YamlCpp_LIBRARIES
            optimized               ${YamlCpp_LIBRARY}
            debug                   ${YamlCpp_LIBRARY_DEBUG})
    # found only release
    elseif(YamlCpp_LIBRARY)
        set(YamlCpp_LIBRARIES    ${YamlCpp_LIBRARY})
    # found only debug
    elseif(YamlCpp_LIBRARY_DEBUG)
        set(YamlCpp_LIBRARIES    ${YamlCpp_LIBRARY_DEBUG})
    endif()
    set(YamlCpp_INCLUDE_DIRS ${YamlCpp_INCLUDE_DIR})

    if(NOT YAMLCPP_USE_STATIC_LIBS)
        add_definitions(-DYAML_CPP_DLL)
    endif()
endif()

mark_as_advanced(
    YamlCpp_INCLUDE_DIRS
    YamlCpp_LIBRARIES
)
