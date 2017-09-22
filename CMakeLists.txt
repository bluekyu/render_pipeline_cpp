# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.8)
project(render_pipeline)

# === configure ====================================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(debugging-information)
include(git-utils)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
    message(STATUS "Windows 10 Target Platform: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
endif()

# version
set(render_pipeline_VERSION "0.3.1")
set(render_pipeline_SEMANTIC_VERSION "${render_pipeline_VERSION}")
set(PROJECT_VERSION "${render_pipeline_SEMANTIC_VERSION}")
message("[render_pipeline] build version: ${render_pipeline_SEMANTIC_VERSION}")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    # git commit
    get_git_head_hash(${CMAKE_CURRENT_SOURCE_DIR} render_pipeline_GIT_COMMIT)
    message("[render_pipeline] git commit: ${render_pipeline_GIT_COMMIT}")
else()
    set(render_pipeline_GIT_COMMIT "")
endif()

# configure project package
include(FindPackages)

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
find_package(Freetype REQUIRED)
if(FREETYPE_FOUND)
    message(STATUS "freetype include path: ${FREETYPE_INCLUDE_DIRS}")
endif()

# find flatbuffers
if(NOT TARGET flatbuffers)
    find_package(FlatBuffers REQUIRED)
endif()

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/render_pipeline-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE})
# ==================================================================================================

# ==================================================================================================
function(render_pipeline_generate_schema_headers target_)
    # generate schema headers
    if(NOT FlatBuffers_flatc)
        message(FATAL_ERROR "Cannot find flatc program.")
    endif()

    set(schema_output_dir "${PROJECT_BINARY_DIR}/rpcore/schema")
    file(GLOB flatc_input_files "${PROJECT_SOURCE_DIR}/resources/data/schema/*.fbs")
    foreach(input_file ${flatc_input_files})
        get_filename_component(input_file_name ${input_file} NAME_WE)

        set(schema_header_file "${schema_output_dir}/${input_file_name}_generated.h")
        add_custom_command(OUTPUT ${schema_header_file}
            COMMAND ${FlatBuffers_flatc} -c --scoped-enums -o "${schema_output_dir}" "${input_file}"
            DEPENDS "${input_file}"
        )

        target_sources(${target_} PRIVATE ${schema_header_file})
    endforeach()
endfunction()

function(render_pipeline_generate_version_hpp)
    configure_file("${PROJECT_SOURCE_DIR}/src/rpcore/version.hpp.in"
        "${PROJECT_BINARY_DIR}/rpcore/version.hpp"
        @ONLY
    )
    target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}/rpcore/version.hpp")
endfunction()
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
add_library(${PROJECT_NAME} SHARED ${render_pipeline_sources} ${render_pipeline_headers})
render_pipeline_generate_schema_headers(${PROJECT_NAME})
render_pipeline_generate_version_hpp()

if(MSVC)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_final cxx_strong_enums cxx_delegating_constructors
        PRIVATE $<$<VERSION_GREATER:${MSVC_VERSION},1800>:cxx_generic_lambdas>
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/utf-8 /std:c++14>
    )
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_final cxx_strong_enums cxx_delegating_constructors
        PRIVATE cxx_std_17)
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
    Boost::filesystem ${FREETYPE_LIBRARIES} yaml-cpp spdlog::spdlog
    FlatBuffers::FlatBuffers

    $<$<PLATFORM_ID:Windows>:Shlwapi.lib>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "render_pipeline"
    DEFINE_SYMBOL "RENDER_PIPELINE_BUILD"
    DEBUG_POSTFIX ${${PROJECT_NAME}_DEBUG_POSTFIX}
    RELWITHDEBINFO_POSTFIX ${${PROJECT_NAME}_RELWITHDEBINFO_POSTFIX}
    VERSION ${PROJECT_VERSION}
)

configure_debugging_information(${PROJECT_NAME})

# === configure package ===
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake")
set(${PROJECT_NAME}_TARGET_NAMESPACE "render_pipeline::")
set(${PROJECT_NAME}_TARGET_EXPORT_NAME "${PACKAGE_NAME}-targets")
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

add_library(${${PROJECT_NAME}_TARGET_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})
# ==================================================================================================

# === install ======================================================================================
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${PACKAGE_NAME})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${${PROJECT_NAME}_TARGET_EXPORT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)
export(EXPORT ${${PROJECT_NAME}_TARGET_EXPORT_NAME}
    NAMESPACE ${${PROJECT_NAME}_TARGET_NAMESPACE}
    FILE "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TARGET_EXPORT_NAME}.cmake"
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}" DESTINATION "include/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/config" DESTINATION "${render_pipeline_DATA_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/data" DESTINATION "${render_pipeline_DATA_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/effects" DESTINATION "${render_pipeline_DATA_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/rpcore" DESTINATION "${render_pipeline_DATA_DIR}/")

install(FILES ${PACKAGE_CONFIG_FILE} ${PACKAGE_VERSION_CONFIG_FILE} ${${PROJECT_NAME}_MACRO_CMAKE_FILE}
    DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)
install(EXPORT ${${PROJECT_NAME}_TARGET_EXPORT_NAME}
    NAMESPACE ${${PROJECT_NAME}_TARGET_NAMESPACE}
    DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md" "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    DESTINATION "share/${PROJECT_NAME}/"
)

install_debugging_information(${PROJECT_NAME})

# documents
if(render_pipeline_BUILD_DOCUMENTS AND DOXYGEN_FOUND)
    set(DOCUMENTS_TARGET "doxygen")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        @ONLY
    )

    add_custom_target(${DOCUMENTS_TARGET} ALL
        ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_target_properties(${DOCUMENTS_TARGET} PROPERTIES FOLDER "render_pipeline")

    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTS_TARGET}/" DESTINATION "share/${PROJECT_NAME}/documents/")
endif()
# ==================================================================================================

# === subdirectory =================================================================================
add_subdirectory("${PROJECT_SOURCE_DIR}/src/rpplugins")
# ==================================================================================================
