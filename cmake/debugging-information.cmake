# Author: Younguk Kim (bluekyu)
# Date  : 2017-07-28

# configure_debugging_information(target, [private_debinfo_postfix])
#
# Configure dubugging information for the target.
#
# If you want to strip private debugging information (ex, sources), you can set 'private_debinfo_postfix' string.
# Then, you will get stripped debugging information and a full debugging file.
#
# configure_debugging_information(<target> [<postfix>])
# @param    target      target variable
# @param    postfix     postfix of file to strip private debugging information
function(configure_debugging_information target)
    if(ARGV1)
        set(private_debinfo_postfix ${ARGV1})
        message(STATUS "Target '${target}' is set to strip debug symbol ...")
    endif()

    foreach(config "Debug" "RelWithDebInfo")
        string(TOUPPER ${config} config_upper)

        define_property(TARGET PROPERTY DEBINFO_PATH_${config_upper}
            BRIEF_DOCS "The path of FULL debugging information file."
            FULL_DOCS "The path of FULL debugging information file."
        )
        define_property(TARGET PROPERTY DEBINFO_STRIP_PATH_${config_upper}
            BRIEF_DOCS "The path of STRIPPED debugging information file."
            FULL_DOCS "The path of STRIPPED debugging information file."
        )

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

            if(private_debinfo_postfix)
                set(DEBINFO_PATH "${LIBRARY_OUTPUT_DIRECTORY}/${OUTPUT_NAME}${POSTFIX}${private_debinfo_postfix}.pdb")
                set(DEBINFO_STRIP_PATH "${LIBRARY_OUTPUT_DIRECTORY}/${OUTPUT_NAME}${POSTFIX}.pdb")

                set_target_properties(${target} PROPERTIES DEBINFO_PATH_${config_upper} "${DEBINFO_PATH}")
                set_target_properties(${target} PROPERTIES DEBINFO_STRIP_PATH_${config_upper} "${DEBINFO_STRIP_PATH}")

                set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS_${config_upper}
                    " /PDB:${DEBINFO_PATH} /PDBSTRIPPED:${DEBINFO_STRIP_PATH} "
                )
            else()
                set(DEBINFO_PATH "${LIBRARY_OUTPUT_DIRECTORY}/${OUTPUT_NAME}${POSTFIX}.pdb")
                set_target_properties(${target} PROPERTIES DEBINFO_PATH_${config_upper} "${DEBINFO_PATH}")
            endif()
        else()
            if(strip_postfix)
                message(WARNING "Striping private debugging infomation is NOT working in this platform.")
            endif()
            return()
        endif()
    endforeach()
endfunction()

# install_debugging_information(target destination)
#
# Install debugging information files for the target.
function(install_debugging_information target destination)
    foreach(config "Debug" "RelWithDebInfo")
        if(MSVC)
            string(TOUPPER ${config} config_upper)

            get_target_property(DEBINFO_PATH ${target} DEBINFO_PATH_${config_upper})
            get_target_property(DEBINFO_STRIP_PATH ${target} DEBINFO_STRIP_PATH_${config_upper})
            if(NOT DEBINFO_PATH)
                message(WARNING "'DEBINFO_PATH' variable is NOT found. Call 'configure_debug_info' function.")
                return()
            endif()

            install(FILES "${DEBINFO_PATH}" DESTINATION "${destination}" CONFIGURATIONS ${config})
            if(DEBINFO_STRIP_PATH)
                install(FILES "${DEBINFO_STRIP_PATH}" DESTINATION "${destination}" CONFIGURATIONS ${config})
            endif()
        endif()
    endforeach()
endfunction()
