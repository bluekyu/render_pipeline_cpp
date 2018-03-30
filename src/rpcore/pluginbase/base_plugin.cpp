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

#include "render_pipeline/rpcore/pluginbase/base_plugin.hpp"

#include <boost/dll/import.hpp>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/pluginbase/manager.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/render_stage.hpp"
#include "render_pipeline/rpcore/pluginbase/day_manager.hpp"
#include "render_pipeline/rppanda/util/filesystem.hpp"

#include "rplibs/yaml.hpp"

#include "rpcore/pluginbase/setting_types.hpp"

namespace rpcore {

class BasePlugin::Impl
{
public:
    std::vector<std::unique_ptr<RenderStage>> assigned_stages_;
    std::vector<std::shared_ptr<boost::dll::shared_library>> shared_libs_;
};

BasePlugin::BasePlugin(RenderPipeline& pipeline, const std::string& plugin_id):
    RPObject(std::string("plugin:") + plugin_id), pipeline_(pipeline), plugin_id_(plugin_id),
    impl_(std::make_unique<Impl>())
{
    trace(fmt::format("Constructing '{}' plugin", plugin_id_));
}

BasePlugin::~BasePlugin()
{
    trace(fmt::format("Destructing '{}' plugin", plugin_id_));

    auto manager = pipeline_.get_stage_mgr();
    if (!manager)
        return;

    for (const auto& stage: impl_->assigned_stages_)
        manager->remove_stage(stage.get());
}

Filename BasePlugin::get_base_path() const
{
    return Filename("/$$rp/rpplugins") / plugin_id_;
}

Filename BasePlugin::get_resource(const Filename& pth) const
{
    return get_base_path() / "resources" / pth;
}

Filename BasePlugin::get_shader_resource(const Filename& pth) const
{
    return get_base_path() / "shader" / pth;
}

void BasePlugin::add_stage(std::unique_ptr<RenderStage> stage)
{
    pipeline_.get_stage_mgr()->add_stage(stage.get());
    impl_->assigned_stages_.push_back(std::move(stage));
}

const boost::any& BasePlugin::get_setting(const std::string& setting_id, const std::string& plugin_id) const
{
    return pipeline_.get_plugin_mgr()->get_setting(plugin_id.empty() ? plugin_id_ : plugin_id).at(setting_id)->get_value();
}

DayBaseType::ValueType BasePlugin::get_daytime_setting(const std::string& setting_id, const std::string& plugin_id) const
{
    const auto& handle = pipeline_.get_plugin_mgr()->get_day_settings().at(plugin_id.empty() ? plugin_id_ : plugin_id).at(setting_id);
    return handle->get_scaled_value_at(pipeline_.get_daytime_mgr()->get_time());
}

BasePlugin* BasePlugin::get_plugin_instance(const std::string& plugin_id) const
{
    return pipeline_.get_plugin_mgr()->get_instance(plugin_id);
}

bool BasePlugin::is_plugin_enabled(const std::string& plugin_id) const
{
    return pipeline_.get_plugin_mgr()->is_plugin_enabled(plugin_id);
}

void BasePlugin::reload_shaders()
{
    for (const auto& stage: impl_->assigned_stages_)
        stage->reload_shaders();
}

const BasePlugin::PluginInfo& BasePlugin::get_plugin_info() const
{
    return pipeline_.get_plugin_mgr()->get_plugin_info(plugin_id_);
}

bool BasePlugin::load_shared_library(const Filename& path)
{
    auto lib_path = rppanda::convert_path(path);

    trace(fmt::format("Loading shared library file ({}) in plugin ({})", lib_path.generic_string(), plugin_id_));

    try
    {
        impl_->shared_libs_.push_back(std::make_shared<boost::dll::shared_library>(
            lib_path,
            boost::dll::load_mode::append_decorations));
    }
    catch (const boost::system::system_error& err)
    {
        error(fmt::format("Failed to load shared library in plugin ({}).", plugin_id_));
        error(fmt::format("Loaded path: {}", lib_path.string()));
        error(fmt::format("Boost::DLL Error message: {} ({})", err.what(), err.code().value()));
        return false;
    }

    return true;
}

}
