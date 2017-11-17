# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.9)

# === configure ====================================================================================
# common variable
set(RPPLUGIN_CONFIG_IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpplugin-config.cmake.in")

set(render_pipeline_PLUGINS "ao" "bloom" "clouds" "color_correction" "dof" "forward_shading"
    "motion_blur" "volumetrics" "scattering" "pssm" "env_probes" "skin_shading" "sky_ao" "smaa"
    "ssr" "vxgi"
)

# subdirectory
foreach(plugin_id ${render_pipeline_PLUGINS})
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${plugin_id}")
endforeach()
# ==================================================================================================
