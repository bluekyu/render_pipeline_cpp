# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)

project(rpplugins_gui_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "GUI for ${RPPLUGINS_ID} library"
    LANGUAGES CXX
)

# === configure ====================================================================================
# === plugin specific packages ===
find_package(imgui CONFIG REQUIRED)
set_target_properties(imgui::imgui PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

find_package(fmt CONFIG REQUIRED)
if(TARGET fmt::fmt-header-only)                 # for libfmt in ubuntu package
    set(FMT_TARGET fmt::fmt-header-only)
else()
    set(FMT_TARGET fmt::fmt)
endif()
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("rpplugins_gui_build.cmake")

target_link_libraries(${PROJECT_NAME} PRIVATE ${FMT_TARGET})
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME} DESTINATION ${RPPLUGINS_INSTALL_DIR})

install(FILES ${${PROJECT_NAME}_MACRO_CMAKE_FILE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR} OPTIONAL)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION "${RPPLUGINS_INSTALL_DIR}")
endif()
# ==================================================================================================
