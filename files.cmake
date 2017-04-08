# list render_pipeline/
set(header_rppanda_actor
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/actor/actor.h"
)

set(header_rppanda_stdpy
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/stdpy/file.h"
)

set(header_rppanda_util
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/util/image_input.h"
)

set(header_rppanda_gui
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_button.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_check_box.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_frame.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_gui_base.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_gui_globals.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_scroll_bar.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_scrolled_frame.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/direct_slider.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/onscreen_image.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/gui/onscreen_text.h"
)

set(header_rppanda_interval
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/interval/lerp_interval.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/interval/meta_interval.h"
)

set(header_rppanda_showbase
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/showbase/direct_object.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rppanda/showbase/showbase.h"
)

set(header_rpcore
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/config.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/effect.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/globals.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/image.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/light_manager.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/loader.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/logger.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/mount_manager.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/render_pipeline.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/render_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/render_target.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/rpobject.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stage_manager.h"
)

set(header_rpcore_gui
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/checkbox.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/checkbox_collection.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/draggable_window.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/exposure_widget.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/labeled_checkbox.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/render_mode_selector.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/slider.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/sprite.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/gui/text.h"
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
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/base_plugin.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/day_manager.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/day_setting_types.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/pluginbase/manager.h"
)

set(header_rpcore_stages
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/ambient_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/apply_lights_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/collect_used_cells_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/combine_velocity_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/cull_lights_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/downscale_z_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/final_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/flag_used_cells_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/gbuffer_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/shadow_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/update_previous_pipes_stage.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/stages/upscale_stage.h"
)

set(header_rpcore_util
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/cubemap_filter.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/generic.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/instancing_node.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/movement_controller.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/network_communication.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/primitives.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/rpmaterial.hpp"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/shader_input_blocks.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/submodule_downloader.h"
    "${PROJECT_SOURCE_DIR}/render_pipeline/rpcore/util/task_scheduler.h"
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
set(source_rppanda
    "${PROJECT_SOURCE_DIR}/src/rppanda/config_rppanda.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/config_rppanda.h"
)

set(source_rppanda_actor
    "${PROJECT_SOURCE_DIR}/src/rppanda/actor/actor.cpp"
)

set(source_rppanda_gui
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
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/lerp_interval.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/interval/meta_interval.cpp"
)

set(source_rppanda_showbase
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/direct_object.cpp"
    "${PROJECT_SOURCE_DIR}/src/rppanda/showbase/showbase.cpp"
)

set(source_rpcore
    "${PROJECT_SOURCE_DIR}/src/rpcore/common_resources.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/common_resources.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/effect.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/globals.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gpu_command_queue.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gpu_command_queue.h"
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
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/buffer_viewer.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/buffer_viewer.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/checkbox.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/checkbox_collection.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/debugger.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/debugger.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/draggable_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/error_message_display.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/error_message_display.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/exposure_widget.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/fps_chart.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/fps_chart.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/labeled_checkbox.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/loading_screen.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/loading_screen.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pipe_viewer.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pipe_viewer.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pixel_inspector.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/pixel_inspector.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/render_mode_selector.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/slider.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/sprite.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/text.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/text_node.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/text_node.hpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/gui/texture_preview.h"
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
    "${PROJECT_SOURCE_DIR}/src/rpcore/pluginbase/setting_types.h"
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
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/display_shader_builder.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/generic.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/ies_profile_loader.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/ies_profile_loader.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/instancing_node.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/movement_controller.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/network_communication.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/post_process_region.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/post_process_region.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/primitives.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/shader_input_blocks.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/smooth_connected_curve.h"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/smooth_connected_curve.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/submodule_downloader.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/task_scheduler.cpp"
    "${PROJECT_SOURCE_DIR}/src/rpcore/util/text3d.cpp"
)

set(source_rplibs
    "${PROJECT_SOURCE_DIR}/src/rplibs/yaml.cpp"
    "${PROJECT_SOURCE_DIR}/src/rplibs/yaml.hpp"
)

set(render_pipeline_sources
    ${source_rppanda}
    ${source_rppanda_actor}
    ${source_rppanda_gui}
    ${source_rppanda_interval}
    ${source_rppanda_showbase}
    ${source_rpcore}
    ${source_rpcore_gui}
    ${source_rpcore_native_source}
    ${source_rpcore_pluginbase}
    ${source_rpcore_stages}
    ${source_rpcore_util}
    ${source_rplibs}
)

# grouping
source_group("src\\rppanda" FILES ${source_rppanda})
source_group("src\\rppanda\\actor" FILES ${source_rppanda_actor})
source_group("src\\rppanda\\gui" FILES ${source_rppanda_gui})
source_group("src\\rppanda\\interval" FILES ${source_rppanda_interval})
source_group("src\\rppanda\\showbase" FILES ${source_rppanda_showbase})
source_group("src\\rpcore" FILES ${source_rpcore})
source_group("src\\rpcore\\gui" FILES ${source_rpcore_gui})
source_group("src\\rpcore\\native\\source" FILES ${source_rpcore_native_source})
source_group("src\\rpcore\\pluginbase" FILES ${source_rpcore_pluginbase})
source_group("src\\rpcore\\stages" FILES ${source_rpcore_stages})
source_group("src\\rpcore\\util" FILES ${source_rpcore_util})
source_group("src\\rplibs" FILES ${source_rplibs})
