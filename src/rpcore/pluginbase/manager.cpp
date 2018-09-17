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

#include "render_pipeline/rpcore/pluginbase/manager.hpp"

#include <unordered_set>
#include <regex>

#ifdef _WIN32
// TODO: remove fs::canonical bug is fixed.
#include <Shlwapi.h>
#endif

#include <boost/dll/import.hpp>

#include <fmt/ostream.h>

#include "render_pipeline/rpcore/mount_manager.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/pluginbase/day_setting_types.hpp"
#include "render_pipeline/rppanda/stdpy/file.hpp"
#include "render_pipeline/rppanda/util/filesystem.hpp"

#include "rplibs/yaml.hpp"

#include "rpcore/pluginbase/setting_types.hpp"

namespace rpcore {

// TODO: change to fs::canonical after canonical bug is fixed.
static boost::filesystem::path get_canonical_path(const boost::filesystem::path& path)
{
#ifdef _WIN32
    wchar_t path_buffer[32767 + 1] = {};
    PathCanonicalizeW(path_buffer, boost::filesystem::absolute(path).native().c_str());
    boost::filesystem::path result(path_buffer);
    if (!boost::filesystem::exists(result))
        throw boost::filesystem::filesystem_error("boost::filesystem::canonical",
            boost::system::error_code(
                boost::system::errc::no_such_file_or_directory, boost::system::generic_category()));

    return result;
#else
    return boost::filesystem::canonical(path);
#endif
}

class PluginManager::Impl
{
public:
    Impl(PluginManager& self, RenderPipeline& pipeline);

    void unload();

    /** Internal method to load a plugin. */
    std::unique_ptr<BasePlugin> load_plugin(const std::string& plugin_id);

    void load_plugin_settings(const std::string& plugin_id, const Filename& plugin_pth);

    void load_setting_overrides(const Filename& override_path);

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

public:
    static std::unordered_map<std::string, std::function<PluginCreatorType>> plugin_creators_;

    PluginManager& self_;
    RenderPipeline& pipeline_;
    boost::filesystem::path plugin_dir_;

    bool requires_daytime_settings_;

    std::unordered_map<std::string, std::unique_ptr<BasePlugin>> instances_;

    std::unordered_set<std::string> enabled_plugins_;

    std::unordered_map<std::string, BasePlugin::PluginInfo> plugin_info_map_;

    ///< { plugin-id, SettingsType }
    std::unordered_map<std::string, SettingsDataType> settings_;

    ///< { plugin-id, DaySettingsType }
    std::unordered_map<std::string, DaySettingsDataType> day_settings_;
};

std::unordered_map<std::string, std::function<PluginManager::PluginCreatorType>> PluginManager::Impl::plugin_creators_;

PluginManager::Impl::Impl(PluginManager& self, RenderPipeline& pipeline): self_(self), pipeline_(pipeline)
{
    // Used by the plugin configurator and to only load the required data
    requires_daytime_settings_ = true;
}

void PluginManager::Impl::unload()
{
    self_.debug(fmt::format("Unloading all plugins: {}", instances_.size()));

    self_.on_unload();

    for (auto&& id_handle: instances_)
    {
        // delete plugin instance.
        id_handle.second.reset();
    }
    instances_.clear();

    settings_.clear();
    day_settings_.clear();
    enabled_plugins_.clear();

    // NOTE: DLLs are not unloaded, yet.
}

std::unique_ptr<BasePlugin> PluginManager::Impl::load_plugin(const std::string& plugin_id)
{
    boost::filesystem::path plugin_path = get_canonical_path(plugin_dir_ / plugin_id);

#if defined(RENDER_PIPELINE_BUILD_CFG_POSTFIX)
    plugin_path = plugin_path / ("rpplugins_" + plugin_id + RENDER_PIPELINE_BUILD_CFG_POSTFIX);
#else
    plugin_path = plugin_path / ("rpplugins_" + plugin_id);
#endif

    self_.trace(fmt::format("Importing shared library file ({}) from {}{}", plugin_id, plugin_path.string(), boost::dll::shared_library::suffix().string()));

    try
    {
        auto result = Impl::plugin_creators_.insert({plugin_id, boost::dll::import_alias<PluginCreatorType>(
            plugin_path,
            "create_plugin",
            boost::dll::load_mode::rtld_global | boost::dll::load_mode::append_decorations)});

        if (!result.second)
        {
            self_.error(fmt::format("Plugin '{}' was already loaded.", plugin_id));
            return nullptr;
        }

        auto instance = result.first->second(pipeline_);

        const auto& plugin_pipeline_info = instance->get_pipeline_info();

        std::smatch version_match;
        if (!std::regex_match(plugin_pipeline_info.version, version_match, std::regex("^(\\d+)\\.(\\d+)\\.(\\d+)$")))
        {
            self_.error(fmt::format("Plugin '{}' has invalid Render Pipeline version: {}", plugin_id, plugin_pipeline_info.version));
            return nullptr;
        }

        const int version_major = std::stoi(version_match[1].str());
        const int version_minor = std::stoi(version_match[2].str());

        int pipeline_major;
        int pipeline_minor;
        int pipeline_patch;
        pipeline_.get_version(pipeline_major, pipeline_minor, pipeline_patch);

        if (version_major != pipeline_major)
        {
            self_.error(fmt::format("Pipeline version ({}) in Plugin '{}' is incompatible with current version ({})",
                plugin_pipeline_info.version, plugin_id, pipeline_.get_version()));
            return nullptr;
        }

        if (version_minor != pipeline_minor)
        {
            self_.warn(fmt::format("Pipeline version ({}) in Plugin '{}' may be incompatible with current version ({})",
                plugin_pipeline_info.version, plugin_id, pipeline_.get_version()));
        }

        self_.trace(fmt::format("Pipeline information in Plugin '{}': Version ({}), Commit ({})",
            plugin_id, plugin_pipeline_info.version, plugin_pipeline_info.commit));

        for (const auto& required_plugin: instance->get_required_plugins())
        {
            if (enabled_plugins_.find(required_plugin) == enabled_plugins_.end())
            {
                if (enabled_plugins_.find(plugin_id) != enabled_plugins_.end())
                {
                    self_.warn(fmt::format("Cannot load {} since it requires {}", plugin_id, required_plugin));
                    return nullptr;
                }
                break;
            }
        }
        return instance;
    }
    catch (const boost::system::system_error& err)
    {
        self_.error(fmt::format("Failed to import plugin or to create plugin ({}).", plugin_id));
        self_.error(fmt::format("Loaded path: {}", plugin_path.string()));
        self_.error(fmt::format("Boost::DLL Error message: {} ({})", err.what(), err.code().value()));
        return nullptr;
    }
    catch (const std::exception& err)
    {
        self_.error(fmt::format("Failed to import plugin or to create plugin ({}).", plugin_id));
        self_.error(fmt::format("Loaded path: {}", plugin_path.string()));
        self_.error(fmt::format("Plugin error message: {}", err.what()));
        return nullptr;
    }
}

void PluginManager::Impl::load_plugin_settings(const std::string& plugin_id, const Filename& plugin_pth)
{
    const Filename& config_file = rppanda::join(plugin_pth, "config.yaml");

    YAML::Node config;
    if (!rplibs::load_yaml_file(config_file, config))
        return;

    // When you don't specify anything in the settings, instead of
    // returning an empty dictionary, pyyaml returns None

    if (!config["information"])
    {
        self_.error(fmt::format("Plugin ({}) configuration does NOT have information.", plugin_id));
        return;
    }

    const auto& info_node = config["information"];
    plugin_info_map_.insert_or_assign(plugin_id, BasePlugin::PluginInfo{
        info_node["category"].as<std::string>("empty_category"),
        info_node["name"].as<std::string>("empty_name"),
        info_node["author"].as<std::string>("empty_author"),
        info_node["version"].as<std::string>(""),
        info_node["description"].as<std::string>("empty_description") });

    if (config["settings"] && config["settings"].size() != 0 && !config["settings"].IsSequence())
        self_.fatal("Invalid plugin configuration, did you miss '!!omap' in 'settings'?");

    auto& settings_map = settings_[plugin_id].get<1>();
    for (auto settings_node: config["settings"])
    {
        // XXX: omap of yaml-cpp is list.
        for (auto key_val: settings_node)
        {
            const auto& key = key_val.first.as<std::string>();

            auto found = settings_map.find(key);
            if (found == settings_map.end())
                settings_map.insert({ key, make_setting_from_data(key_val.second) });
            else
                settings_map.replace(found, { key, make_setting_from_data(key_val.second) });
        }
    }

    if (requires_daytime_settings_)
    {
        auto& day_settings_map = day_settings_[plugin_id].get<1>();
        for (auto daytime_settings_node: config["daytime_settings"])
        {
            // XXX: omap of yaml-cpp is list.
            for (auto key_val : daytime_settings_node)
            {
                const auto& key = key_val.first.as<std::string>();

                auto found = day_settings_map.find(key);
                if (found == day_settings_map.end())
                    day_settings_map.insert({ key, make_daysetting_from_data(key_val.second) });
                else
                    day_settings_map.replace(found, { key, make_daysetting_from_data(key_val.second) });
            }
        }
    }
}

void PluginManager::Impl::load_setting_overrides(const Filename& override_path)
{
    self_.trace(fmt::format("Loading setting overrides from '{}'", override_path.to_os_specific()));

    YAML::Node overrides;
    if (!rplibs::load_yaml_file(override_path, overrides))
    {
        self_.warn("Failed to load overrides");
        return;
    }

    for (const auto& plugin_id: overrides["enabled"])
    {
        enabled_plugins_.insert(plugin_id.as<std::string>());
    }

    for (const auto& id_settings: overrides["overrides"])
    {
        const std::string plugin_id(id_settings.first.as<std::string>());
        if (settings_.find(plugin_id) == settings_.end())
        {
            self_.warn(fmt::format("Unknown plugin in plugin ({}) config.", plugin_id));
            continue;
        }

        for (const auto& id_val: id_settings.second)
        {
            const std::string setting_id(id_val.first.as<std::string>());
            auto& plugin_setting_map = settings_.at(plugin_id).get<1>();
            auto found = plugin_setting_map.find(setting_id);
            if (found == plugin_setting_map.end())
            {
                self_.warn(fmt::format("Unknown override: {}:{}", plugin_id, setting_id));
                continue;
            }
            found->value->set_value(id_val.second);
        }
    }
}

void PluginManager::Impl::on_load()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_load() in plugin ({}).", plugin_id));
        instances_.at(plugin_id)->on_load();
    }
}

void PluginManager::Impl::on_stage_setup()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_stage_setup() in plugin ({}).", plugin_id));
        instances_.at(plugin_id)->on_stage_setup();
    }
}

void PluginManager::Impl::on_post_stage_setup()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_post_stage_setup() in plugin ({}).", plugin_id));
        instances_.at(plugin_id)->on_post_stage_setup();
    }
}

void PluginManager::Impl::on_pipeline_created()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_pipeline_created() in plugin ({}).", plugin_id));
        instances_.at(plugin_id)->on_pipeline_created();
    }
}

void PluginManager::Impl::on_prepare_scene(NodePath scene)
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_prepare_scene(NodePath) in plugin ({}).", plugin_id));
        instances_.at(plugin_id)->on_prepare_scene(scene);
    }
}

void PluginManager::Impl::on_pre_render_update()
{
    for (const auto& plugin_id: enabled_plugins_)
        instances_.at(plugin_id)->on_pre_render_update();
}

void PluginManager::Impl::on_post_render_update()
{
    for (const auto& plugin_id: enabled_plugins_)
        instances_.at(plugin_id)->on_post_render_update();
}

void PluginManager::Impl::on_shader_reload()
{
    for (const auto& plugin_id: enabled_plugins_)
        instances_.at(plugin_id)->on_shader_reload();
}

void PluginManager::Impl::on_window_resized()
{
    for (const auto& plugin_id: enabled_plugins_)
        instances_.at(plugin_id)->on_window_resized();
}

void PluginManager::Impl::on_unload()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_unload() in plugin ({}).", plugin_id));
        instances_.at(plugin_id)->on_unload();
    }
}

// ************************************************************************************************

void PluginManager::release_all_dll()
{
    Impl::plugin_creators_.clear();
}

PluginManager::PluginManager(RenderPipeline& pipeline): RPObject("PluginManager"), impl_(std::make_unique<Impl>(*this, pipeline))
{
}

PluginManager::~PluginManager()
{
    trace("Destructing PluginManager");

    unload();

    // NOTE: DLLs are not unloaded.
}

void PluginManager::load()
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
            disable_plugin(plugin_id);
            continue;
        }

        auto handle = impl_->load_plugin(plugin_id);

        if (handle)
            impl_->instances_[plugin_id] = std::move(handle);
        else
            disable_plugin(plugin_id);
    }
}

void PluginManager::disable_plugin(const std::string& plugin_id)
{
    warn(fmt::format("Disabling plugin ({}).", plugin_id));
    if (impl_->enabled_plugins_.find(plugin_id) != impl_->enabled_plugins_.end())
        impl_->enabled_plugins_.erase(plugin_id);

    for (const auto& id_handle: impl_->instances_)
    {
        const auto& plugins = id_handle.second->get_required_plugins();
        if (std::find(plugins.begin(), plugins.end(), plugin_id) != plugins.end())
            disable_plugin(id_handle.second->get_plugin_id());
    }
}

void PluginManager::load_base_settings(const Filename& plugin_dir)
{
    trace(fmt::format("Loading base setting from '{}'", plugin_dir.c_str()));

    impl_->plugin_dir_ = rppanda::convert_path(plugin_dir);
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
    impl_->load_plugin_settings(plugin_id, plugin_pth);
}

void PluginManager::load_daytime_overrides(const Filename& override_path)
{
    trace(fmt::format("Loading daytime overrides from '{}'", override_path.to_os_specific()));

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
            const auto& plugin_day_setting_map = impl_->day_settings_.at(plugin_id).get<1>();

            auto found = plugin_day_setting_map.find(setting_id);
            if (found == plugin_day_setting_map.end())
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

            found->value->set_control_points(control_points);
        }
    }
}

bool PluginManager::is_plugin_enabled(const std::string& plugin_id) const
{
    return impl_->enabled_plugins_.find(plugin_id) != impl_->enabled_plugins_.end();
}

void PluginManager::init_defines()
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        const auto& pluginsettings_vector = impl_->settings_.at(plugin_id).get<0>();
        auto& defines = impl_->pipeline_.get_stage_mgr()->get_defines();
        defines["HAVE_PLUGIN_" + plugin_id] = std::string("1");
        for (const auto& id_setting: pluginsettings_vector)
        {
            if (id_setting.value->is_shader_runtime() || !id_setting.value->is_runtime())
            {
                // Only store settings which either never change, or trigger
                // a shader reload when they change
                id_setting.value->add_defines(plugin_id, id_setting.key, defines);
            }
        }
    }
}

BasePlugin* PluginManager::get_instance(const std::string& plugin_id) const
{
    return impl_->instances_.at(plugin_id).get();
}

size_t PluginManager::get_enabled_plugins_count() const noexcept
{
    return impl_->enabled_plugins_.size();
}

const PluginManager::SettingsDataType& PluginManager::get_setting(const std::string& setting_id) const
{
    return impl_->settings_.at(setting_id);
}

const BasePlugin::PluginInfo& PluginManager::get_plugin_info(const std::string& plugin_id) const noexcept
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

const std::unordered_map<std::string, PluginManager::DaySettingsDataType>& PluginManager::get_day_settings() const
{
    return impl_->day_settings_;
}

void PluginManager::unload() { impl_->unload(); }
void PluginManager::load_setting_overrides(const Filename& override_path) { impl_->load_setting_overrides(override_path); }
void PluginManager::on_load() { impl_->on_load(); }
void PluginManager::on_stage_setup() { impl_->on_stage_setup(); }
void PluginManager::on_post_stage_setup() { impl_->on_post_stage_setup(); }
void PluginManager::on_pipeline_created() { impl_->on_pipeline_created(); }
void PluginManager::on_prepare_scene(NodePath scene) { impl_->on_prepare_scene(scene); }
void PluginManager::on_pre_render_update() { impl_->on_pre_render_update(); }
void PluginManager::on_post_render_update() { impl_->on_post_render_update(); }
void PluginManager::on_shader_reload() { impl_->on_shader_reload(); }
void PluginManager::on_window_resized() { impl_->on_window_resized(); }
void PluginManager::on_unload() { impl_->on_unload(); }

}
