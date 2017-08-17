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

#pragma once

#include <nodePath.h>

#include <vector>

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rpcore/pluginbase/day_setting_types.hpp>

#define RENDER_PIPELINE_PLUGIN_CREATOR(PLUGIN_TYPE) \
    static std::shared_ptr<::rpcore::BasePlugin> rpcpp_plugin_creator__(::rpcore::RenderPipeline& pipeline) \
    { \
        return std::make_shared<PLUGIN_TYPE>(pipeline); \
    } \
    BOOST_DLL_ALIAS(::rpcpp_plugin_creator__, create_plugin)

// ************************************************************************************************

namespace boost {
class any;
}

namespace rpcore {

class RenderPipeline;
class RenderStage;

class RENDER_PIPELINE_DECL BasePlugin: public RPObject
{
public:
    using RequrieType = std::vector<std::string>;

    struct PluginInfo
    {
        std::string category;
        std::string name;
        std::string author;
        std::string version;
        std::string description;
    };

public:
    BasePlugin(RenderPipeline& pipeline, const std::string& plugin_id);
    BasePlugin(const BasePlugin&) = delete;
    BasePlugin(BasePlugin&&) = delete;

    virtual ~BasePlugin() {}

    BasePlugin& operator=(const BasePlugin&) = delete;
    BasePlugin& operator=(BasePlugin&&) = delete;

    /** Returns the path to the root directory of the plugin. */
    virtual Filename get_base_path() const;

    /** Converts a local path from the plugins resource directory into an absolute path.*/
    Filename get_resource(const Filename& pth) const;

    /** Converts a local path from the plugins shader directory into an absolute path. */
    Filename get_shader_resource(const Filename& pth) const;

    void add_stage(const std::shared_ptr<RenderStage>& stage);

    const boost::any& get_setting(const std::string& setting_id, const std::string& plugin_id="") const;

    DayBaseType::ValueType get_daytime_setting(const std::string& setting_id, const std::string& plugin_id="") const;

    const std::shared_ptr<BasePlugin>& get_plugin_instance(const std::string& plugin_id) const;

    /** Returns whether a plugin is enabled and loaded, given is plugin id. */
    bool is_plugin_enabled(const std::string& plugin_id) const;

    /** Reloads all shaders of the plugin. */
    void reload_shaders();

    const std::string& get_plugin_id() const;
    const PluginInfo& get_plugin_info() const;

    virtual RequrieType& get_required_plugins() const = 0;

    /** Trigger hook. */
    ///@{
    virtual void on_load() {}
    virtual void on_stage_setup() {}
    virtual void on_post_stage_setup() {}
    virtual void on_pipeline_created() {}
    virtual void on_prepare_scene(NodePath scene) {}
    virtual void on_pre_render_update() {}
    virtual void on_post_render_update() {}
    virtual void on_shader_reload() {}
    virtual void on_window_resized() {}
    virtual void on_unload() {}
    ///@}

protected:
    RenderPipeline& pipeline_;
    const std::string plugin_id_;

private:
    std::vector<std::shared_ptr<RenderStage>> assigned_stages_;
};

// ************************************************************************************************
inline const std::string& BasePlugin::get_plugin_id() const
{
    return plugin_id_;
}

}
