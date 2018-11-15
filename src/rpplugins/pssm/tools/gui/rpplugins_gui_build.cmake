# === target =======================================================================================
add_library(${PROJECT_NAME} MODULE ${${PROJECT_NAME}_sources} ${${PROJECT_NAME}_headers})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251 /utf-8 /permissive-
        $<$<NOT:$<BOOL:${rpcpp_plugins_ENABLE_RTTI}>>:/GR->

        # note: windows.cmake in vcpkg
        $<$<CONFIG:Release>:/Oi /Gy /Z7>
    )

    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
        $<$<NOT:$<BOOL:${rpcpp_plugins_ENABLE_RTTI}>>:-fno-rtti>
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPPLUGINS_GUI_ID_STRING="${RPPLUGINS_ID}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline::render_pipeline imgui::imgui

    rpplugins::imgui
    rpplugins::rpstat
    ${RPPLUGINS_ID}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "rpplugins_gui"
    DEBUG_POSTFIX ${render_pipeline_DEBUG_POSTFIX}
    RELWITHDEBINFO_POSTFIX ${render_pipeline_RELWITHDEBINFO_POSTFIX}
    VERSION ${PROJECT_VERSION}
)
# ==================================================================================================
