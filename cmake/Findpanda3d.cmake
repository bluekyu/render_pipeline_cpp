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
# This module reads hints about search locations from variables::
#
#   panda3d_ROOT             - Preferred installation prefix
#   panda3d_ROOT_DEBUG       - Preferred Debug installation prefix (Panda3D optimization level 1-2)
#   panda3d_ROOT_RELEASE     - Preferred Release installation prefix (Panda3D optimization level 3-4)
#
# The following `IMPORTED` targets are also defined::
#
#   panda3d::panda3d    - Target for the Panda3D necessary libraries.
#   panda3d::<C>        - Target for specific Panda3D component.

cmake_minimum_required(VERSION 3.6)

if(panda3d_ROOT AND NOT panda3d_ROOT_RELEASE)
    set(panda3d_ROOT_RELEASE "${panda3d_ROOT}")
endif()

if(panda3d_ROOT AND NOT panda3d_ROOT_DEBUG)
    set(panda3d_ROOT_DEBUG "${panda3d_ROOT}")
endif()

set(panda3d_DEFAULT_COMPONENTS p3framework panda pandaexpress p3dtool p3dtoolconfig p3direct p3interrogatedb)

list(APPEND panda3d_FIND_COMPONENTS ${panda3d_DEFAULT_COMPONENTS})
list(REMOVE_DUPLICATES panda3d_FIND_COMPONENTS)

set(panda3d_NON_DEFAULT_COMPONENTS ${panda3d_FIND_COMPONENTS})
list(REMOVE_ITEM panda3d_NON_DEFAULT_COMPONENTS ${panda3d_DEFAULT_COMPONENTS})

if(WIN32)
    set(panda3d_lib_prefix "lib")
else()
    set(panda3d_lib_prefix "")
endif()
set(panda3d_lib_suffix_RELEASE "")
set(panda3d_lib_suffix_DEBUG "_d")
set(panda3d_configurations "DEBUG" "RELEASE")

foreach(configuration ${panda3d_configurations})
    find_path(panda3d_INCLUDE_DIR_${configuration}
        NAMES "pandaFramework.h"
        HINTS "${panda3d_ROOT_${configuration}}/include"
    )

    foreach(component_name ${panda3d_FIND_COMPONENTS})
        find_library(panda3d_${component_name}_LIBRARY_${configuration}
            NAMES ${panda3d_lib_prefix}${component_name}${panda3d_lib_suffix_${configuration}}
            HINTS "${panda3d_ROOT_${configuration}}"
            PATH_SUFFIXES "lib"
        )
    endforeach()
endforeach()

if(panda3d_INCLUDE_DIR_RELEASE)
    set(panda3d_INCLUDE_DIR ${panda3d_INCLUDE_DIR_RELEASE})
else()
    set(panda3d_INCLUDE_DIR ${panda3d_INCLUDE_DIR_DEBUG})
endif()

# Set panda3d_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(panda3d
    FOUND_VAR panda3d_FOUND
    REQUIRED_VARS panda3d_INCLUDE_DIR
)

if(panda3d_FOUND)
    message(STATUS "Found the following Panda3D libraries:")

    # create targets of found components
    foreach(component_name ${panda3d_FIND_COMPONENTS})
        if(NOT TARGET panda3d::${component_name})
            add_library(panda3d::${component_name} UNKNOWN IMPORTED)

            if(EXISTS "${panda3d_${component_name}_LIBRARY_RELEASE}")
                set_property(TARGET panda3d::${component_name} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
                set_property(TARGET panda3d::${component_name} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
                set_target_properties(panda3d::${component_name} PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                    IMPORTED_LOCATION "${panda3d_${component_name}_LIBRARY_RELEASE}"

                    IMPORTED_LINK_INTERFACE_LANGUAGES_${configuration} "CXX"
                    IMPORTED_LOCATION_RELEASE "${panda3d_${component_name}_LIBRARY_RELEASE}"
                    IMPORTED_LOCATION_RELWITHDEBINFO "${panda3d_${component_name}_LIBRARY_RELEASE}"
                    IMPORTED_LOCATION_MINSIZEREL "${panda3d_${component_name}_LIBRARY_RELEASE}"
                )
                message(STATUS "  ${component_name}")
            endif()

            if(EXISTS "${panda3d_${component_name}_LIBRARY_DEBUG}")
                set_property(TARGET panda3d::${component_name} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
                set_target_properties(panda3d::${component_name} PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES_${configuration} "CXX"
                    IMPORTED_LOCATION_DEBUG "${panda3d_${component_name}_LIBRARY_DEBUG}"
                )

                if(NOT EXISTS "${panda3d_${component_name}_LIBRARY_RELEASE}")
                    set_target_properties(panda3d::${component_name} PROPERTIES
                        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                        IMPORTED_LOCATION "${panda3d_${component_name}_LIBRARY_DEBUG}"
                    )
                    message(STATUS "  ${component_name}")
                endif()
            endif()

            # Make variables changeable to the advanced user
            mark_as_advanced(panda3d_${component_name}_LIBRARY_RELEASE)
            mark_as_advanced(panda3d_${component_name}_LIBRARY_DEBUG)
        endif()
    endforeach()

    # create interface target
    if(NOT TARGET panda3d::panda3d)
        add_library(panda3d::panda3d INTERFACE IMPORTED)

        foreach(component_name ${panda3d_DEFAULT_COMPONENTS})
            list(APPEND _panda3d_DEFAULT_TARGET_DEPENDENCIES panda3d::${component_name})
        endforeach()

        set_target_properties(panda3d::panda3d PROPERTIES
            INTERFACE_LINK_LIBRARIES "${_panda3d_DEFAULT_TARGET_DEPENDENCIES}"
        )

        if(panda3d_INCLUDE_DIR_DEBUG AND panda3d_INCLUDE_DIR_RELEASE)
            set_target_properties(panda3d::panda3d PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES $<IF:$<CONFIG:DEBUG>,${panda3d_INCLUDE_DIR_DEBUG},${panda3d_INCLUDE_DIR_RELEASE}>
            )
        else()
            set_target_properties(panda3d::panda3d PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES ${panda3d_INCLUDE_DIR}
            )
        endif()

        mark_as_advanced(panda3d_INCLUDE_DIR)
        mark_as_advanced(panda3d_INCLUDE_DIR_DEBUG)
        mark_as_advanced(panda3d_INCLUDE_DIR_RELEASE)
    endif()
endif()
