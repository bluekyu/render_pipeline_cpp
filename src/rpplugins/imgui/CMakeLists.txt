# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)

get_filename_component(RPPLUGINS_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugins_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "Plugin for imgui"
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

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("../rpplugins_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::filesystem
    ${FMT_TARGET} imgui::imgui
)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugins_install.cmake")
# ==================================================================================================
