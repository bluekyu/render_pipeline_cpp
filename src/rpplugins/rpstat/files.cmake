# list header
set(${PROJECT_NAME}_header_root
    "${PROJECT_SOURCE_DIR}/include/rpplugins/rpstat/gui_helper.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpplugins/rpstat/gui_interface.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpplugins/rpstat/plugin.hpp"
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
)

# grouping
source_group("rpstat" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_imgui
    "${PROJECT_SOURCE_DIR}/src/imgui/imgui_stdlib.cpp"
    "${PROJECT_SOURCE_DIR}/src/imgui/imgui_stdlib.h"
)

set(${PROJECT_NAME}_source_ImGuizmo
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/imgui_internal.h"
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/ImGuizmo.cpp"
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/ImGuizmo.h"
    "${PROJECT_SOURCE_DIR}/src/ImGuizmo/imstb_textedit.h"
)

set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/actor_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/actor_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/anim_control_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/anim_control_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/day_manager_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/day_manager_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/dialog.cpp"
    "${PROJECT_SOURCE_DIR}/src/dialog.hpp"
    "${PROJECT_SOURCE_DIR}/src/file_dialog.cpp"
    "${PROJECT_SOURCE_DIR}/src/file_dialog.hpp"
    "${PROJECT_SOURCE_DIR}/src/import_model_dialog.cpp"
    "${PROJECT_SOURCE_DIR}/src/import_model_dialog.hpp"
    "${PROJECT_SOURCE_DIR}/src/load_animation_dialog.cpp"
    "${PROJECT_SOURCE_DIR}/src/load_animation_dialog.hpp"
    "${PROJECT_SOURCE_DIR}/src/material_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/material_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/nodepath_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/nodepath_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/scenegraph_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/scenegraph_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/texture_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/texture_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/window_interface.cpp"
    "${PROJECT_SOURCE_DIR}/src/window_interface.hpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_imgui}
    ${${PROJECT_NAME}_source_ImGuizmo}
    ${${PROJECT_NAME}_source_root}
)

# grouping
source_group("src\\ImGuizmo" FILES ${${PROJECT_NAME}_source_ImGuizmo})
source_group("src\\imgui" FILES ${${PROJECT_NAME}_source_imgui})
source_group("src" FILES ${${PROJECT_NAME}_source_root})
