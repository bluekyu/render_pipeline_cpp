#pragma once

#include <vector>

#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.h>
#include <render_pipeline/rpcore/pluginbase/day_setting_types.h>

namespace boost {
class any;
}

namespace rpcore {

class RenderPipeline;
class RenderStage;

class RPCPP_DECL BasePlugin: public RPObject
{
public:
    using RequrieType = std::vector<std::string>;

    struct PluginInfo
    {
        const char* category;
        const char* id;
        const char* name;
        const char* author;
        const char* version;
        const char* description;
    };

public:
    BasePlugin(RenderPipeline& pipeline, const PluginInfo& plugin_info);
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

    const char* get_plugin_category(void) const;
    const char* get_plugin_id(void) const;
    const char* get_name(void) const;
    const char* get_author(void) const;
    const char* get_description(void) const;
    const char* get_version(void) const;

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
    const PluginInfo& plugin_info_;

private:
    std::vector<std::shared_ptr<RenderStage>> assigned_stages_;
};

// ************************************************************************************************
inline const char* BasePlugin::get_plugin_category(void) const
{
    return plugin_info_.category;
}

inline const char* BasePlugin::get_plugin_id(void) const
{
    return plugin_info_.id;
}

inline const char* BasePlugin::get_name(void) const
{
    return plugin_info_.name;
}

inline const char* BasePlugin::get_author(void) const
{
    return plugin_info_.author;
}

inline const char* BasePlugin::get_description(void) const
{
    return plugin_info_.description;
}

inline const char* BasePlugin::get_version(void) const
{
    return plugin_info_.version;
}

}
