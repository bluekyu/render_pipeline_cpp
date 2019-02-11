# list header
set(${PROJECT_NAME}_header_root
    "${PROJECT_SOURCE_DIR}/include/rpplugins/imgui/plugin.hpp"
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
)

# grouping
source_group("imgui" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
)

set(${PROJECT_NAME}_thirdparty_panda3d_imgui
    "${PROJECT_SOURCE_DIR}/thirdparty/panda3d_imgui/panda3d_imgui/panda3d_imgui.cpp"
    "${PROJECT_SOURCE_DIR}/thirdparty/panda3d_imgui/panda3d_imgui/panda3d_imgui.hpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_root}
    ${${PROJECT_NAME}_thirdparty_panda3d_imgui}
)

# grouping
source_group("src" FILES ${${PROJECT_NAME}_source_root})
source_group("thirdparty\\panda3d_imgui" FILES ${${PROJECT_NAME}_thirdparty_panda3d_imgui})
