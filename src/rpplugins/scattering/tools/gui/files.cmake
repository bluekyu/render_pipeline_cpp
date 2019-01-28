# list header
set(${PROJECT_NAME}_header_root
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
)

# grouping
source_group("openvr" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_root}
)

# grouping
source_group("src" FILES ${${PROJECT_NAME}_source_root})
