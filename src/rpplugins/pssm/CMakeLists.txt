# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(pssm)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === plugin specific packages ===
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Panda3d_INCLUDE_DIRS})
# ================================

# configure plugin
set(LIB_NAME "plugin")
set(LIB_TYPE MODULE)
add_definitions(-DRPCPP_PLUGIN_ID_STRING="${PROJECT_NAME}")

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release."
            FORCE)
    endif()
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif()
else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

# subdirectory
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}")

# include header directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")      # /
include_directories("${PROJECT_BINARY_DIR}")            # /build
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")  # ./src
include_directories("${RP_PUBLIC_HEADER_DIR}/..")      # /render_pipeline/..

# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/pssm_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/pssm_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/pssm_dist_shadow_stage.h"
    "${PROJECT_SOURCE_DIR}/src/pssm_dist_shadow_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/pssm_scene_shadow_stage.h"
    "${PROJECT_SOURCE_DIR}/src/pssm_scene_shadow_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/pssm_shadow_stage.h"
    "${PROJECT_SOURCE_DIR}/src/pssm_shadow_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/pssm_stage.h"
    "${PROJECT_SOURCE_DIR}/src/pssm_stage.cpp")
# ==================================================================================================

# === build & install===============================================================================
# create library
include(plugin_build)
# ==================================================================================================
