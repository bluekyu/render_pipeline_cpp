# Author: Younguk Kim (bluekyu)
# Date  : 2017-01-25

cmake_minimum_required(VERSION 3.5)
project(render_pipeline)

# === configure ====================================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

set(RP_PUBLIC_HEADER_DIR "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}")

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

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Panda3d_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${YAML_CPP_INCLUDE_DIR})
# ================================

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release."
            FORCE)
    endif()
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd\"4251\"")
    endif()

    if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
        message(STATUS "Windows 10 Target Platform: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
    endif()
else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

# include header directories
include_directories("${RP_PUBLIC_HEADER_DIR}/..")      # /
include_directories("${PROJECT_BINARY_DIR}")            # /build
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")        # /src

# set input files
include("${PROJECT_SOURCE_DIR}/files.cmake")
# ==================================================================================================

# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} SHARED ${render_pipeline_sources} ${render_pipeline_headers})
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${Boost_LIBRARIES} ${FREETYPE_LIBRARIES} ${Panda3d_LIBRARIES} yaml-cpp spdlog::spdlog)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL "RPCPP_BUILD")

# library
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")

install(DIRECTORY ${RP_PUBLIC_HEADER_DIR} DESTINATION "include/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/config/" DESTINATION "etc/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/data" DESTINATION "share/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/effects" DESTINATION "share/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/rpcore" DESTINATION "share/${PROJECT_NAME}/")

install(EXPORT ${PROJECT_NAME}Config DESTINATION "lib/${PROJECT_NAME}/cmake")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindRenderPipeline.cmake" DESTINATION "lib/${PROJECT_NAME}/cmake")

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
