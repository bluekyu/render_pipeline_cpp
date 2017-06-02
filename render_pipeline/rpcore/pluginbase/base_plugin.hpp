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
    virtual ~BasePlugin(void) {}

    /** Returns the path to the root directory of the plugin. */
    virtual std::string get_base_path(void) const;

    /** Converts a local path from the plugins resource directory into an absolute path.*/
    std::string get_resource(const std::string& pth) const;

    /** Converts a local path from the plugins shader directory into an absolute path. */
    std::string get_shader_resource(const std::string& pth) const;

    void add_stage(const std::shared_ptr<RenderStage>& stage);

    const boost::any& get_setting(const std::string& setting_id, const std::string& plugin_id="") const;

    DayBaseType::ValueType get_daytime_setting(const std::string& setting_id, const std::string& plugin_id="") const;

    const std::shared_ptr<BasePlugin>& get_plugin_instance(const std::string& plugin_id) const;

    /** Returns whether a plugin is enabled and loaded, given is plugin id. */
    bool is_plugin_enabled(const std::string& plugin_id) const;

    /** Reloads all shaders of the plugin. */
    void reload_shaders(void);

    const std::string& get_plugin_id(void) const;
    const PluginInfo& get_plugin_info(void) const;

    virtual RequrieType& get_required_plugins(void) const = 0;

    /** Trigger hook. */
    ///@{
    virtual void on_load(void) {}
    virtual void on_stage_setup(void) {}
    virtual void on_post_stage_setup(void) {}
    virtual void on_pipeline_created(void) {}
    virtual void on_prepare_scene(NodePath scene) {}
    virtual void on_pre_render_update(void) {}
    virtual void on_post_render_update(void) {}
    virtual void on_shader_reload(void) {}
    virtual void on_window_resized(void) {}
    virtual void on_unload(void) {}
    ///@}

protected:
    RenderPipeline& pipeline_;
    const std::string plugin_id_;

private:
    std::vector<std::shared_ptr<RenderStage>> assigned_stages_;
};

// ************************************************************************************************
inline const std::string& BasePlugin::get_plugin_id(void) const
{
    return plugin_id_;
}

}
