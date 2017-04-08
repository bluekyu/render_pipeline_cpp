#include "rpcore/gui/debugger.h"

#include <graphicsBuffer.h>
#include <textNode.h>
#include <texturePool.h>
#include <textureCollection.h>
#include <sceneGraphAnalyzer.h>
#include <nodePathCollection.h>
#include <computeNode.h>

#include <spdlog/fmt/bundled/format.h>

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/light_manager.h>
#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/gui/sprite.h>
#include <render_pipeline/rppanda/showbase/showbase.h>
#include <render_pipeline/rpcore/gui/exposure_widget.h>
#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/util/task_scheduler.h>
#include <render_pipeline/rpcore/pluginbase/manager.h>
#include <render_pipeline/rpcore/pluginbase/day_manager.h>
#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include <rpplugins/scattering/include/scattering_plugin.hpp>
#include <rpplugins/pssm/include/pssm_plugin.hpp>

#include "rpcore/gui/text_node.hpp"
#include "rpcore/gpu_command_queue.h"
#include "rpcore/gui/buffer_viewer.h"
#include "rpcore/gui/error_message_display.h"
#include "rpcore/gui/fps_chart.h"
#include "rpcore/gui/pipe_viewer.h"
#include "rpcore/gui/pixel_inspector.h"

namespace rpcore {

Debugger::Debugger(RenderPipeline* pipeline): RPObject("Debugger"), pipeline(pipeline)
{
    debug("Creating debugger");
    _analyzer = new SceneGraphAnalyzer;

    fullscreen_node = Globals::base->get_pixel_2d().attach_new_node("rp_debugger");
    create_components();
    init_keybindings();

    if (use_advanced_info())
    {
        Globals::base->do_method_later(0.5f, [](GenericAsyncTask* task, void* user_data) -> AsyncTask::DoneStatus {
            return reinterpret_cast<Debugger*>(user_data)->collect_scene_data(nullptr, user_data);
            }, "RPDebugger_collectSceneData_initial", this);
    }

    Globals::base->do_method_later(0.1f, update_stats, "RPDebugger_updateStats", this);
}

Debugger::~Debugger(void)
{
    for (auto& tn: debug_lines)
        delete tn;
    delete _hint_reloading;
    delete keybinding_instructions_;
    delete _error_msg_handler;
    delete _exposure_widget;
    delete _fps_widget;
    delete _pipeline_logo;
    delete _pipe_viewer;
    delete _buffer_viewer;
    delete _analyzer;
}

bool Debugger::use_advanced_info(void) const
{
    return pipeline->get_setting<bool>("pipeline.advanced_debugging_info");
}

void Debugger::create_components(void)
{
    debugger_width = 460;
    debugger_visible = false;

    // XXX: this is unused code.
    //self.debugger_interval = None

    create_stats();
    create_hints();

    _pipeline_logo = new Sprite("/$$rp/data/gui/pipeline_logo_text.png", fullscreen_node, 30, 30);

    if (use_advanced_info())
    {
        exposure_node = fullscreen_node.attach_new_node("ExposureWidget");
        _exposure_widget = new ExposureWidget(pipeline, exposure_node);
    }

    fps_node = fullscreen_node.attach_new_node("FPSChart");
    fps_node.set_pos(LVecBase3f(21, 1, -108 - 40));
    _fps_widget = new FPSChart(pipeline, fps_node);

    _pixel_widget = new PixelInspector(pipeline);
    _buffer_viewer = new BufferViewer(pipeline, fullscreen_node);
    _pipe_viewer = new PipeViewer(pipeline, fullscreen_node);
    // TODO: implement
    //self.rm_selector = RenderModeSelector(self.pipeline, self.fullscreen_node)
    _error_msg_handler = new ErrorMessageDisplay;

    handle_window_resize();
}

void Debugger::update(void)
{
	_error_msg_handler->update();
	_pixel_widget->update();
}

AsyncTask::DoneStatus Debugger::collect_scene_data(GenericAsyncTask* task, void* user_data)
{
	_analyzer->clear();
	const auto& npc = Globals::base->get_render().find_all_matches("**/+GeomNode");
	for (int k=0, k_end=npc.get_num_paths(); k < k_end; ++k)
		_analyzer->add_node(npc.get_path(k).node());

	if (task)
		return AsyncTask::DS_again;
	return AsyncTask::DS_done;
}

void Debugger::create_stats(void)
{
    overlay_node = Globals::base->get_aspect_2d().attach_new_node("Overlay");
    debug_lines.clear();

    const int num_lines = use_advanced_info() ? 6 : 1;
    for (int k = 0; k < num_lines; ++k)
    {
        rpcore::TextNode::Parameters params;
        params.parent = overlay_node;
        params.pixel_size = 16.0f;
        params.pos = LVecBase2(0, -k * 0.046f);
        params.color = LVecBase3(0.7, 1, 1);
        params.align = "right";
        debug_lines.push_back(new rpcore::TextNode(params));
    }
    debug_lines[0]->set_color(LColor(1, 1, 0, 1));
}

void Debugger::create_hints(void)
{
    _hint_reloading = new Sprite("/$$rp/data/gui/shader_reload_hint.png", fullscreen_node);
    set_reload_hint_visible(false);

    // C++ does not implement this part.
    //python_warning = nullptr;

    // Keybinding hints
    keybinding_instructions_ = new Sprite("/$$rp/data/gui/keybindings.png", fullscreen_node, 0.0f, 0.0f, true, true, false);

    TextNode::Parameters params;
    params.text = "F6: View Keyboard Shortcuts";
    params.color = LVecBase3(0.8, 0.8, 0.8);
    keybinding_text_ = new TextNode(params);
    toggle_keybindings_visible();
}

void Debugger::set_reload_hint_visible(bool flag)
{
	if (flag)
		_hint_reloading->show();
	else
		_hint_reloading->hide();
}

void Debugger::handle_window_resize(void)
{
    // When using small resolutions, scale the GUI so its still useable,
    // otherwise the sub-windows are bigger than the main window
    gui_scale = (std::max)(0.65f, (std::min)(1.0f, Globals::native_resolution.get_x() / 1920.0f));
    fullscreen_node.set_scale(gui_scale);

    // TODO: implement
    if (use_advanced_info())
    {
        exposure_node.set_pos(
            int(Globals::native_resolution.get_x() / gui_scale) - 200,
            1, int(-Globals::native_resolution.get_y() / gui_scale) + 120);
    }
    _hint_reloading->set_pos(float((Globals::native_resolution.get_x()) / 2) / gui_scale - 465 / 2, 220);
    keybinding_instructions_->set_pos(30, Globals::native_resolution.get_y() / gui_scale - 510.0);
    keybinding_text_->get_np().set_pos(-Globals::base->get_aspect_ratio() + 0.07, 0, -0.9);
    keybinding_text_->set_pixel_size(16 * (std::max)(0.8f, gui_scale));
    overlay_node.set_pos(Globals::base->get_aspect_ratio() - 0.07, 1, 1.0 - 0.07);

    // C++ does not implement this part.
    //if (python_warning)
    //{
    //  self.python_warning.set_pos(
    //      (Globals.native_resolution.x // self.gui_scale - 1054) // 2,
    //      (Globals.native_resolution.y // self.gui_scale - 118 - 40))
    //}

    for (const auto& text: debug_lines)
    {
        text->set_pixel_size(16 * max(0.8, gui_scale));
    }

    _buffer_viewer->center_on_screen();
    _pipe_viewer->center_on_screen();
    //self.rm_selector.center_on_screen()
}

void Debugger::init_keybindings(void)
{
    Globals::base->accept("v", [](const Event* ev, void* data) {
        if (Globals::base->get_render_2d().is_hidden())
            Globals::base->get_render_2d().show();
        reinterpret_cast<Debugger*>(data)->_buffer_viewer->toggle();
    }, this);
    Globals::base->accept("c", [](const Event* ev, void* data) {
        if (Globals::base->get_render_2d().is_hidden())
            Globals::base->get_render_2d().show();
        reinterpret_cast<Debugger*>(data)->_pipe_viewer->toggle();
    }, this);

    // TODO: implement
    //Globals.base.accept("z", self.rm_selector.toggle)
    Globals::base->accept("f5", [](const Event* ev, void* data) {
        reinterpret_cast<Debugger*>(data)->toggle_gui_visible();
    }, this);
    Globals::base->accept("f6", [](const Event* ev, void* data) {
        if (Globals::base->get_render_2d().is_hidden())
            Globals::base->get_render_2d().show();
        reinterpret_cast<Debugger*>(data)->toggle_keybindings_visible();
    }, this);
    Globals::base->accept("r", [](const Event* ev, void* data) {
        reinterpret_cast<Debugger*>(data)->pipeline->reload_shaders();
    }, this);
}

void Debugger::toggle_gui_visible(void)
{
    if (Globals::base->get_render_2d().is_hidden())
    {
        Globals::base->get_render_2d().show();
    }
    else
    {
        Globals::base->get_render_2d().hide();
    }
}

void Debugger::toggle_keybindings_visible(void)
{
    if (keybinding_instructions_->is_hidden())
    {
        keybinding_instructions_->show();
        keybinding_text_->get_np().hide();
    }
    else
    {
        keybinding_instructions_->hide();
        keybinding_text_->get_np().show();
    }
}

AsyncTask::DoneStatus Debugger::update_stats(GenericAsyncTask* task, void* user_data)
{
    Debugger* debugger = reinterpret_cast<Debugger*>(user_data);
    const RenderPipeline* pipeline = debugger->pipeline;

    const auto& clock = Globals::clock;
    debugger->debug_lines[0]->set_text(fmt::format("{:3.0f} fps  |  {:3.1f} ms  |  {:3.1f} ms max",
        clock->get_average_frame_rate(),
        (1000.0 / (std::max)(0.001, clock->get_average_frame_rate())),
        (clock->get_max_frame_duration() * 1000.0)));

    if (!debugger->use_advanced_info())
        return task ? AsyncTask::DS_again : AsyncTask::DS_done;

    const auto& light_mgr = pipeline->get_light_mgr();

    debugger->debug_lines[1]->set_text(fmt::format(
        "{:4d} states |  {:4d} transforms |  {:4d} cmds |  {:4d} lights |  {:4d} shadow |  {:5.1f}% atlas usage",

        RenderState::get_num_states(),
        TransformState::get_num_states(),
        light_mgr->get_cmd_queue()->get_num_processed_commands(),
        light_mgr->get_num_lights(),
        light_mgr->get_num_shadow_sources(),
        light_mgr->get_shadow_atlas_coverage()));

    const auto& tex_memory_count = debugger->_buffer_viewer->get_stage_information();

    int views = 0;
    int active_views = 0;
    for (const auto& target: RenderTarget::REGISTERED_TARGETS)
    {
        if (!target->get_create_default_region())
        {
            int num_regions = target->get_internal_buffer()->get_num_display_regions();
            for (int i = 0; i < num_regions; ++i)
            {
                // Skip overlay display region
                if (i == 0 && num_regions > 1)
                    continue;
                ++views;

                if (target->get_active() && target->get_internal_buffer()->get_display_region(i)->is_active())
                    ++active_views;
            }
        }
    }

    debugger->debug_lines[2]->set_text(fmt::format(
        "Internal:  {:3.0f} MB VRAM  |  {:5d} img |  {:5d} tex |  "
        "{:5d} fbos |  {:3d} plugins |  {:2d}  views  ({:2d} active)",

        (tex_memory_count.first / (1024.0f*1024.0f)),
        Image::REGISTERED_IMAGES.size(),
        tex_memory_count.second,
        RenderTarget::REGISTERED_TARGETS.size(),
        pipeline->get_plugin_mgr()->get_enabled_plugins_count(),
        views,
        active_views
    ));

    size_t scene_tex_size = 0;
    const auto& texture_collection = TexturePool::find_all_textures();
    const int tex_count = texture_collection.get_num_textures();
    for (int k = 0; k < tex_count; ++k)
        scene_tex_size += texture_collection.get_texture(k)->estimate_texture_memory();

    debugger->debug_lines[3]->set_text(fmt::format(
        "Scene:   {:4.0f} MB VRAM |  {:3d} tex |  {:4d} geoms |  {:4d} nodes |  {:7d} vertices",

        (scene_tex_size / (1024.0*1024.0)),
        tex_count,
        debugger->_analyzer->get_num_geoms(),
        debugger->_analyzer->get_num_nodes(),
        debugger->_analyzer->get_num_vertices()
        ));

    LVecBase3f sun_vector(0);
    if (pipeline->get_plugin_mgr()->is_plugin_enabled("scattering"))
    {
        sun_vector = dynamic_pointer_cast<rpplugins::ScatteringPlugin>(pipeline->get_plugin_mgr()->get_instance("scattering"))->get_sun_vector();
    }

    const NodePath& camera = Globals::base->get_cam();
    const NodePath& render = Globals::base->get_render();
    const LPoint3& camera_global_pos = camera.get_pos(render);

    debugger->debug_lines[4]->set_text(fmt::format(
        "Time: {} ({:1.3f}) |  Sun  {:0.2f} {:0.2f} {:0.2f} |  X {:3.1f}  Y {:3.1f}  Z {:3.1f} |  {:2d} tasks |  scheduled: {:2d}",

        pipeline->get_daytime_mgr()->get_formatted_time(),
        pipeline->get_daytime_mgr()->get_time(),
        sun_vector[0], sun_vector[1], sun_vector[2],
        camera_global_pos[0],
        camera_global_pos[1],
        camera_global_pos[2],
        pipeline->get_task_scheduler()->get_num_tasks(),
        pipeline->get_task_scheduler()->get_num_scheduled_tasks()
    ));

    static std::string debug_lines_5_text;
    debug_lines_5_text = "Scene shadows:  ";
    if (pipeline->get_plugin_mgr()->is_plugin_enabled("pssm"))
    {
        const std::shared_ptr<BasePlugin>& pssm_plugin = pipeline->get_plugin_mgr()->get_instance("pssm");

        LVecBase3f focus_point;
        float focus_size;
        bool exist = std::dynamic_pointer_cast<rpplugins::PSSMPlugin>(pssm_plugin)->get_last_focus(focus_point, focus_size);

        if (exist)
        {
            debug_lines_5_text += fmt::format("{:3.1f} {:3.1f} {:3.1f} r {:3.1f}",
                focus_point[0], focus_point[1], focus_point[2], focus_size);
        }
        else
        {
            debug_lines_5_text += "none";
        }
    }
    else
    {
        debug_lines_5_text += "inactive";
    }

    const LPoint3& camera_global_hpr = camera.get_hpr(render);

    debug_lines_5_text += fmt::format("   |  HPR  ({:3.1f} {:3.1f} {:3.1f})  |   {:4d} x {:4d} pixels @ {:3.1f} %   |  {:3d} x {:3d} tiles",
        camera_global_hpr[0],
        camera_global_hpr[1],
        camera_global_hpr[2],
        Globals::native_resolution.get_x(),
        Globals::native_resolution.get_y(),
        (pipeline->get_setting<float>("pipeline.resolution_scale") * 100.0f),
        light_mgr->get_num_tiles().get_x(),
        light_mgr->get_num_tiles().get_y());
    debugger->debug_lines[5]->set_text(debug_lines_5_text);

    return task ? AsyncTask::DS_again : AsyncTask::DS_done;
}

}
