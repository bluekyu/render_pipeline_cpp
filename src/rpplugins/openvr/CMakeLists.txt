# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID})

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

set(RPPLUGIN_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGIN_ID}")

set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# === plugin specific packages ===
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

if(NOT TARGET OpenVR)
    # find OpenVR
    set(OpenVR_ROOT "" CACHE PATH "Hint for finding OpenVR root directory")
    find_package(OpenVR REQUIRED)
endif()
# ==================================================================================================

# === target =======================================================================================
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/openvr_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/openvr_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/openvr_render_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/openvr_render_stage.hpp"
)
include("../rpplugin_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE OpenVR)
target_link_libraries(${RPPLUGIN_ID} INTERFACE OpenVR)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
