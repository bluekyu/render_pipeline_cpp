# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.10.2)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID}
    VERSION 0.1.0
    DESCRIPTION "Color correction plugin"
    LANGUAGES CXX
)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

set(RPPLUGIN_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGIN_ID}")

# === plugin specific packages ===

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
set(${PROJECT_NAME}_sources
    "${PROJECT_SOURCE_DIR}/src/plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/auto_exposure_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/auto_exposure_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/color_correction_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/color_correction_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/manual_exposure_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/manual_exposure_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/sharpen_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/sharpen_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/tonemapping_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/tonemapping_stage.hpp"
)
include("../rpplugin_build.cmake")
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
