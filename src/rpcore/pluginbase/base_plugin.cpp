#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include <filename.h>

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/pluginbase/manager.h>
#include <render_pipeline/rpcore/stage_manager.h>
#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/pluginbase/day_manager.h>

#include "rplibs/yaml.hpp"

#include "setting_types.h"

namespace rpcore {

BasePlugin::BasePlugin(RenderPipeline& pipeline, const std::string& plugin_id): RPObject(std::string("plugin:") + plugin_id), pipeline_(pipeline), plugin_id_(plugin_id)
{
    // TODO: implement
    //self._set_debug_color("magenta", "bright")
}

std::string BasePlugin::get_base_path(void) const
{
    return Filename("/$$rp/rpplugins") / plugin_id_;
}

std::string BasePlugin::get_resource(const std::string& pth) const
{
    return Filename::from_os_specific(get_base_path()) / "resources" / pth;
}

std::string BasePlugin::get_shader_resource(const std::string& pth) const
{
    return Filename::from_os_specific(get_base_path()) / "shader" / pth;
}

void BasePlugin::add_stage(const std::shared_ptr<RenderStage>& stage)
{
    pipeline_.get_stage_mgr()->add_stage(stage);
    assigned_stages_.push_back(stage);
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

const std::shared_ptr<BasePlugin>& BasePlugin::get_plugin_instance(const std::string& plugin_id) const
{
    return pipeline_.get_plugin_mgr()->get_instance(plugin_id);
}

bool BasePlugin::is_plugin_enabled(const std::string& plugin_id) const
{
    return pipeline_.get_plugin_mgr()->is_plugin_enabled(plugin_id);
}

void BasePlugin::reload_shaders(void)
{
    for (const auto& stage: assigned_stages_)
        stage->reload_shaders();
}

const BasePlugin::PluginInfo& BasePlugin::get_plugin_info(void) const
{
    return pipeline_.get_plugin_mgr()->get_plugin_info(plugin_id_);
}

}
