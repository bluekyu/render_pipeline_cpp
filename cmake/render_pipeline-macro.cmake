# Author: Younguk Kim (bluekyu)

# render pipeline variable
set(render_pipeline_DATA_DIR "share/render_pipeline" CACHE INTERNAL
    "Relative path of data directory in Render Pipeline"
)
set(render_pipeline_PLUGIN_DIR "${render_pipeline_DATA_DIR}/rpplugins" CACHE INTERNAL
    "Relative path of plugin directory in Render Pipeline"
)
set(render_pipeline_DEBUG_POSTFIX "-debug" CACHE INTERNAL
    "Postfix string used for the name of plugin of Debug mode in Render Pipeline"
)
set(render_pipeline_RELWITHDEBINFO_POSTFIX "-reldeb" CACHE INTERNAL
    "Postfix string used for the name of plugin of RelWithDebInfo mode in Render Pipeline"
)

# help function to find rpplugin
function(render_pipeline_find_plugins)
    cmake_parse_arguments(ARG "REQUIRED" "" "" ${ARGN})
    set(plugin_id_list ${ARG_UNPARSED_ARGUMENTS})
    if(${ARG_REQUIRED})
        set(plugin_required REQUIRED)
    endif()

    set(missed_plugin_id_list "")
    set(found_plugin_id_list "")
    foreach(plugin_id ${plugin_id_list})
        if(NOT TARGET rpplugins::${plugin_id})
            list(APPEND missed_plugin_id_list ${plugin_id})
        endif()
    endforeach()

    if(missed_plugin_id_list)
        if(render_pipeline_INSTALL_DIR)
            set(PLUGIN_DIR_HINT "${render_pipeline_INSTALL_DIR}/${render_pipeline_PLUGIN_DIR}")
        endif()

        foreach(plugin_id ${missed_plugin_id_list})
            find_package(rpplugins_${plugin_id} CONFIG ${plugin_required} HINTS "${PLUGIN_DIR_HINT}/${plugin_id}")
            if(TARGET rpplugins::${plugin_id})
                list(APPEND found_plugin_id_list ${plugin_id})
            else()
                message(STATUS "  Failed to find '${plugin_id}' plugin.\n")
            endif()
        endforeach()
    endif()

    if(found_plugin_id_list)
        message(STATUS "Found the following Render Pipeline plugins:")
        foreach(plugin_id ${found_plugin_id_list})
            message(STATUS "  ${plugin_id}")
        endforeach()
    endif()
endfunction()

# helper to check install directory
function(render_pipeline_check_install_directory)
    if(NOT DEFINED render_pipeline_INSTALL_DIR)
        return()
    endif()

    get_filename_component(_render_pipeline_INSTALL_DIR ${render_pipeline_INSTALL_DIR} ABSOLUTE)
    get_filename_component(_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} ABSOLUTE)
    if(NOT "${_render_pipeline_INSTALL_DIR}" STREQUAL "${_CMAKE_INSTALL_PREFIX}")
        message(AUTHOR_WARNING "CMAKE_INSTALL_PREFIX is NOT same as installed directory of Render Pipeline:
        CMAKE_INSTALL_PREFIX: ${_CMAKE_INSTALL_PREFIX}
 render_pipeline_INSTALL_DIR: ${_render_pipeline_INSTALL_DIR}\n")
    endif()
endfunction()
