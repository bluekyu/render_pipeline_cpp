#.rst:
# Findpanda3d
# --------------
#
# Findpanda3d.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-19
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   panda3d_FOUND            - True if panda3d has been found and can be used
#
# The following `IMPORTED` targets are also defined::
#
#   panda3d::panda3d    - Target for the Panda3D necessary libraries.
#   panda3d::<C>        - Target for specific Panda3D component.

cmake_minimum_required(VERSION 3.6)

find_path(panda3d_INCLUDE_DIR
    NAMES "pandaFramework.h"
    PATHS "${panda3d_ROOT}"
    PATH_SUFFIXES include
)

set(panda3d_DEFAULT_COMPONENTS p3framework panda pandaexpress p3dtool p3dtoolconfig p3direct p3interrogatedb)

list(APPEND panda3d_FIND_COMPONENTS ${panda3d_DEFAULT_COMPONENTS})
list(REMOVE_DUPLICATES panda3d_FIND_COMPONENTS)

set(panda3d_NON_DEFAULT_COMPONENTS ${panda3d_FIND_COMPONENTS})
list(REMOVE_ITEM panda3d_NON_DEFAULT_COMPONENTS ${panda3d_DEFAULT_COMPONENTS})

foreach(COMPONENT_NAME ${panda3d_FIND_COMPONENTS})
    find_library(panda3d_${COMPONENT_NAME}_LIBRARY
        NAMES lib${COMPONENT_NAME}
        PATHS "${panda3d_ROOT}"
        PATH_SUFFIXES lib
    )
    list(APPEND panda3d_LIBRARY ${panda3d_${COMPONENT_NAME}_LIBRARY})
endforeach()
#find_library(panda3d_LIBRARY_DEBUG
#    NAMES ${panda3d_FIND_COMPONENTS}
#    HINTS ${panda3d_LIBRARYDIR}
#    PATH_SUFFIXES lib64 lib
#)

# Set panda3d_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(panda3d
    FOUND_VAR panda3d_FOUND
    REQUIRED_VARS panda3d_LIBRARY panda3d_INCLUDE_DIR
)

if(panda3d_FOUND)
    message(STATUS "Found the following Panda3D libraries:")
    foreach(COMPONENT ${panda3d_FIND_COMPONENTS})
        message (STATUS "  ${COMPONENT}")
    endforeach()

    # create targets of found components
    foreach(COMPONENT_NAME ${panda3d_FIND_COMPONENTS})
        if(NOT TARGET panda3d::${COMPONENT_NAME})
            add_library(panda3d::${COMPONENT_NAME} UNKNOWN IMPORTED)
            set_target_properties(panda3d::${COMPONENT_NAME} PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${panda3d_INCLUDE_DIR}"
                IMPORTED_LOCATION "${panda3d_${COMPONENT_NAME}_LIBRARY}"
                IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            )

            # Make variables changeable to the advanced user
            mark_as_advanced(panda3d_${COMPONENT_NAME}_LIBRARY)
        endif()
    endforeach()

    # create interface target
    if(NOT TARGET panda3d::panda3d)
        add_library(panda3d::panda3d INTERFACE IMPORTED)

        foreach(COMPONENT_NAME ${panda3d_DEFAULT_COMPONENTS})
            list(APPEND _panda3d_DEFAULT_TARGET_DEPENDENCIES panda3d::${COMPONENT_NAME})
        endforeach()

        set_target_properties(panda3d::panda3d PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${panda3d_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${_panda3d_DEFAULT_TARGET_DEPENDENCIES}"
        )

        mark_as_advanced(panda3d_INCLUDE_DIR)
    endif()
endif()
