# Author: Younguk Kim (bluekyu)
# Date  : 2017-07-28

function(target_strip_debug_symbol target strip_postfix)
    message(STATUS "Target '${target}' is set to strip debug symbol ...")
    foreach(config "Debug" "RelWithDebInfo")
        string(TOUPPER ${config} config_upper)
        if(MSVC)
            get_target_property(LIBRARY_OUTPUT_DIRECTORY ${target} LIBRARY_OUTPUT_DIRECTORY_${config_upper})
            if(NOT LIBRARY_OUTPUT_DIRECTORY)
                set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${config}")
            endif()

            get_target_property(OUTPUT_NAME ${target} OUTPUT_NAME_${config_upper})
            if(NOT OUTPUT_NAME)
                get_target_property(OUTPUT_NAME ${target} NAME)
            endif()

            get_target_property(POSTFIX ${target} ${config_upper}_POSTFIX)

            set(STRIPPED_PDB_PATH "${LIBRARY_OUTPUT_DIRECTORY}/${OUTPUT_NAME}${POSTFIX}${strip_postfix}.pdb")

            # msvc
            set_target_properties(${target} PROPERTIES LINK_FLAGS_${config_upper} /PDBSTRIPPED:${STRIPPED_PDB_PATH})

            define_property(TARGET PROPERTY STRIPPED_PDB_PATH_${config_upper}
                BRIEF_DOCS "The path of stripped PDB file."
                FULL_DOCS "The path of stripped PDB file."
            )
            set_target_properties(${target} PROPERTIES STRIPPED_PDB_PATH_${config_upper} "${STRIPPED_PDB_PATH}")
        else()
            message(WARNING "Strip debug symbol is NOT working in this platform.")
        endif()
    endforeach()
endfunction()
