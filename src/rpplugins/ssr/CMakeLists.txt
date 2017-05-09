# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(ssr)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === plugin specific packages ===
# ================================

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug RelWithDebInfo Release."
            FORCE)
    endif()
    message("${PROJECT_NAME} Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd\"4251\"")
    endif()
endif()

# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/src/ssr_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/ssr_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/ssr_stage.h"
    "${PROJECT_SOURCE_DIR}/src/ssr_stage.cpp")
# ==================================================================================================

# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} MODULE ${plugin_sources})

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
