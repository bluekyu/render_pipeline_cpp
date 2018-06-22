# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.10.2)
project(rpassimp
    VERSION ${render_pipeline_VERSION}
    DESCRIPTION "Assimp File Loader for Render Pipeline"
    LANGUAGES CXX
)

# === configure ====================================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
include(debugging-information)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === project specific packages ===
find_package(assimp CONFIG REQUIRED)
if(NOT ASSIMP_LIBRARIES)
    message(FATAL_ERROR
        "  \n"
        "  '${PROJECT_NAME}' project requires 'assimp' package.\n"
        "  Set OFF to 'render_pipeline_BUILD_RPASSIMP' option If you want to disable this.\n"
    )
endif()

find_package(fmt CONFIG REQUIRED)
if(TARGET fmt::fmt-header-only)
    set(FMT_TARGET fmt::fmt-header-only)
else()                                      # for ubuntu libfmt-dev package
    set(FMT_TARGET fmt::fmt)
endif()
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
add_library(${PROJECT_NAME} MODULE ${${PROJECT_NAME}_sources} ${${PROJECT_NAME}_headers})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251 /wd4275 /utf-8
        $<$<NOT:$<BOOL:${${PROJECT_NAME}_ENABLE_RTTI}>>:/GR->
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/permissive->

        # note: windows.cmake in vcpkg
        $<$<CONFIG:Release>:/Oi /Gy /Z7>
    )
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
        $<$<NOT:$<BOOL:${${PROJECT_NAME}_ENABLE_RTTI}>>:-fno-rtti>
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
    PRIVATE $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    render_pipeline::render_pipeline
    ${FMT_TARGET} ${ASSIMP_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "render_pipeline"
    PREFIX "lib"
    DEBUG_POSTFIX "_d"
    VERSION ${PROJECT_VERSION}
)

configure_debugging_information(TARGET ${PROJECT_NAME} CONFIGURATIONS Debug RelWithDebInfo Release)
# ==================================================================================================

# === install ======================================================================================
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${PACKAGE_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_BINDIR}")

install_debugging_information(TARGET ${PROJECT_NAME}
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    CONFIGURATIONS Debug RelWithDebInfo Release
)
# ==================================================================================================
