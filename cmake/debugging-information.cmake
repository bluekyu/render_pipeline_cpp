# Author: Younguk Kim (bluekyu)


# Configure dubugging information for the target.
#
# If you want to strip private debugging information (ex, sources), you can set 'postfix' string.
# Then, you will get stripped debugging information and a full debugging file.
#
# configure_debugging_information(TARGET <target> [POSTFIX <postfix>])
# @param    TARGET      target variable
# @param    POSTFIX     postfix of file of full (private) debugging information
function(configure_debugging_information)
    cmake_parse_arguments(ARG "" "TARGET;POSTFIX" "CONFIGURATIONS" ${ARGN})

    if(NOT ARG_CONFIGURATIONS)
        message(FATAL_ERROR "At least one configuration should be used in CONFIGURATIONS.")
    endif()

    if(ARG_POSTFIX)
        set(private_debinfo_postfix ${ARG_POSTFIX})
        message("Target '${ARG_TARGET}' is set to strip private debugging information.")
    endif()

    foreach(config ${ARG_CONFIGURATIONS})
        string(TOUPPER ${config} config_upper)

        define_property(TARGET PROPERTY DEBINFO_PATH_${config_upper}
            BRIEF_DOCS "The path of FULL debugging information file."
            FULL_DOCS "The path of FULL debugging information file."
        )

        if(MSVC)
            define_property(TARGET PROPERTY DEBINFO_STRIP_PATH_${config_upper}
                BRIEF_DOCS "The path of STRIPPED debugging information file."
                FULL_DOCS "The path of STRIPPED debugging information file."
            )

            get_target_property(LIBRARY_OUTPUT_DIRECTORY ${ARG_TARGET} LIBRARY_OUTPUT_DIRECTORY_${config_upper})
            if(NOT LIBRARY_OUTPUT_DIRECTORY)
                set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${config}")
            endif()

            get_target_property(OUTPUT_NAME ${ARG_TARGET} OUTPUT_NAME_${config_upper})
            if(NOT OUTPUT_NAME)
                get_target_property(OUTPUT_NAME ${ARG_TARGET} NAME)
            endif()

            get_target_property(POSTFIX ${ARG_TARGET} ${config_upper}_POSTFIX)
            if(NOT POSTFIX)
                set(POSTFIX "")
            endif()

            if(private_debinfo_postfix)
                set(DEBINFO_PATH "${LIBRARY_OUTPUT_DIRECTORY}/${OUTPUT_NAME}${POSTFIX}${private_debinfo_postfix}.pdb")
                set(DEBINFO_STRIP_PATH "${LIBRARY_OUTPUT_DIRECTORY}/${OUTPUT_NAME}${POSTFIX}.pdb")

                set_target_properties(${ARG_TARGET} PROPERTIES DEBINFO_PATH_${config_upper} "${DEBINFO_PATH}")
                set_target_properties(${ARG_TARGET} PROPERTIES DEBINFO_STRIP_PATH_${config_upper} "${DEBINFO_STRIP_PATH}")

                set_property(TARGET ${ARG_TARGET} APPEND_STRING PROPERTY LINK_FLAGS_${config_upper}
                    " /PDB:${DEBINFO_PATH} /PDBSTRIPPED:${DEBINFO_STRIP_PATH} "
                )
            else()
                set(DEBINFO_PATH "${LIBRARY_OUTPUT_DIRECTORY}/${OUTPUT_NAME}${POSTFIX}.pdb")
                set_target_properties(${ARG_TARGET} PROPERTIES DEBINFO_PATH_${config_upper} "${DEBINFO_PATH}")
            endif()
        else()
            if(strip_postfix)
                message(WARNING "Striping private debugging infomation is NOT working in this platform.")
            endif()
            return()
        endif()
    endforeach()
endfunction()


# Install debugging information files for the target configured by "configure_debugging_information"
#
# If a stripped debugging information exists, this installs only it.
# If "INSTALL_PRIVATE" is specified, this installs both the stripped debugging information
# and a full (private) debugging information.
#
# install_debugging_information(TARGET <target> DESTINATION <destination> [INSTALL_PRIVATE])
# @param    TARGET              Target variable
# @param    DESTINATION         Installed destination
# @param    INSTALL_PRIVATE     Install also full (private) debugging information
function(install_debugging_information)
    cmake_parse_arguments(ARG "INSTALL_PRIVATE" "TARGET;DESTINATION" "CONFIGURATIONS" ${ARGN})

    if(NOT ARG_CONFIGURATIONS)
        message(FATAL_ERROR "At least one configuration should be used in CONFIGURATIONS.")
    endif()

    if(ARG_INSTALL_PRIVATE)
        message("Target '${ARG_TARGET}' will install full (private) debugging information.")
    endif()

    foreach(config ${ARG_CONFIGURATIONS})
        if(MSVC)
            string(TOUPPER ${config} config_upper)

            get_target_property(DEBINFO_PATH ${ARG_TARGET} DEBINFO_PATH_${config_upper})
            get_target_property(DEBINFO_STRIP_PATH ${ARG_TARGET} DEBINFO_STRIP_PATH_${config_upper})
            if(NOT DEBINFO_PATH)
                message(WARNING "'DEBINFO_PATH' variable is NOT found. Call 'configure_debug_info' function.")
                return()
            endif()

            if(DEBINFO_STRIP_PATH)
                install(FILES "${DEBINFO_STRIP_PATH}" DESTINATION "${ARG_DESTINATION}" CONFIGURATIONS ${config})
                if(ARG_INSTALL_PRIVATE)
                    install(FILES "${DEBINFO_PATH}" DESTINATION "${ARG_DESTINATION}" CONFIGURATIONS ${config})
                endif()
            else()
                install(FILES "${DEBINFO_PATH}" DESTINATION "${ARG_DESTINATION}" CONFIGURATIONS ${config})
            endif()
        endif()
    endforeach()
endfunction()
