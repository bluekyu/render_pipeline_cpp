#pragma once

#include <memory>
#include <map>
#include <unordered_map>
#include <functional>

#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.h>
#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpcore {

class RenderPipeline;
class BaseType;
class DayBaseType;

/**
 * This class manages all plugins. It provides functionality to load plugin
 * settings, trigger callbacks on plugins, initialize the plugin instances
 * and much more.
 */
class RPCPP_DECL PluginManager: public RPObject
{
public:
    // TODO: FIX map --> OrderedDict (use boost multi-index)
    using SettingsDataType = std::map<std::string, std::shared_ptr<BaseType>>;
    using DaySettingsDataType = std::map<std::string, std::shared_ptr<DayBaseType>>;

    typedef std::shared_ptr<BasePlugin> (PluginCreatorType)(RenderPipeline&);

public:
    /**
     * Constructs a new manager with no plugins loaded. To load settings
     * and plugins, call load().
     */
    PluginManager(RenderPipeline& pipeline);

    ~PluginManager(void);

    /**
     * Loads all plugins and their settings, and also constructs instances
     * of the main plugin classes for all enabled plugins.
     */
    void load(void);

    /**
     * Disables a plugin, given its plugin_id. This will remove it from
     * the list of enabled plugins, if it ever was there.
     */
    void disable_plugin(const std::string& plugin_id);

    /** Unloads all plugins. */
    void unload(void);

    /** Main update method. */
    void update(void) {}

    /**
     * Loads the base settings of all plugins, even of disabled plugins.
     * This is required to verify all overrides.
     */
    void load_base_settings(const std::string& plugin_dir);

    /**
     * Internal method to load all settings of a plugin, given its plugin
     * id and path to the plugin base directory.
     */
    void load_plugin_settings(const std::string& plugin_id, const std::string& plugin_pth);

    /**
     * Loads an override file for the settings, which contains values to
     * override the settings with.
     */
    void load_setting_overrides(const std::string& override_path);

    /**
     * Loads an override file for the daytime settings, which contains
     * values to override the settings with.
     */
    void load_daytime_overrides(const std::string& override_path);

    /** Returns whether a plugin is currently enabled and loaded. */
    bool is_plugin_enabled(const std::string& plugin_id) const;

    /** Initializes all plugin settings as a define, so they can be queried in a shader. */
    void init_defines(void);

    size_t get_enabled_plugins_count(void) const;

    const BasePlugin::PluginInfo& get_plugin_info(const std::string& plugin_id) const;
    const SettingsDataType& get_setting(const std::string& setting_id) const;
    const std::unordered_map<std::string, DaySettingsDataType>& get_day_settings(void) const;

    /** Get plugin instance. */
    const std::shared_ptr<BasePlugin>& get_instance(const std::string& plugin_id) const;

    /** Trigger hook. */
    ///@{
    void on_load(void);
    void on_stage_setup(void);
    void on_post_stage_setup(void);
    void on_pipeline_created(void);
    void on_prepare_scene(NodePath scene);
    void on_pre_render_update(void);
    void on_post_render_update(void);
    void on_shader_reload(void);
    void on_window_resized(void);
    void on_unload(void);
    ///@}

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
