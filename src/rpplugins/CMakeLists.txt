# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)

# === configure ====================================================================================
# rpplugins
set(RPPLUGINS_FOLDER_NAME "rpplugins")

# subdirectory
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ao")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/bloom")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/clouds")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/color_correction")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dof")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/env_probes")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/forward_shading")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/motion_blur")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pssm")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/scattering")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/skin_shading")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/sky_ao")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/smaa")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ssr")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/volumetrics")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vxgi")

# optional plugin
option(RPCPP_PLUGIN_BUILD_OPENVR    "Build OpenVR Plugin" OFF)
if(RPCPP_PLUGIN_BUILD_OPENVR)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/openvr")
endif()
# ==================================================================================================
