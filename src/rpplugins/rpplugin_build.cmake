# === target =======================================================================================
add_library(${PROJECT_NAME} MODULE ${plugin_sources})

if(MSVC)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_final cxx_strong_enums cxx_delegating_constructors
        PRIVATE $<$<VERSION_GREATER:${MSVC_VERSION},1800>:cxx_generic_lambdas>
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/std:c++14>
    )
else()
    if(CMAKE_VERSION VERSION_LESS 3.8)
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -std=c++1z)
    else()
        target_compile_features(${PROJECT_NAME} PUBLIC cxx_final cxx_strong_enums cxx_delegating_constructors
            PRIVATE cxx_std_17)
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
    endif()
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPPLUGIN_ID_STRING="${RPPLUGIN_ID}"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline::render_pipeline
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "render_pipeline/rpplugins"
    DEBUG_POSTFIX ${render_pipeline_DEBUG_POSTFIX}
    RELWITHDEBINFO_POSTFIX ${render_pipeline_RELWITHDEBINFO_POSTFIX}
    VERSION ${PROJECT_VERSION}
)
# ==================================================================================================

# === target =======================================================================================
# create plugin interface target
add_library(${RPPLUGIN_ID} INTERFACE)

target_include_directories(${RPPLUGIN_ID}
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${RPPLUGIN_INSTALL_DIR}/include>
)

target_link_libraries(${RPPLUGIN_ID}
    INTERFACE render_pipeline
)

# configure package
set(PACKAGE_NAME "rpplugin_${RPPLUGIN_ID}")
set(TARGET_NAMESPACE "rpplugins::")
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake")
set(TARGET_EXPORT_NAME "${PACKAGE_NAME}-targets")
set(PACKAGE_CMAKE_INSTALL_DIR "${RPPLUGIN_INSTALL_DIR}/cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(${RPPLUGIN_CONFIG_IN_FILE}
    ${PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)

add_library(${TARGET_NAMESPACE}${RPPLUGIN_ID} ALIAS ${RPPLUGIN_ID})
# ==================================================================================================
