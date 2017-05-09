# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.6)

# === configure ====================================================================================
# rpplugins
set(RPPLUGINS_FOLDER_NAME "rpplugins")

set(RPCPP_PLUGINS "ao" "bloom" "clouds" "color_correction" "dof" "env_probes" "forward_shading"
    "motion_blur" "pssm" "scattering" "skin_shading" "sky_ao" "smaa" "ssr" "volumetrics" "vxgi")

# optional plugin
option(RPCPP_PLUGIN_BUILD_OPENVR    "Build OpenVR Plugin" OFF)
if(RPCPP_PLUGIN_BUILD_OPENVR)
    list(APPEND RPCPP_PLUGINS "openvr")
endif()

# subdirectory
foreach(plugin_id ${RPCPP_PLUGINS})
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${plugin_id}")
endforeach()
# ==================================================================================================
