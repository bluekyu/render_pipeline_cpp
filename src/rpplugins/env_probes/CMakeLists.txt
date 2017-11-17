# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.9)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID}
    DESCRIPTION "Environment probes plugin"
)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

set(RPPLUGIN_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGIN_ID}")

set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# === plugin specific packages ===

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/env_probes_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/apply_envprobes_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/apply_envprobes_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/cull_probes_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/cull_probes_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/env_probes_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/environment_capture_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/environment_capture_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/environment_probe.cpp"
    "${PROJECT_SOURCE_DIR}/src/environment_probe.hpp"
    "${PROJECT_SOURCE_DIR}/src/probe_manager.cpp"
    "${PROJECT_SOURCE_DIR}/src/probe_manager.hpp"
)
include("../rpplugin_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE rpplugins::pssm)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
