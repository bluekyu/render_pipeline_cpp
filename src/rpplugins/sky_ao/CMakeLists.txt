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
render_pipeline_FIND_PLUGINS("env_probes")
# ==================================================================================================

# === target =======================================================================================
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/src/sky_ao_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/sky_ao_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/ao_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/ao_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/capture_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/capture_stage.cpp"
)
include("../rpplugin_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE rpplugin::env_probes)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
