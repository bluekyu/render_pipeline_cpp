# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.10.2...3.12)
project(render_pipeline
    VERSION 0.7.0
    DESCRIPTION "Render Pipeline C++ project"
    LANGUAGES CXX
)

# === configure ====================================================================================
option(${PROJECT_NAME}_ENABLE_RTTI "Enable Run-Time Type Information" OFF)
option(${PROJECT_NAME}_BUILD_RPASSIMP "Build rpassimp plugin for Panda3D" ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
include(git-utils)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# version
set(${PROJECT_NAME}_SEMANTIC_VERSION "${${PROJECT_NAME}_VERSION}")
message("[${PROJECT_NAME}] build version: ${${PROJECT_NAME}_SEMANTIC_VERSION}")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    # git commit
    get_git_head_hash(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME}_GIT_COMMIT)
    message("[${PROJECT_NAME}] git commit: ${${PROJECT_NAME}_GIT_COMMIT}")
else()
    set(${PROJECT_NAME}_GIT_COMMIT "")
endif()

# === project specific packages ===
find_package(panda3d REQUIRED p3framework p3direct)
find_package(Boost REQUIRED filesystem)

find_package(fmt CONFIG REQUIRED)
if(TARGET fmt::fmt-header-only)
    set(FMT_TARGET fmt::fmt-header-only)
else()                                      # for ubuntu libfmt-dev package
    set(FMT_TARGET fmt::fmt)
endif()

find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

# find freetype
find_package(Freetype REQUIRED)
if(FREETYPE_FOUND)
    message(STATUS "freetype include path: ${FREETYPE_INCLUDE_DIRS}")
endif()

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE})
# ==================================================================================================

# ==================================================================================================
function(render_pipeline_generate_version_hpp)
    set(dest_file "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}/render_pipeline/rpcore/version.hpp")
    configure_file("${PROJECT_SOURCE_DIR}/src/rpcore/version.hpp.in" "${dest_file}" @ONLY)
    target_sources(${PROJECT_NAME} PRIVATE "${dest_file}")
endfunction()
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
add_library(${PROJECT_NAME} SHARED ${render_pipeline_sources} ${render_pipeline_headers})
render_pipeline_generate_version_hpp()

if(MSVC)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251 /wd4275 /utf-8
        $<$<NOT:$<BOOL:${${PROJECT_NAME}_ENABLE_RTTI}>>:/GR->
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/permissive->

        # note: windows.cmake in vcpkg
        $<$<CONFIG:Release>:/Oi /Gy /Z7>
    )
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
        $<$<NOT:$<BOOL:${${PROJECT_NAME}_ENABLE_RTTI}>>:-fno-rtti>
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE

    # Visual Studio 2017 15.8 fixes align bug, which needs macro definition:
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},19.15>>:_ENABLE_EXTENDED_ALIGNED_STORAGE=1>

    # used for plugin postfix.
    $<$<CONFIG:Debug>:RENDER_PIPELINE_BUILD_CFG_POSTFIX="${${PROJECT_NAME}_DEBUG_POSTFIX}">
    $<$<CONFIG:RelWithDebInfo>:RENDER_PIPELINE_BUILD_CFG_POSTFIX="${${PROJECT_NAME}_RELWITHDEBINFO_POSTFIX}">
)

target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

    PRIVATE "${PROJECT_SOURCE_DIR}/src"
    ${FREETYPE_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Boost::boost panda3d::p3framework panda3d::p3direct

    PRIVATE $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::filesystem ${FREETYPE_LIBRARIES} ${FMT_TARGET} yaml-cpp spdlog::spdlog

    $<$<PLATFORM_ID:Windows>:Shlwapi.lib>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "render_pipeline"
    DEFINE_SYMBOL "RENDER_PIPELINE_BUILD"
    DEBUG_POSTFIX ${${PROJECT_NAME}_DEBUG_POSTFIX}
    RELWITHDEBINFO_POSTFIX ${${PROJECT_NAME}_RELWITHDEBINFO_POSTFIX}
    VERSION ${PROJECT_VERSION}
)

# === configure package ===
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake")
set(${PROJECT_NAME}_TARGET_NAMESPACE "${PROJECT_NAME}::")
set(${PROJECT_NAME}_TARGET_EXPORT_NAME "${PACKAGE_NAME}-targets")
set(PACKAGE_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}")

include(CMakePackageConfigHelpers)
if(CMAKE_VERSION VERSION_LESS "3.11")
    write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY SameMajorVersion
    )
else()
    write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY SameMinorVersion
    )
endif()

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/${PACKAGE_NAME}-config.cmake.in"
    ${PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)

add_library(${${PROJECT_NAME}_TARGET_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})
# ==================================================================================================

# === install ======================================================================================
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${PACKAGE_NAME})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${${PROJECT_NAME}_TARGET_EXPORT_NAME}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
export(EXPORT ${${PROJECT_NAME}_TARGET_EXPORT_NAME}
    NAMESPACE ${${PROJECT_NAME}_TARGET_NAMESPACE}
    FILE "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TARGET_EXPORT_NAME}.cmake"
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/config" DESTINATION "${render_pipeline_DATA_DIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/data" DESTINATION "${render_pipeline_DATA_DIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/effects" DESTINATION "${render_pipeline_DATA_DIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/rpcore" DESTINATION "${render_pipeline_DATA_DIR}")

install(FILES ${PACKAGE_CONFIG_FILE} ${PACKAGE_VERSION_CONFIG_FILE} ${${PROJECT_NAME}_MACRO_CMAKE_FILE}
    DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)
install(EXPORT ${${PROJECT_NAME}_TARGET_EXPORT_NAME}
    NAMESPACE ${${PROJECT_NAME}_TARGET_NAMESPACE}
    DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md" "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/"
)

if(MSVC)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
# ==================================================================================================

# === subdirectory =================================================================================
add_subdirectory("${PROJECT_SOURCE_DIR}/src/rpplugins")

if(${${PROJECT_NAME}_BUILD_RPASSIMP})
    add_subdirectory("${PROJECT_SOURCE_DIR}/src/rpassimp")
endif()
# ==================================================================================================
