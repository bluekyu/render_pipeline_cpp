#include "render_pipeline/rpcore/pluginbase/manager.hpp"

#include <unordered_set>

#include <boost/detail/winapi/dll.hpp>
#include <boost/dll/import.hpp>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rpcore/mount_manager.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/pluginbase/day_setting_types.hpp"
#include "render_pipeline/rppanda/stdpy/file.hpp"

#include "rplibs/yaml.hpp"

#include "rpcore/pluginbase/setting_types.hpp"

namespace rpcore {

class PluginManager::Impl
{
public:
    Impl(PluginManager& self, RenderPipeline& pipeline);

    void unload(void);

    /** Internal method to load a plugin. */
    std::shared_ptr<BasePlugin> load_plugin(const std::string& plugin_id);

public:
    PluginManager& self_;
    RenderPipeline& pipeline_;
    std::string plugin_dir_;

    bool requires_daytime_settings_;

    std::unordered_map<std::string, std::shared_ptr<BasePlugin>> instances_;

    std::unordered_set<std::string> enabled_plugins_;
    std::unordered_map<std::string, std::function<PluginCreatorType>> plugin_creators_;

    std::unordered_map<std::string, BasePlugin::PluginInfo> plugin_info_map_;

    ///< { plugin-id, SettingsType }
    std::unordered_map<std::string, SettingsDataType> settings_;

    ///< { plugin-id, DaySettingsType }
    std::unordered_map<std::string, DaySettingsDataType> day_settings_;
};

PluginManager::Impl::Impl(PluginManager& self, RenderPipeline& pipeline): self_(self), pipeline_(pipeline)
{
    // Used by the plugin configurator and to only load the required data
    requires_daytime_settings_ = true;
}

void PluginManager::Impl::unload(void)
{
    self_.debug("Unloading all plugins");

    self_.on_unload();

    for (auto& id_handle: instances_)
    {
        auto count = id_handle.second.use_count();
        if (count != 1)
            self_.warn(fmt::format("Plugin ({}) is used as {} on somewhere before unloading.", id_handle.first, count));

        // delete plugin instance.
        id_handle.second.reset();
    }
    instances_.clear();

    settings_.clear();
    day_settings_.clear();
    enabled_plugins_.clear();

    // unload plugins.
    plugin_creators_.clear();
}

std::shared_ptr<BasePlugin> PluginManager::Impl::load_plugin(const std::string& plugin_id)
{
    boost::filesystem::path plugin_path(plugin_dir_);

#if defined(RENDER_PIPELINE_BUILD_CFG_POSTFIX)
    plugin_path = boost::filesystem::absolute(plugin_path / plugin_id / ("rpplugin_" + plugin_id + RENDER_PIPELINE_BUILD_CFG_POSTFIX));
#else
    plugin_path = boost::filesystem::absolute(plugin_path / plugin_id / ("rpplugin_" + plugin_id));
#endif

    self_.trace(fmt::format("Importing shared library file ({}) from {}{}", plugin_id, plugin_path.string(), boost::dll::shared_library::suffix().string()));

    try
    {
        plugin_creators_[plugin_id] = boost::dll::import_alias<PluginCreatorType>(
            plugin_path,
            "create_plugin",
            boost::dll::load_mode::append_decorations);

        return plugin_creators_.at(plugin_id)(pipeline_);
    }
    catch (const std::exception& err)
    {
        self_.error(fmt::format("Failed to import plugin or to create plugin ({}). Error message: {}", plugin_id, err.what()));
        return std::shared_ptr<BasePlugin>();
    }

    // TODO: implement
}

// ************************************************************************************************

PluginManager::PluginManager(RenderPipeline& pipeline): RPObject("PluginManager"), impl_(std::make_unique<Impl>(*this, pipeline))
{
}

PluginManager::~PluginManager(void)
{
    unload();
}

void PluginManager::load(void)
{
    debug("Loading plugin settings");
    load_base_settings("/$$rp/rpplugins");
    load_setting_overrides("/$$rpconfig/plugins.yaml");

    if (impl_->requires_daytime_settings_)
        load_daytime_overrides("/$$rpconfig/daytime.yaml");

    debug("Creating plugin instances ..");
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        if (impl_->settings_.find(plugin_id) == impl_->settings_.end())
        {
            error(fmt::format("Cannot find plugin ({}) in plugin directory.", plugin_id));
            continue;
        }

        const std::shared_ptr<BasePlugin>& handle = impl_->load_plugin(plugin_id);

        if (handle)
            impl_->instances_[plugin_id] = handle;
        else
            disable_plugin(plugin_id);
    }
}

void PluginManager::disable_plugin(const std::string& plugin_id)
{
    warn(fmt::format("Disabling plugin ({}).", plugin_id));
    if (impl_->enabled_plugins_.find(plugin_id) != impl_->enabled_plugins_.end())
        impl_->enabled_plugins_.erase(plugin_id);

    for (auto& id_handle: impl_->instances_)
    {
        const auto& plugins = id_handle.second->get_required_plugins();
        if (std::find(plugins.begin(), plugins.end(), plugin_id) != plugins.end())
            disable_plugin(id_handle.second->get_plugin_id());
    }
}

void PluginManager::unload(void)
{
    impl_->unload();
}

void PluginManager::load_base_settings(const Filename& plugin_dir)
{
    trace(fmt::format("Loading base setting from '{}'", plugin_dir.c_str()));

    impl_->plugin_dir_ = MountManager::convert_to_physical_path(plugin_dir);
    if (impl_->plugin_dir_.empty())
    {
        error(fmt::format("Cannot find plugin directory ({}).", plugin_dir.c_str()));
        return;
    }

    for (const auto& entry: rppanda::listdir(plugin_dir))
    {
        const Filename& abspath = rppanda::join(plugin_dir, entry);
        if (rppanda::isdir(abspath) && (entry != "__pycache__" || entry != "plugin_prefab"))
        {
            load_plugin_settings(entry, abspath);
        }
    }
}

void PluginManager::load_plugin_settings(const std::string& plugin_id, const Filename& plugin_pth)
{
    const Filename& config_file = rppanda::join(plugin_pth, "config.yaml");

    YAML::Node config;
    if (!rplibs::load_yaml_file(config_file, config))
        return;

    // When you don't specify anything in the settings, instead of
    // returning an empty dictionary, pyyaml returns None

    if (!config["information"])
    {
        error(fmt::format("Plugin ({}) configuration does NOT have information.", plugin_id));
        return;
    }

    const auto& info_node = config["information"];
#if _MSC_VER >= 1900
    impl_->plugin_info_map_.insert_or_assign(plugin_id, BasePlugin::PluginInfo {
        info_node["category"].as<std::string>("empty_category"),
        info_node["name"].as<std::string>("empty_name"),
        info_node["author"].as<std::string>("empty_author"),
        info_node["version"].as<std::string>("empty_version"),
        info_node["description"].as<std::string>("empty_description") });
#else
    impl_->plugin_info_map_[plugin_id] = BasePlugin::PluginInfo {
        info_node["category"].as<std::string>("empty_category"),
        info_node["name"].as<std::string>("empty_name"),
        info_node["author"].as<std::string>("empty_author"),
        info_node["version"].as<std::string>("empty_version"),
        info_node["description"].as<std::string>("empty_description") };
#endif

    if (config["settings"] && config["settings"].size() != 0 && !config["settings"].IsSequence())
        fatal("Invalid plugin configuration, did you miss '!!omap'?");

    SettingsDataType& settings = impl_->settings_[plugin_id];
    for (auto& settings_node: config["settings"])
    {
        // XXX: omap of yaml-cpp is list.
        for (auto& key_val: settings_node)
        {
            settings[key_val.first.as<std::string>()] = make_setting_from_data(key_val.second);
        }
    }

    if (impl_->requires_daytime_settings_)
    {
        DaySettingsDataType& day_settings = impl_->day_settings_[plugin_id];
        for (auto& daytime_settings_node: config["daytime_settings"])
        {
            // XXX: omap of yaml-cpp is list.
            for (auto& key_val: daytime_settings_node)
            {
                day_settings[key_val.first.as<std::string>()] = make_daysetting_from_data(key_val.second);
            }
        }
    }
}

void PluginManager::load_setting_overrides(const std::string& override_path)
{
    trace(fmt::format("Loading setting overrides from '{}'", override_path));

    YAML::Node overrides;
    if (!rplibs::load_yaml_file(override_path, overrides))
    {
        warn("Failed to load overrides");
        return;
    }

    for (const auto& plugin_id: overrides["enabled"])
    {
        impl_->enabled_plugins_.insert(plugin_id.as<std::string>());
    }

    for (const auto& id_settings: overrides["overrides"])
    {
        const std::string plugin_id(id_settings.first.as<std::string>());
        if (impl_->settings_.find(plugin_id) == impl_->settings_.end())
        {
            warn(fmt::format("Unknown plugin in plugin ({}) config.", plugin_id));
            continue;
        }

        for (const auto& id_val: id_settings.second)
        {
            const std::string setting_id(id_val.first.as<std::string>());
            const auto& plugin_setting = impl_->settings_.at(plugin_id);
            if (plugin_setting.find(setting_id) == plugin_setting.end())
            {
                warn(fmt::format("Unknown override: {}:{}", plugin_id, setting_id));
                continue;
            }
            plugin_setting.at(setting_id)->set_value(id_val.second);
        }
    }
}

void PluginManager::load_daytime_overrides(const std::string& override_path)
{
    trace(fmt::format("Loading daytime overrides from '{}'", override_path));

    YAML::Node overrides;
    if (!rplibs::load_yaml_file(override_path, overrides))
    {
        warn("Failed to load daytime overrides");
        return;
    }

    for (const auto& key_val: overrides["control_points"])
    {
        const std::string plugin_id(key_val.first.as<std::string>());

        if (impl_->enabled_plugins_.find(plugin_id) == impl_->enabled_plugins_.end())
            continue;

        for (const auto& id_points: key_val.second)
        {
            const std::string setting_id(id_points.first.as<std::string>());
            const auto& plugin_day_setting = impl_->day_settings_.at(plugin_id);
            if (plugin_day_setting.find(setting_id) == plugin_day_setting.end())
            {
                warn(fmt::format("Unknown daytime override: {}:{}", plugin_id, setting_id));
                continue;
            }

            std::vector<std::vector<LVecBase2f>> control_points;
            for (const auto& curve_index_points: id_points.second)
            {
                control_points.push_back({});
                auto& current_points = control_points.back();
                for (const auto& points: curve_index_points)
                {
                    current_points.push_back(LVecBase2f(points[0].as<float>(), points[1].as<float>()));
                }
            }

            plugin_day_setting.at(setting_id)->set_control_points(control_points);
        }
    }
}

bool PluginManager::is_plugin_enabled(const std::string& plugin_id) const
{
    return impl_->enabled_plugins_.find(plugin_id) != impl_->enabled_plugins_.end();
}

void PluginManager::init_defines(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        const auto& pluginsettings = impl_->settings_.at(plugin_id);
        auto& defines = impl_->pipeline_.get_stage_mgr()->get_defines();
        defines[std::string("HAVE_PLUGIN_" + plugin_id)] = std::string("1");
        for (const auto& id_setting: pluginsettings)
        {
            if (id_setting.second->is_shader_runtime() || !id_setting.second->is_runtime())
            {
                // Only store settings which either never change, or trigger
                // a shader reload when they change
                id_setting.second->add_defines(plugin_id, id_setting.first, defines);
            }
        }
    }
}

const std::shared_ptr<BasePlugin>& PluginManager::get_instance(const std::string& plugin_id) const
{
    return impl_->instances_.at(plugin_id);
}

size_t PluginManager::get_enabled_plugins_count(void) const NOEXCEPT
{
    return impl_->enabled_plugins_.size();
}

const PluginManager::SettingsDataType& PluginManager::get_setting(const std::string& setting_id) const
{
    return impl_->settings_.at(setting_id);
}

const BasePlugin::PluginInfo& PluginManager::get_plugin_info(const std::string& plugin_id) const NOEXCEPT
{
    try
    {
        return impl_->plugin_info_map_.at(plugin_id);
    }
    catch (...)
    {
        static const BasePlugin::PluginInfo null;
        error(fmt::format("Plugin ({}) does NOT exist!", plugin_id));
        return null;
    }
}

const std::unordered_map<std::string, PluginManager::DaySettingsDataType>& PluginManager::get_day_settings(void) const
{
    return impl_->day_settings_;
}

void PluginManager::on_load(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        trace(fmt::format("Call on_load() in plugin ({}).", plugin_id));
        impl_->instances_.at(plugin_id)->on_load();
    }
}

void PluginManager::on_stage_setup(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        trace(fmt::format("Call on_stage_setup() in plugin ({}).", plugin_id));
        impl_->instances_.at(plugin_id)->on_stage_setup();
    }
}

void PluginManager::on_post_stage_setup(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        trace(fmt::format("Call on_post_stage_setup() in plugin ({}).", plugin_id));
        impl_->instances_.at(plugin_id)->on_post_stage_setup();
    }
}

void PluginManager::on_pipeline_created(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        trace(fmt::format("Call on_pipeline_created() in plugin ({}).", plugin_id));
        impl_->instances_.at(plugin_id)->on_pipeline_created();
    }
}

void PluginManager::on_prepare_scene(NodePath scene)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        trace(fmt::format("Call on_prepare_scene() in plugin ({}).", plugin_id));
        impl_->instances_.at(plugin_id)->on_prepare_scene(scene);
    }
}

void PluginManager::on_pre_render_update(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
        impl_->instances_.at(plugin_id)->on_pre_render_update();
}

void PluginManager::on_post_render_update(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
        impl_->instances_.at(plugin_id)->on_post_render_update();
}

void PluginManager::on_shader_reload(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
        impl_->instances_.at(plugin_id)->on_shader_reload();
}

void PluginManager::on_window_resized(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
        impl_->instances_.at(plugin_id)->on_window_resized();
}

void PluginManager::on_unload(void)
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        trace(fmt::format("Call on_unload() in plugin ({}).", plugin_id));
        impl_->instances_.at(plugin_id)->on_unload();
    }
}

}
