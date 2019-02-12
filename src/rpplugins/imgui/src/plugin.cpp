/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "rpplugins/imgui/plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>
#include <boost/filesystem/operations.hpp>

#include <fmt/ostream.h>

#include <imgui.h>

#include <buttonThrower.h>
#include <buttonRegistry.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/util/filesystem.hpp>

#include "panda3d_imgui.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::ImGuiPlugin)

namespace rpplugins {

rpcore::BasePlugin::RequrieType ImGuiPlugin::require_plugins_;

ImGuiPlugin::ImGuiPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING)
{
}

ImGuiPlugin::~ImGuiPlugin() = default;

rpcore::BasePlugin::RequrieType& ImGuiPlugin::get_required_plugins(void) const
{
    return require_plugins_;
}

ImGuiContext* ImGuiPlugin::get_context() const
{
    return p3d_imgui_->get_context();
}

NodePath ImGuiPlugin::get_root() const
{
    return p3d_imgui_->get_root();
}

const std::vector<Filename>& ImGuiPlugin::get_dropped_files() const
{
    return p3d_imgui_->get_dropped_files();
}

const LVecBase2& ImGuiPlugin::get_dropped_point() const
{
    return p3d_imgui_->get_dropped_point();
}

void ImGuiPlugin::on_load()
{
    p3d_imgui_ = std::make_unique<Panda3DImGui>(rpcore::Globals::base->get_win(), rpcore::Globals::base->get_pixel_2d());

    const std::string style_setting = boost::any_cast<std::string>(get_setting("style"));
    if (style_setting == "classic")
        p3d_imgui_->setup_style(Panda3DImGui::Style::classic);
    else if (style_setting == "light")
        p3d_imgui_->setup_style(Panda3DImGui::Style::light);
    else
        p3d_imgui_->setup_style(Panda3DImGui::Style::dark);

    p3d_imgui_->setup_geom();

    p3d_imgui_->setup_shader(rpcore::RPLoader::load_shader({ get_shader_resource("imgui.vert.glsl"), get_shader_resource("imgui.frag.glsl") }));

    auto default_font_path = rppanda::convert_path(Filename(boost::any_cast<std::string>(get_setting("default_font_path"))));
    if (boost::filesystem::exists(default_font_path))
    {
        const float font_size = boost::any_cast<float>(get_setting("default_font_size"));
        p3d_imgui_->setup_font(default_font_path.generic_string().c_str(), font_size);
    }
    else
    {
        error(fmt::format("Failed to find font: {}", default_font_path));
        p3d_imgui_->setup_font();
    }

    p3d_imgui_->setup_event();
    setup_button();

    on_window_resized();

    // ig_loop has process_events and 50 sort.
    add_task([this](auto) { p3d_imgui_->new_frame_imgui(); return AsyncTask::DS_cont; }, "ImGuiPlugin::new_frame", 0);
    add_task([this](auto) { p3d_imgui_->render_imgui(); return AsyncTask::DS_cont; }, "ImGuiPlugin::render", 40);

    accept(SETUP_CONTEXT_EVENT_NAME, std::bind(&ImGuiPlugin::setup_context, this, std::placeholders::_1));

    // register file drop
    if (boost::any_cast<bool>(get_setting("os_file_drop")))
        p3d_imgui_->enable_file_drop();
}

void ImGuiPlugin::on_window_resized()
{
    const int w = rpcore::Globals::native_resolution[0];
    const int h = rpcore::Globals::native_resolution[1];
    p3d_imgui_->on_window_resized(LVecBase2(static_cast<float>(w), static_cast<float>(h)));
}

void ImGuiPlugin::on_unload()
{
    remove_all_tasks();
    p3d_imgui_.reset();
}

void ImGuiPlugin::setup_context(const Event* ev)
{
    const auto num_parameters = ev->get_num_parameters();
    if (num_parameters != 1)
    {
        error(fmt::format("Invalid number of task: {}", num_parameters));
        return;
    }

    auto param = ev->get_parameter(0);
    if (!param.is_typed_ref_count())
    {
        error(fmt::format("Invalid type of parameter."));
        return;
    }

    auto ptr = param.get_typed_ref_count_value();
    if (!ptr->is_of_type(rppanda::FunctionalTask::get_class_type()))
    {
        error(fmt::format("Type of parameter is NOT rppanda::FunctionalTask"));
        return;
    }

    auto task = DCAST(rppanda::FunctionalTask, ptr);
    task->set_user_data(std::shared_ptr<ImGuiContext>(get_context(), [](auto) {}));
    add_task(task);
}

void ImGuiPlugin::setup_button()
{
    if (auto bt = rpcore::Globals::base->get_button_thrower())
    {
        ButtonThrower* bt_node = DCAST(ButtonThrower, bt.node());
        std::string ev_name;

        ev_name = bt_node->get_button_down_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-button-down";
            bt_node->set_button_down_event(ev_name);
        }
        accept(ev_name, [this](const Event* ev) { p3d_imgui_->on_button_down_or_up(ButtonRegistry::ptr()->get_button(ev->get_parameter(0).get_string_value()), true); });

        ev_name = bt_node->get_button_up_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-button-up";
            bt_node->set_button_up_event(ev_name);
        }
        accept(ev_name, [this](const Event* ev) { p3d_imgui_->on_button_down_or_up(ButtonRegistry::ptr()->get_button(ev->get_parameter(0).get_string_value()), false); });

        ev_name = bt_node->get_keystroke_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-keystroke";
            bt_node->set_keystroke_event(ev_name);
        }
        accept(ev_name, [this](const Event* ev) { p3d_imgui_->on_keystroke(ev->get_parameter(0).get_wstring_value()[0]); });
    }
}

}
