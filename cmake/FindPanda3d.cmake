#.rst:
# FindPanda3d
# --------------
#
# FindPanda3d.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   Panda3d_INCLUDE_DIRS     - Include directories for Panda3D
#   Panda3d_LIBRARIES        - Libraries to link against Panda3D
#   Panda3d_FOUND            - True if Panda3D has been found and can be used
#   Panda3d_VERSION          - Library version for Panda3D
#   Panda3d_VERSION_STRING   - Library version for Panda3D
#   Panda3d_VERSION_MAJOR    - Library major version for Panda3D
#   Panda3d_VERSION_MINOR    - Library minor version for Panda3D
#   Panda3d_VERSION_PATCH    - Library patch version for Panda3D
#
# and the following imported targets::
#
#   Panda3d::Panda3d  - The Panda3D library

cmake_minimum_required(VERSION 3.2)

find_path(Panda3d_INCLUDE_DIR
    NAMES "pandaFramework.h"
    HINTS "${PANDA3D_ROOT}/include"
)

foreach(COMPONENT_NAME ${Panda3d_FIND_COMPONENTS})
    find_library(PANDA3D_${COMPONENT_NAME}_LIBRARY
        NAMES ${COMPONENT_NAME}
        HINTS "${PANDA3D_ROOT}/lib"
    )
    list(APPEND Panda3d_LIBRARY ${PANDA3D_${COMPONENT_NAME}_LIBRARY})
endforeach()
#find_library(Panda3d_LIBRARY_DEBUG
#    NAMES ${Panda3d_FIND_COMPONENTS}
#    HINTS ${PANDA3D_LIBRARYDIR}
#    PATH_SUFFIXES lib64 lib
#)

# Set Panda3d_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Panda3d
    FOUND_VAR Panda3d_FOUND
    REQUIRED_VARS Panda3d_LIBRARY Panda3d_INCLUDE_DIR
    VERSION_VAR Panda3d_VERSION
)

if(Panda3d_FOUND)
    message(STATUS "Found the following Panda3D libraries:")
    foreach(COMPONENT ${Panda3d_FIND_COMPONENTS})
        message (STATUS "  ${COMPONENT}")
    endforeach()

    # found both
    if(Panda3d_LIBRARY AND Panda3d_LIBRARY_DEBUG)
        set(Panda3d_LIBRARIES
            optimized               ${Panda3d_LIBRARY}
            debug                   ${Panda3d_LIBRARY_DEBUG})
    # found only release
    elseif(Panda3d_LIBRARY)
        set(Panda3d_LIBRARIES    ${Panda3d_LIBRARY})
    # found only debug
    elseif(Panda3d_LIBRARY_DEBUG)
        set(Panda3d_LIBRARIES    ${Panda3d_LIBRARY_DEBUG})
    endif()
    set(Panda3d_INCLUDE_DIRS ${Panda3d_INCLUDE_DIR})
endif()

mark_as_advanced(
    Panda3d_INCLUDE_DIRS
    Panda3d_LIBRARIES
)
