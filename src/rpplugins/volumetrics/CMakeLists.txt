# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.6)
project(volumetrics)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === plugin specific packages ===
# ================================

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug RelWithDebInfo Release."
            FORCE)
    endif()
    message("${PROJECT_NAME} Build Type: ${CMAKE_BUILD_TYPE}")
endif()

# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/volumetrics_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/include/volumetrics_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/volumetrics_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/volumetrics_stage.cpp")
# ==================================================================================================

# === target =======================================================================================
add_library(${PROJECT_NAME} MODULE ${plugin_sources})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_auto_type)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPCPP_PLUGIN_ID_STRING="${PROJECT_NAME}"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline
)

include(plugin_install)
# ==================================================================================================
