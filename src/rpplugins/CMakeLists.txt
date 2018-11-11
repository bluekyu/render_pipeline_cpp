# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)

# === configure ====================================================================================
# common variable
set(RPPLUGINS_CONFIG_IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpplugins-config.cmake.in")

set(render_pipeline_optional_PLUGINS
    "imgui"
    "rpstat"
)

foreach(plugin_id ${render_pipeline_optional_PLUGINS})
    option(rpcpp_plugins_BUILD_${plugin_id} "Enable to build '${plugin_id}'" OFF)
    if(rpcpp_plugins_BUILD_${plugin_id})
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${plugin_id}")
    endif()
endforeach()

set(render_pipeline_PLUGINS "ao" "bloom" "clouds" "color_correction" "dof" "forward_shading"
    "motion_blur" "volumetrics" "scattering" "pssm" "env_probes" "skin_shading" "sky_ao" "smaa"
    "ssr" "vxgi"
)

# subdirectory
foreach(plugin_id ${render_pipeline_PLUGINS})
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${plugin_id}")
endforeach()
# ==================================================================================================
