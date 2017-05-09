# Author: Younguk Kim (bluekyu)
# Date  : 2017-01-25

cmake_minimum_required(VERSION 3.6)
project(render_pipeline)

# === configure ====================================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
    message(STATUS "Windows 10 Target Platform: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
endif()

# version
set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# configure project package
include(FindPackages)

# subdirectory
add_subdirectory("${PROJECT_SOURCE_DIR}/src/rpplugins")

# === project specific packages ===
# find spdlog
find_package(spdlog REQUIRED)

# find yaml-cpp
find_package(yaml-cpp REQUIRED)

# find freetype
find_package(FreeType REQUIRED)
if(FREETYPE_FOUND)
    message(STATUS "freetype include path: ${FREETYPE_INCLUDE_DIRS}")
endif()
# ================================

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release."
            FORCE)
    endif()
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
endif()

# set input files
include("${PROJECT_SOURCE_DIR}/files.cmake")
# ==================================================================================================

# === target =======================================================================================
add_library(${PROJECT_NAME} SHARED ${render_pipeline_sources} ${render_pipeline_headers})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_final PRIVATE cxx_auto_type)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:share>

    PRIVATE "${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${FREETYPE_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Boost::boost panda3d::panda3d

    PRIVATE $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::filesystem Boost::system
    ${FREETYPE_LIBRARIES} yaml-cpp spdlog::spdlog
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    DEFINE_SYMBOL "RPCPP_BUILD"
    DEBUG_POSTFIX "-debug"
    RELWITHDEBINFO_POSTFIX "-reldeb"
    VERSION ${PROJECT_VERSION}
)

# configure package
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake")
set(TARGET_EXPORT_NAME "${PROJECT_NAME}-targets")
set(PACKAGE_CMAKE_INSTALL_DIR "lib/${PROJECT_NAME}/cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
configure_package_config_file("${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}-config.cmake.in"
    ${PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR})
export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_BINARY_DIR}/${TARGET_EXPORT_NAME}.cmake)
# ==================================================================================================

# === install ======================================================================================
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${TARGET_EXPORT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)

install(FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.pdb"
    DESTINATION "bin"
    CONFIGURATIONS Debug RelWithDebInfo
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}" DESTINATION "include/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/config/" DESTINATION "etc/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/data" DESTINATION "share/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/effects" DESTINATION "share/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/rpcore" DESTINATION "share/${PROJECT_NAME}/")

install(FILES ${PACKAGE_CONFIG_FILE} ${PACKAGE_VERSION_CONFIG_FILE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR})
install(EXPORT ${TARGET_EXPORT_NAME} DESTINATION "lib/${PROJECT_NAME}/cmake")

# documents
if(RPCPP_BUILD_DOCUMENTS AND DOXYGEN_FOUND)
    set(DOCUMENTS_TARGET "documents")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        @ONLY
    )

    add_custom_target(${DOCUMENTS_TARGET} ALL
        ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTS_TARGET}/" DESTINATION "share/${PROJECT_NAME}/doc/")
endif()

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md"
    DESTINATION "share/${PROJECT_NAME}/")
# ==================================================================================================
