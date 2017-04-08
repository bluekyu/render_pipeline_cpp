#.rst:
# FindRenderPipeline
# --------------
#
# FindRenderPipeline.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   RenderPipeline_INCLUDE_DIRS     - Include directories for render-pipeline
#   RenderPipeline_LIBRARIES        - Libraries to link against render-pipeline
#   RenderPipeline_FOUND            - True if render-pipeline has been found and can be used
#   RenderPipeline_VERSION          - Library version for render-pipeline
#   RenderPipeline_VERSION_STRING   - Library version for render-pipeline
#   RenderPipeline_VERSION_MAJOR    - Library major version for render-pipeline
#   RenderPipeline_VERSION_MINOR    - Library minor version for render-pipeline
#   RenderPipeline_VERSION_PATCH    - Library patch version for render-pipeline
#
# and the following imported targets::
#
#   RenderPipeline::RenderPipeline  - The render-pipeline library

cmake_minimum_required(VERSION 3.2)

find_path(RenderPipeline_INCLUDE_DIR
    NAMES "render_pipeline/rpcore/config.h"
    HINTS ${RenderPipeline_INCLUDEDIR} "${RenderPipeline_ROOT}/include"
)
find_path(RenderPipeline_INCLUDE_PLUGIN_DIR
    NAMES "render_pipeline/rpplugins"
    HINTS ${RenderPipeline_INCLUDEDIR} "${RenderPipeline_ROOT}/share"
)
find_library(RenderPipeline_LIBRARY
    NAMES render_pipeline
    HINTS ${RenderPipeline_LIBRARYDIR} "${RenderPipeline_ROOT}/lib"
)
find_library(RenderPipeline_LIBRARY_DEBUG
    NAMES render_pipelined
    HINTS ${RenderPipeline_LIBRARYDIR} "${RenderPipeline_ROOT}/lib"
)

# Set RenderPipeline_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RenderPipeline
    FOUND_VAR RenderPipeline_FOUND
    REQUIRED_VARS RenderPipeline_LIBRARY RenderPipeline_INCLUDE_DIR
    VERSION_VAR RenderPipeline_VERSION
)

if(RenderPipeline_FOUND)
    # found both
    if(RenderPipeline_LIBRARY AND RenderPipeline_LIBRARY_DEBUG)
        set(RenderPipeline_LIBRARIES
            optimized               ${RenderPipeline_LIBRARY}
            debug                   ${RenderPipeline_LIBRARY_DEBUG})
    # found only release
    elseif(RenderPipeline_LIBRARY)
        set(RenderPipeline_LIBRARIES    ${RenderPipeline_LIBRARY})
    # found only debug
    elseif(RenderPipeline_LIBRARY_DEBUG)
        set(RenderPipeline_LIBRARIES    ${RenderPipeline_LIBRARY_DEBUG})
    endif()
    set(RenderPipeline_INCLUDE_DIRS ${RenderPipeline_INCLUDE_DIR} ${RenderPipeline_INCLUDE_PLUGIN_DIR})
endif()

mark_as_advanced(
    RenderPipeline_INCLUDE_DIRS
    RenderPipeline_LIBRARIES
)
