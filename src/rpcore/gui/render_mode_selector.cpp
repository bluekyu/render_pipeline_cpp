/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2018 Center of Human-centered Interaction for Coexistence.
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

#include "rpcore/gui/render_mode_selector.hpp"

#include <boost/algorithm/string.hpp>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/pluginbase/manager.hpp"
#include "render_pipeline/rpcore/gui/checkbox_collection.hpp"
#include "render_pipeline/rpcore/gui/labeled_checkbox.hpp"

#include "rplibs/yaml.hpp"

namespace rpcore {

RenderModeSelector::RenderModeSelector(RenderPipeline* pipeline, NodePath parent):
    DraggableWindow(690, 340, "Select render mode", parent), pipeline_(pipeline)
{
    create_components();
    hide();
}

RenderModeSelector::~RenderModeSelector() = default;

void RenderModeSelector::create_components()
{
    DraggableWindow::create_components();
    content_node_ = node_.attach_new_node("content");
    populate_content();
}

void RenderModeSelector::populate_content()
{
    collection_.reset();
    boxes_.clear();
    content_node_.node()->remove_all_children();

    // Reload config each time the window is opened so its easy to add new
    // render modes
    YAML::Node config;
    if (!rplibs::load_yaml_file("/$$rpconfig/debugging.yaml", config))
        return;

    auto debugger_content = content_node_.attach_new_node("RenderModes");
    debugger_content.set_z(-20);
    debugger_content.set_x(20);

    std::vector<std::tuple<std::string, std::string, std::string, bool>> render_modes = { { "Default", "", "", false } };

    try
    {
        // Read modes from configuration
        for (auto&& mode: config["render_modes"])
        {
            render_modes.push_back({
                mode["name"].as<std::string>(),
                mode["key"].as<std::string>(),
                mode["requires"].as<std::string>(""),
                mode["special"].as<bool>(false)});
        }
    }
    catch (...)
    {
        error("'name' or 'key' does NOT exist in debugging.yaml file");
        return;
    }

    collection_ = std::make_unique<CheckboxCollection>();

    int max_column_height = 9;

    for (int idx = 0, idx_end = static_cast<int>(render_modes.size()); idx < idx_end; ++idx)
    {
        const int offs_y = (idx % max_column_height) * 24 + 35;
        const int offs_x = (idx / max_column_height) * 220;
        bool enabled = true;

        const auto& mode = std::get<0>(render_modes[idx]);
        const auto& mode_id = std::get<1>(render_modes[idx]);
        const auto& requires_plugin = std::get<2>(render_modes[idx]);
        const auto& special = std::get<3>(render_modes[idx]);

        if (!requires_plugin.empty())
        {
            if (!pipeline_->get_plugin_mgr()->is_plugin_enabled(requires_plugin))
                enabled = false;
        }

        auto box = std::make_unique<LabeledCheckbox>(debugger_content, offs_x, offs_y,
            std::bind(&RenderModeSelector::set_render_mode, this, mode_id, special, std::placeholders::_1),
            (mode_id == selected_mode_), boost::to_upper_copy(mode), 14, true, LVecBase3(0.4f), 230, enabled);
        collection_->add(box->get_checkbox());
        boxes_.push_back(std::move(box));
    }
}

void RenderModeSelector::set_render_mode(const std::string& mode_id, bool special, bool value)
{
    if (!value)
        return;

    auto& defines = pipeline_->get_stage_mgr()->get_defines();

    std::vector<std::string> to_remove;
    for (const auto& key_val: defines)
    {
        const auto& define = key_val.first;
        if (define.find("_RM_") == 0)
            to_remove.push_back(define);
    }
    for (const auto& define: to_remove)
        defines.erase(define);

    if (mode_id.empty())
    {
        defines["ANY_DEBUG_MODE"] = "0";
    }
    else
    {
        // Don't activate the generic debugging mode for special modes. This
        // is for modes like luminance which expect the scene to be rendered
        // unaltered.
        defines["ANY_DEBUG_MODE"] = special ? "0" : "1";
        defines["_RM_" + mode_id] = "1";
    }

    selected_mode_ = mode_id;
    pipeline_->reload_shaders();
}

}
