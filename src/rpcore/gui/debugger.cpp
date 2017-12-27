/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "rpcore/gui/debugger.hpp"

#include <graphicsBuffer.h>
#include <textNode.h>
#include <texturePool.h>
#include <textureCollection.h>
#include <sceneGraphAnalyzer.h>
#include <nodePathCollection.h>
#include <computeNode.h>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/gui/direct_scrolled_frame.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/light_manager.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/util/task_scheduler.hpp"
#include "render_pipeline/rpcore/pluginbase/manager.hpp"
#include "render_pipeline/rpcore/pluginbase/day_manager.hpp"
#include "render_pipeline/rpcore/pluginbase/base_plugin.hpp"

#include <rpplugins/scattering/include/scattering_plugin.hpp>
#include <rpplugins/pssm/include/pssm_plugin.hpp>

#include "rpcore/gpu_command_queue.hpp"
#include "rpcore/gui/exposure_widget.hpp"
#include "rpcore/gui/text_node.hpp"
#include "rpcore/gui/buffer_viewer.hpp"
#include "rpcore/gui/error_message_display.hpp"
#include "rpcore/gui/fps_chart.hpp"
#include "rpcore/gui/pipe_viewer.hpp"
#include "rpcore/gui/pixel_inspector.hpp"

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
        Globals::base->do_method_later(0.5f, [this](rppanda::FunctionalTask* task) {
            return collect_scene_data(nullptr);
            }, "RPDebugger_collectSceneData_initial");
    }

    Globals::base->do_method_later(0.1f, std::bind(&Debugger::update_stats, this, std::placeholders::_1),
        "RPDebugger_updateStats");
}

Debugger::~Debugger()
{
    trace("Destructing Debugger");

    for (auto&& tn: debug_lines)
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

bool Debugger::use_advanced_info() const
{
    return pipeline->get_setting<bool>("pipeline.advanced_debugging_info");
}

void Debugger::create_components()
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

void Debugger::update()
{
    _error_msg_handler->update();
    _pixel_widget->update();
}

AsyncTask::DoneStatus Debugger::collect_scene_data(rppanda::FunctionalTask* task)
{
    _analyzer->clear();
    const auto& npc = Globals::base->get_render().find_all_matches("**/+GeomNode");
    for (int k=0, k_end=npc.get_num_paths(); k < k_end; ++k)
        _analyzer->add_node(npc.get_path(k).node());

    if (task)
        return AsyncTask::DS_again;
    return AsyncTask::DS_done;
}

void Debugger::create_stats()
{
    overlay_node = Globals::base->get_aspect_2d().attach_new_node("Overlay");
    debug_lines.clear();

    const int num_lines = use_advanced_info() ? 6 : 1;
    for (int k = 0; k < num_lines; ++k)
    {
        debug_lines.push_back(new TextNode(
            overlay_node,
            TextNode::Default::pixel_size,
            LVecBase2(0, -k * 0.046f),
            LVecBase3(0.7, 1, 1),
            "right"));
    }
    debug_lines[0]->set_color(LColor(1, 1, 0, 1));
}

void Debugger::create_hints()
{
    _hint_reloading = new Sprite("/$$rp/data/gui/shader_reload_hint.png", fullscreen_node);
    set_reload_hint_visible(false);

    // C++ does not implement this part.
    //python_warning = nullptr;

    // Keybinding hints
    keybinding_instructions_ = new Sprite("/$$rp/data/gui/keybindings.png", fullscreen_node, 0.0f, 0.0f, true, true, false);

    keybinding_text_ = new TextNode(
        {},
        TextNode::Default::pixel_size,
        LVecBase2(0),
        LVecBase3(0.8, 0.8, 0.8),
        TextNode::Default::align,
        TextNode::Default::font,
        "F6: View Keyboard Shortcuts"
    );
    toggle_keybindings_visible();
}

void Debugger::set_reload_hint_visible(bool flag)
{
    if (flag)
        _hint_reloading->show();
    else
        _hint_reloading->hide();
}

void Debugger::handle_window_resize()
{
    // When using small resolutions, scale the GUI so its still useable,
    // otherwise the sub-windows are bigger than the main window
    gui_scale = (std::max)(0.65f, (std::min)(1.0f, Globals::native_resolution.get_x() / 1920.0f));
    fullscreen_node.set_scale(gui_scale);

    if (use_advanced_info())
    {
        exposure_node.set_pos(
            static_cast<int>(Globals::native_resolution.get_x() / gui_scale) - 200,
            1, static_cast<int>(-Globals::native_resolution.get_y() / gui_scale) + 120);
    }
    _hint_reloading->set_pos((Globals::native_resolution.get_x() / 2.0f) / gui_scale - 465 / 2, 220);
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
        text->set_pixel_size(16 * (std::max)(0.8f, gui_scale));
    }

    _buffer_viewer->center_on_screen();
    _pipe_viewer->center_on_screen();
    // TODO: implement
    //self.rm_selector.center_on_screen()
}

void Debugger::init_keybindings()
{
    Globals::base->accept("v", [this](const Event* ev) {
        if (Globals::base->get_render_2d().is_hidden())
            Globals::base->get_render_2d().show();
        _buffer_viewer->toggle();
    });
    Globals::base->accept("c", [this](const Event* ev) {
        if (Globals::base->get_render_2d().is_hidden())
            Globals::base->get_render_2d().show();
        _pipe_viewer->toggle();
    });

    // TODO: implement
    //Globals.base.accept("z", self.rm_selector.toggle)
    Globals::base->accept("f5", [this](const Event* ev) {
        toggle_gui_visible();
    });
    Globals::base->accept("f6", [this](const Event* ev) {
        if (Globals::base->get_render_2d().is_hidden())
            Globals::base->get_render_2d().show();
        toggle_keybindings_visible();
    });
    Globals::base->accept("r", [this](const Event* ev) {
        pipeline->reload_shaders();
    });
}

void Debugger::toggle_gui_visible()
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

void Debugger::toggle_keybindings_visible()
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

AsyncTask::DoneStatus Debugger::update_stats(rppanda::FunctionalTask* task)
{
    const auto& clock = Globals::clock;
    debug_lines[0]->set_text(fmt::format("{:3.1f} fps  |  {:3.2f} ms  |  {:3.2f} ms max",
        clock->get_average_frame_rate(),
        (1000.0 / (std::max)(0.001, clock->get_average_frame_rate())),
        (clock->get_max_frame_duration() * 1000.0)));

    if (!use_advanced_info())
        return task ? AsyncTask::DS_again : AsyncTask::DS_done;

    const auto& light_mgr = pipeline->get_light_mgr();

    debug_lines[1]->set_text(fmt::format(
        "{:4d} states |  {:4d} transforms |  {:4d} cmds |  {:4d} lights |  {:4d} shadow |  {:5.1f}% atlas usage",

        RenderState::get_num_states(),
        TransformState::get_num_states(),
        light_mgr->get_cmd_queue()->get_num_processed_commands(),
        light_mgr->get_num_lights(),
        light_mgr->get_num_shadow_sources(),
        light_mgr->get_shadow_atlas_coverage()));

    const auto& tex_memory_count = _buffer_viewer->get_stage_information();

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

    debug_lines[2]->set_text(fmt::format(
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

    debug_lines[3]->set_text(fmt::format(
        "Scene:   {:4.0f} MB VRAM |  {:3d} tex |  {:4d} geoms |  {:4d} nodes |  {:7d} vertices",

        (scene_tex_size / (1024.0*1024.0)),
        tex_count,
        _analyzer->get_num_geoms(),
        _analyzer->get_num_nodes(),
        _analyzer->get_num_vertices()
        ));

    LVecBase3f sun_vector(0);
    if (pipeline->get_plugin_mgr()->is_plugin_enabled("scattering"))
    {
        sun_vector = dynamic_pointer_cast<rpplugins::ScatteringPlugin>(pipeline->get_plugin_mgr()->get_instance("scattering"))->get_sun_vector();
    }

    const NodePath& camera = Globals::base->get_cam();
    const NodePath& render = Globals::base->get_render();
    const LPoint3& camera_global_pos = camera.get_pos(render);

    debug_lines[4]->set_text(fmt::format(
        "Time: {} ({:1.3f}) |  Sun  {:0.2f} {:0.2f} {:0.2f} |  X {:4.2f}  Y {:4.2f}  Z {:4.2f} |  {:2d} tasks |  scheduled: {:2d}",

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

    debug_lines_5_text += fmt::format("   |  HPR  ({:3.1f} {:3.1f} {:3.1f})  | {:4d} x {:4d} pixels @ {:4d} x {:4d} | {:3d} x {:3d} tiles",
        camera_global_hpr[0],
        camera_global_hpr[1],
        camera_global_hpr[2],
        Globals::native_resolution.get_x(),
        Globals::native_resolution.get_y(),
        Globals::resolution.get_x(),
        Globals::resolution.get_y(),
        light_mgr->get_num_tiles().get_x(),
        light_mgr->get_num_tiles().get_y());
    debug_lines[5]->set_text(debug_lines_5_text);

    return task ? AsyncTask::DS_again : AsyncTask::DS_done;
}

}
