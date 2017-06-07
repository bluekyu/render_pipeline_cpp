# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(render_pipeline)

# === configure ====================================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
    message(STATUS "Windows 10 Target Platform: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
endif()

# version
file(STRINGS "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/version.hpp" _render_pipeline_VERSION_HPP REGEX "#define RENDER_PIPELINE_VERSION ")
if("${_render_pipeline_VERSION_HPP}" MATCHES "#define RENDER_PIPELINE_VERSION ([0-9]+)")
    set(render_pipeline_VERSION "${CMAKE_MATCH_1}")
endif()
unset(_render_pipeline_VERSION_HPP)

math(EXPR render_pipeline_VERSION_MAJOR "${render_pipeline_VERSION} / 10000")
math(EXPR render_pipeline_VERSION_MINOR "${render_pipeline_VERSION} / 100 % 100")
math(EXPR render_pipeline_VERSION_PATCH "${render_pipeline_VERSION} % 100")
set(render_pipeline_VERSION_TAG "")

set(render_pipeline_VERSION "${render_pipeline_VERSION_MAJOR}.${render_pipeline_VERSION_MINOR}.${render_pipeline_VERSION_PATCH}${render_pipeline_VERSION_TAG}")
set(PROJECT_VERSION "${render_pipeline_VERSION}")

message("Render Pipeline building version: ${render_pipeline_VERSION}")

# configure project package
include(FindPackages)

set(render_pipeline_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/render_pipeline-macro.cmake")
include(${render_pipeline_MACRO_CMAKE_FILE})

# === project specific packages ===
# find spdlog
if(NOT TARGET spdlog::spdlog)
    find_package(spdlog REQUIRED)
endif()

# find yaml-cpp
if(NOT TARGET yaml-cpp)
    find_package(yaml-cpp REQUIRED)
endif()

# find freetype
find_package(FreeType REQUIRED)
if(FREETYPE_FOUND)
    message(STATUS "freetype include path: ${FREETYPE_INCLUDE_DIRS}")
endif()
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
add_library(${PROJECT_NAME} SHARED ${render_pipeline_sources} ${render_pipeline_headers})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_final PRIVATE cxx_auto_type)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE

    # used for plugin postfix.
    $<$<CONFIG:Debug>:RENDER_PIPELINE_BUILD_CFG_POSTFIX="${${PROJECT_NAME}_DEBUG_POSTFIX}">
    $<$<CONFIG:RelWithDebInfo>:RENDER_PIPELINE_BUILD_CFG_POSTFIX="${${PROJECT_NAME}_RELWITHDEBINFO_POSTFIX}">
)

target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>

    PRIVATE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src"
    ${FREETYPE_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Boost::boost panda3d::panda3d

    PRIVATE $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::filesystem
    ${FREETYPE_LIBRARIES} yaml-cpp spdlog::spdlog
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "render_pipeline"
    DEFINE_SYMBOL "RENDER_PIPELINE_BUILD"
    DEBUG_POSTFIX ${${PROJECT_NAME}_DEBUG_POSTFIX}
    RELWITHDEBINFO_POSTFIX ${${PROJECT_NAME}_RELWITHDEBINFO_POSTFIX}
    VERSION ${PROJECT_VERSION}
)

# configure package
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake")
set(TARGET_EXPORT_NAME "${PACKAGE_NAME}-targets")
set(PACKAGE_CMAKE_INSTALL_DIR "lib/cmake/${PACKAGE_NAME}")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/${PACKAGE_NAME}-config.cmake.in"
    ${PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)
# ==================================================================================================

# === install ======================================================================================
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${PACKAGE_NAME})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${TARGET_EXPORT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)
export(EXPORT ${TARGET_EXPORT_NAME} FILE ${PROJECT_BINARY_DIR}/${TARGET_EXPORT_NAME}.cmake)

install(FILES "${PROJECT_BINARY_DIR}/Debug/${PROJECT_NAME}${${PROJECT_NAME}_DEBUG_POSTFIX}.pdb"
    DESTINATION "bin"
    CONFIGURATIONS Debug
)

install(FILES "${PROJECT_BINARY_DIR}/RelWithDebInfo/${PROJECT_NAME}${${PROJECT_NAME}_RELWITHDEBINFO_POSTFIX}.pdb"
    DESTINATION "bin"
    CONFIGURATIONS RelWithDebInfo
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}" DESTINATION "include/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/config" DESTINATION "${render_pipeline_DATA_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/data" DESTINATION "${render_pipeline_DATA_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/effects" DESTINATION "${render_pipeline_DATA_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/rpcore" DESTINATION "${render_pipeline_DATA_DIR}/")

install(FILES ${PACKAGE_CONFIG_FILE} ${PACKAGE_VERSION_CONFIG_FILE} ${render_pipeline_MACRO_CMAKE_FILE}
    DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)
install(EXPORT ${TARGET_EXPORT_NAME} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR})

# documents
if(render_pipeline_BUILD_DOCUMENTS AND DOXYGEN_FOUND)
    set(DOCUMENTS_TARGET "documents")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        @ONLY
    )

    add_custom_target(${DOCUMENTS_TARGET} ALL
        ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_target_properties(${DOCUMENTS_TARGET} PROPERTIES FOLDER "render_pipeline")

    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTS_TARGET}/" DESTINATION "share/${PROJECT_NAME}/doc/")
endif()

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md"
    DESTINATION "share/${PROJECT_NAME}/")
# ==================================================================================================

# === subdirectory =================================================================================
add_subdirectory("${PROJECT_SOURCE_DIR}/src/rpplugins")
# ==================================================================================================
