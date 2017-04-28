# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(openvr)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === plugin specific packages ===
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# find OpenVR
set(OPENVR_ROOT "" CACHE PATH "Hint for finding OpenVR root directory")
set(OPENVR_INCLUDEDIR "" CACHE PATH "Hint for finding OpenVR include directory")
set(OPENVR_LIBRARYDIR "" CACHE PATH "Hint for finding OpenVR library directory")

find_package(OpenVR REQUIRED)
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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif()
endif()

# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/openvr_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/openvr_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/openvr_render_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/openvr_render_stage.h")
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
    ${OpenVR_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline ${OpenVR_LIBRARIES}
)

include(plugin_install)
# ==================================================================================================
