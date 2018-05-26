# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.9)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID}
    VERSION 0.1.0
    DESCRIPTION "Voxel Cone Tracing plugin"
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
    "${PROJECT_SOURCE_DIR}/src/vxgi_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/vxgi_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/voxelization_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/voxelization_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/vxgi_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/vxgi_stage.cpp"
)
include("../rpplugin_build.cmake")
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
