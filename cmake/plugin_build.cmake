# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} ${LIB_TYPE} ${plugin_sources})
target_link_libraries(${PROJECT_NAME}
    PUBLIC ${Boost_LIBRARIES} ${Panda3d_LIBRARIES}
    PRIVATE render_pipeline)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${RPPLUGINS_FOLDER_NAME})

# library
set(RPPLUGINS_INSTALL_DIR "share/render_pipeline/${RPPLUGINS_FOLDER_NAME}/${PROJECT_NAME}")
install(TARGETS ${PROJECT_NAME} DESTINATION "${RPPLUGINS_INSTALL_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/" DESTINATION ${RPPLUGINS_INSTALL_DIR})
if(EXISTS "${PROJECT_SOURCE_DIR}/include")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION ${RPPLUGINS_INSTALL_DIR})
endif()
# ==================================================================================================
