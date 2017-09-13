# list render_pipeline/
set(header_rppanda_actor
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/actor/actor.hpp"
)

set(header_rppanda_stdpy
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/stdpy/file.hpp"
)

set(header_rppanda_util
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/util/image_input.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/util/filesystem.hpp"
)

set(header_rppanda_gui
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_button.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_check_box.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_frame.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_gui_base.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_gui_globals.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_scroll_bar.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_scrolled_frame.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_slider.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/onscreen_image.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/onscreen_text.hpp"
)

set(header_rppanda_interval
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/interval/actor_interval.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/interval/lerp_interval.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/interval/meta_interval.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/interval/sound_interval.hpp"
)

set(header_rppanda_showbase
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/showbase/direct_object.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/showbase/loader.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/showbase/sfx_player.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/showbase/showbase.hpp"
)

set(header_rpcore
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/config.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/effect.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/globals.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/image.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/light_manager.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/loader.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/logger.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/mount_manager.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/render_pipeline.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/render_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/render_target.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/rpobject.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stage_manager.hpp"
)

set(header_rpcore_gui
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/checkbox.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/draggable_window.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/labeled_checkbox.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/slider.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/sprite.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/text.hpp"
)

set(header_rpcore_native
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/config_rsnative.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/gpu_command.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/gpu_command.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/gpu_command_list.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/ies_dataset.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/internal_light_manager.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/internal_light_manager.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/pointer_slot_storage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/pssm_camera_rig.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/pssm_camera_rig.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/pssm_helper.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/rp_light.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/rp_light.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/rp_point_light.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/rp_point_light.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/rp_spot_light.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/rp_spot_light.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/shadow_atlas.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/shadow_atlas.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/shadow_manager.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/shadow_manager.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/shadow_source.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/shadow_source.I"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/tag_state_manager.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/native/tag_state_manager.I"
)

set(header_rpcore_pluginbase
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/base_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/day_manager.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/day_setting_types.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/manager.hpp"
)

set(header_rpcore_stages
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/ambient_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/apply_lights_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/collect_used_cells_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/combine_velocity_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/cull_lights_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/downscale_z_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/final_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/flag_used_cells_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/gbuffer_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/shadow_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/update_previous_pipes_stage.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/upscale_stage.hpp"
)

set(header_rpcore_util
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/cubemap_filter.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/generic.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/instancing_node.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/movement_controller.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/points_node.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/primitives.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/rpgeomnode.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/rpmaterial.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/rpmodel.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/shader_input_blocks.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/task_scheduler.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/text3d.hpp"
)

set(header_rplibs
    "${PROJECT_SOURCE_DIR}/render_pipeline/rplibs/py_to_cpp.hpp"
)

set(render_pipeline_headers
    ${header_rppanda_actor}
    ${header_rppanda_gui}
    ${header_rppanda_interval}
    ${header_rppanda_showbase}
    ${header_rppanda_stdpy}
    ${header_rppanda_util}
    ${header_rpcore}
    ${header_rpcore_gui}
    ${header_rpcore_native}
    ${header_rpcore_pluginbase}
    ${header_rpcore_stages}
    ${header_rpcore_util}
    ${header_rplibs}
)

# grouping
source_group("render_pipeline\\rppanda\\actor" FILES ${header_rppanda_actor})
source_group("render_pipeline\\rppanda\\gui" FILES ${header_rppanda_gui})
source_group("render_pipeline\\rppanda\\interval" FILES ${header_rppanda_interval})
source_group("render_pipeline\\rppanda\\showbase" FILES ${header_rppanda_showbase})
source_group("render_pipeline\\rppanda\\stdpy" FILES ${header_rppanda_stdpy})
source_group("render_pipeline\\rppanda\\util" FILES ${header_rppanda_util})
source_group("render_pipeline\\rpcore" FILES ${header_rpcore})
source_group("render_pipeline\\rpcore\\gui" FILES ${header_rpcore_gui})
source_group("render_pipeline\\rpcore\\native" FILES ${header_rpcore_native})
source_group("render_pipeline\\rpcore\\pluginbase" FILES ${header_rpcore_pluginbase})
source_group("render_pipeline\\rpcore\\stages" FILES ${header_rpcore_stages})
source_group("render_pipeline\\rpcore\\util" FILES ${header_rpcore_util})
source_group("render_pipeline\\rplibs" FILES ${header_rplibs})



# list src/
set(source_rppanda_actor
    "${PROJECT_SOURCE_DIR}/src/rppanda/actor/config_rppanda_actor.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/actor/config_rppanda_actor.hpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/actor/actor.cpp"
)

set(source_rppanda_gui
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/config_rppanda_gui.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/config_rppanda_gui.hpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_button.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_check_box.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_frame.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_gui_base.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_gui_globals.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_scroll_bar.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_scrolled_frame.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/direct_slider.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/onscreen_image.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/gui/onscreen_text.cpp"
)

set(source_rppanda_interval
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/actor_interval.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/config_rppanda_interval.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/config_rppanda_interval.hpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/lerp_interval.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/meta_interval.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/sound_interval.cpp"
)

set(source_rppanda_showbase
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/config_rppanda_showbase.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/config_rppanda_showbase.hpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/direct_object.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/loader.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/sfx_player.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/showbase.cpp"
)

set(source_rppanda_util
    "${PROJECT_SOURCE_DIR}/src/rppanda/util/filesystem.cpp"
)

set(source_rpcore
    "${PROJECT_SOURCE_DIR}/src/rpcore/common_resources.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/common_resources.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/effect.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/globals.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gpu_command_queue.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gpu_command_queue.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/image.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/light_manager.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/loader.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/logger.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/mount_manager.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/render_pipeline.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/render_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/render_target.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/rpobject.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stage_manager.cpp"
)

set(source_rpcore_gui
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/buffer_viewer.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/buffer_viewer.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/checkbox.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/debugger.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/debugger.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/draggable_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/error_message_display.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/error_message_display.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/exposure_widget.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/exposure_widget.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/fps_chart.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/fps_chart.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/labeled_checkbox.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/loading_screen.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/loading_screen.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pipe_viewer.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pipe_viewer.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pixel_inspector.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pixel_inspector.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/slider.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/sprite.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/text.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/text_node.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/text_node.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/texture_preview.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/texture_preview.cpp"
)

set(source_rpcore_native_source
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/config_rsnative.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/gpu_command.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/gpu_command_list.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/ies_dataset.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/internal_light_manager.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/pssm_camera_rig.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/pssm_helper.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/rp_light.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/rp_point_light.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/rp_spot_light.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/shadow_atlas.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/shadow_manager.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/shadow_source.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/native/tag_state_manager.cpp"
)

set(source_rpcore_pluginbase
    "${PROJECT_SOURCE_DIR}/src/rpcore/pluginbase/base_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/pluginbase/day_manager.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/pluginbase/day_setting_types.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/pluginbase/manager.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/pluginbase/setting_types.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/pluginbase/setting_types.hpp"
)

set(source_rpcore_stages
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/ambient_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/apply_lights_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/collect_used_cells_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/combine_velocity_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/cull_lights_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/downscale_z_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/final_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/flag_used_cells_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/gbuffer_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/shadow_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/update_previous_pipes_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/stages/upscale_stage.cpp"
)

set(source_rpcore_util
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/cubemap_filter.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/display_shader_builder.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/display_shader_builder.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/generic.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/ies_profile_loader.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/ies_profile_loader.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/instancing_node.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/movement_controller.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/points_node.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/post_process_region.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/post_process_region.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/primitives.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/rpgeomnode.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/rpmodel.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/shader_input_blocks.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/smooth_connected_curve.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/smooth_connected_curve.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/task_scheduler.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/text3d.cpp"
)

set(source_rplibs
    "${PROJECT_SOURCE_DIR}/src/rplibs/yaml.cpp"
    "${PROJECT_SOURCE_DIR}/src/rplibs/yaml.hpp"
)

set(render_pipeline_sources
    ${source_rppanda_actor}
    ${source_rppanda_gui}
    ${source_rppanda_interval}
    ${source_rppanda_showbase}
    ${source_rppanda_util}
    ${source_rpcore}
    ${source_rpcore_gui}
    ${source_rpcore_native_source}
    ${source_rpcore_pluginbase}
    ${source_rpcore_stages}
    ${source_rpcore_util}
    ${source_rplibs}
)

# grouping
source_group("src\\rppanda\\actor" FILES ${source_rppanda_actor})
source_group("src\\rppanda\\gui" FILES ${source_rppanda_gui})
source_group("src\\rppanda\\interval" FILES ${source_rppanda_interval})
source_group("src\\rppanda\\showbase" FILES ${source_rppanda_showbase})
source_group("src\\rppanda\\util" FILES ${source_rppanda_util})
source_group("src\\rpcore" FILES ${source_rpcore})
source_group("src\\rpcore\\gui" FILES ${source_rpcore_gui})
source_group("src\\rpcore\\native\\source" FILES ${source_rpcore_native_source})
source_group("src\\rpcore\\pluginbase" FILES ${source_rpcore_pluginbase})
source_group("src\\rpcore\\stages" FILES ${source_rpcore_stages})
source_group("src\\rpcore\\util" FILES ${source_rpcore_util})
source_group("src\\rplibs" FILES ${source_rplibs})
