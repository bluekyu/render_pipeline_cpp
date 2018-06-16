# === target =======================================================================================
add_library(${PROJECT_NAME} MODULE ${${PROJECT_NAME}_sources} ${${PROJECT_NAME}_headers})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251 /utf-8
        $<$<NOT:$<BOOL:${render_pipeline_ENABLE_RTTI}>>:/GR->
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/permissive->

        # note: windows.cmake in vcpkg
        $<$<CONFIG:Release>:/Oi /Gy /Z7>
    )

    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
        $<$<NOT:$<BOOL:${render_pipeline_ENABLE_RTTI}>>:-fno-rtti>
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPPLUGIN_ID_STRING="${RPPLUGIN_ID}"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC render_pipeline::render_pipeline
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "render_pipeline/rpplugins"
    DEBUG_POSTFIX ${render_pipeline_DEBUG_POSTFIX}
    RELWITHDEBINFO_POSTFIX ${render_pipeline_RELWITHDEBINFO_POSTFIX}
    VERSION ${PROJECT_VERSION}
)

configure_debugging_information(TARGET ${PROJECT_NAME} CONFIGURATIONS Debug RelWithDebInfo Release)
# ==================================================================================================

# === target =======================================================================================
# create plugin interface target
add_library(${RPPLUGIN_ID} INTERFACE)

target_include_directories(${RPPLUGIN_ID}
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${RPPLUGIN_INSTALL_DIR}/include>
)

target_link_libraries(${RPPLUGIN_ID}
    INTERFACE render_pipeline::render_pipeline
)

# configure package
set(PACKAGE_NAME "rpplugin_${RPPLUGIN_ID}")
set(TARGET_NAMESPACE "rpplugins::")
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake")
set(TARGET_EXPORT_NAME "${PACKAGE_NAME}-targets")
set(PACKAGE_CMAKE_INSTALL_DIR "${RPPLUGIN_INSTALL_DIR}/cmake")

include(CMakePackageConfigHelpers)
if(CMAKE_VERSION VERSION_LESS "3.11")
    write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY SameMajorVersion
    )
else()
    write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY SameMinorVersion
    )
endif()
configure_package_config_file(${RPPLUGIN_CONFIG_IN_FILE}
    ${PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)

add_library(${TARGET_NAMESPACE}${RPPLUGIN_ID} ALIAS ${RPPLUGIN_ID})
# ==================================================================================================
