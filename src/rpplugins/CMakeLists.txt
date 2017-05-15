# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)

# === configure ====================================================================================
# common variable
set(RPPLUGIN_CONFIG_IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpplugin-config.cmake.in")

set(RENDER_PIPELINE_PLUGINS "ao" "bloom" "clouds" "color_correction" "dof" "env_probes" "forward_shading"
    "motion_blur" "pssm" "scattering" "skin_shading" "sky_ao" "smaa" "ssr" "volumetrics" "vxgi"
)

# optional plugin
option(RENDER_PIPELINE_PLUGIN_BUILD_OPENVR    "Build OpenVR Plugin" OFF)
if(RENDER_PIPELINE_PLUGIN_BUILD_OPENVR)
    list(APPEND RENDER_PIPELINE_PLUGINS "openvr")
endif()

# subdirectory
foreach(plugin_id ${RENDER_PIPELINE_PLUGINS})
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${plugin_id}")
endforeach()
# ==================================================================================================
