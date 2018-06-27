# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.10.2)

get_filename_component(RPPLUGINS_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugins_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "Sky occlusion plugin"
    LANGUAGES CXX
)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

set(RPPLUGINS_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGINS_ID}")

# === plugin specific packages ===

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
set(${PROJECT_NAME}_sources
    "${PROJECT_SOURCE_DIR}/src/sky_ao_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/sky_ao_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/ao_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/ao_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/capture_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/capture_stage.cpp"
)
include("../rpplugins_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE rpplugins::env_probes)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugins_install.cmake")
# ==================================================================================================
