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

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>
#include <render_pipeline/rplibs/ordered_map.hpp>

namespace rpcore {

class RenderPipeline;
class BaseType;
class DayBaseType;

/**
 * This class manages all plugins. It provides functionality to load plugin
 * settings, trigger callbacks on plugins, initialize the plugin instances
 * and much more.
 */
class RENDER_PIPELINE_DECL PluginManager : public RPObject
{
public:
    using PluginIDType = std::string;
    using SettingsDataType = rplibs::OrderedMap<PluginIDType, std::shared_ptr<BaseType>>;
    using DaySettingsDataType = rplibs::OrderedMap<PluginIDType, std::shared_ptr<DayBaseType>>;

    typedef std::unique_ptr<BasePlugin> (PluginCreatorType)(RenderPipeline&);

public:
    /** Release handle of loaded DLLs. */
    static void release_all_dll();

    /**
     * Constructs a new manager with no plugins loaded. To load settings
     * and plugins, call load().
     */
    PluginManager(RenderPipeline& pipeline);

    ~PluginManager();

    /**
     * Loads all plugins and their settings, and also constructs instances
     * of the main plugin classes for all enabled plugins.
     */
    void load();

    /**
     * Disables a plugin, given its plugin_id. This will remove it from
     * the list of enabled plugins, if it ever was there.
     */
    void disable_plugin(const PluginIDType& plugin_id);

    /** Unloads all plugins. */
    void unload();

    /** Main update method. */
    void update() {}

    /**
     * Loads the base settings of all plugins, even of disabled plugins.
     * This is required to verify all overrides.
     */
    void load_base_settings(const Filename& plugin_dir);

    /**
     * Internal method to load all settings of a plugin, given its plugin
     * id and path to the plugin base directory.
     */
    void load_plugin_settings(const PluginIDType& plugin_id, const Filename& plugin_pth);

    /**
     * Loads an override file for the settings, which contains values to
     * override the settings with.
     */
    void load_setting_overrides(const Filename& override_path);

    /**
     * Loads an override file for the daytime settings, which contains
     * values to override the settings with.
     */
    void load_daytime_overrides(const Filename& override_path);

    /** Saves all overrides to the given file. */
    void save_overrides(const Filename& override_path);

    /** Saves all time of day overrides to the given file. */
    void save_daytime_overrides(const Filename& override_path);

    /**
     * Sets whether a plugin is enabled or not, thus should be loaded when
     * the pipeline starts or not.
     */
    void set_plugin_enabled(const PluginIDType& plugin_id, bool enabled);

    /** Resets all settings of a given plugin. */
    void reset_plugin_settings(const PluginIDType& plugin_id);

    /** Returns whether a plugin is currently enabled and loaded. */
    bool is_plugin_enabled(const PluginIDType& plugin_id) const;

    /** Initializes all plugin settings as a define, so they can be queried in a shader. */
    void init_defines();

    /**
     * @return  Sorted list of plugin ID regardless of enabling.
     */
    const std::vector<PluginIDType>& get_plugin_ids() const;

    /**
     * @return  Enabled plugin list.
     */
    const std::unordered_set<PluginIDType>& get_enabled_plugins() const;

    const BasePlugin::PluginInfo& get_plugin_info(const PluginIDType& plugin_id) const noexcept;
    BaseType* get_setting_handle(const PluginIDType& plugin_id, const std::string& setting_id);
    const BaseType* get_setting_handle(const PluginIDType& plugin_id, const std::string& setting_id) const;
    const DaySettingsDataType* get_day_settings(const PluginIDType& plugin_id) const;

    /** Get plugin instance. */
    BasePlugin* get_instance(const PluginIDType& plugin_id) const;

    /** Trigger hook. */
    ///@{
    void on_load();
    void on_stage_setup();
    void on_post_stage_setup();
    void on_pipeline_created();
    void on_prepare_scene(NodePath scene);
    void on_pre_render_update();
    void on_post_render_update();
    void on_shader_reload();
    void on_window_resized();
    void on_unload();

    /**
     * Update single setting value after changing.
     */
    void on_setting_changed(const PluginIDType& plugin_id, const std::string& setting_id);

    /**
     * Update multiple setting values after changing.
     *
     * @param[in]   { { plugin_id, { setting_id, ... } }, ... } map
     */
    void on_setting_changed(const std::unordered_map<PluginIDType, std::unordered_set<std::string>>& settings_map);
    ///@}

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
