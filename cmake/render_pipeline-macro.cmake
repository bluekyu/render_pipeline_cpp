# render pipeline variable
set(render_pipeline_DATA_DIR "share/render_pipeline" CACHE INTERNAL
    "Relative path of data directory in Render Pipeline"
)
set(render_pipeline_PLUGIN_DIR "${render_pipeline_DATA_DIR}/rpplugins" CACHE INTERNAL
    "Relative path of plugin directory in Render Pipeline"
)

# help function to find rpplugin
function(render_pipeline_FIND_PLUGINS plugin_id_list)
    set(missed_plugin_id_list "")
    foreach(plugin_id ${plugin_id_list})
        if(NOT TARGET rpplugin::${plugin_id})
            list(APPEND missed_plugin_id_list ${plugin_id})
        endif()
    endforeach()

    if(missed_plugin_id_list)
        if(render_pipeline_INSTALL_DIR)
            set(PLUGIN_DIR_HINT "${render_pipeline_INSTALL_DIR}/${render_pipeline_PLUGIN_DIR}")
        endif()

        foreach(plugin_id ${missed_plugin_id_list})
            find_package(rpplugin_${plugin_id} REQUIRED HINTS "${PLUGIN_DIR_HINT}/${plugin_id}")
        endforeach()
    endif()

    message(STATUS "Found the following Render Pipeline plugins:")
    foreach(plugin_id ${plugin_id_list})
        message(STATUS "  ${plugin_id}")
    endforeach()
endfunction()
