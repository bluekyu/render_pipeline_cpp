# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)

get_filename_component(RPPLUGINS_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugins_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "Plugin for rpstat"
    LANGUAGES CXX
)

# === configure ====================================================================================
set(RPPLUGINS_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGINS_ID}")

# === plugin specific packages ===
find_package(Boost REQUIRED filesystem)

find_package(fmt CONFIG REQUIRED)
if(TARGET fmt::fmt-header-only)                 # for libfmt in ubuntu package
    set(FMT_TARGET fmt::fmt-header-only)
else()
    set(FMT_TARGET fmt::fmt)
endif()

find_package(imgui CONFIG REQUIRED)
set_target_properties(imgui::imgui PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

render_pipeline_find_plugins("imgui")
if(NOT TARGET rpplugins::imgui)
    message(STATUS "  ${PROJECT_NAME} project will be disabled.\n")
    return()
endif()

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("../rpplugins_build.cmake")
target_compile_definitions(${PROJECT_NAME} PRIVATE
    # used for GUI plugin postfix.
    $<$<CONFIG:Debug>:RENDER_PIPELINE_BUILD_CFG_POSTFIX="${render_pipeline_DEBUG_POSTFIX}">
    $<$<CONFIG:RelWithDebInfo>:RENDER_PIPELINE_BUILD_CFG_POSTFIX="${render_pipeline_RELWITHDEBINFO_POSTFIX}">
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::filesystem
    ${FMT_TARGET} imgui::imgui

    rpplugins::imgui
)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugins_install.cmake")
# ==================================================================================================
