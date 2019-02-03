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
#include "render_pipeline/rpcore/pluginbase/setting_types.hpp"
#include "render_pipeline/rppanda/stdpy/file.hpp"
#include "render_pipeline/rppanda/util/filesystem.hpp"

#include "rplibs/yaml.hpp"

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
    struct PluginDataType
    {
        std::unique_ptr<BasePlugin> instance;
        BasePlugin::PluginInfo plugin_info;
        SettingsDataType settings;
        DaySettingsDataType day_settings;
    };

public:
    Impl(PluginManager& self, RenderPipeline& pipeline);

    void unload();

    /** Internal method to load a plugin. */
    std::unique_ptr<BasePlugin> load_plugin(const PluginIDType& plugin_id);

    void save_overrides(const Filename& override_path);
    void save_daytime_overrides(const Filename& override_path);

    void load_plugin_settings(const PluginIDType& plugin_id, const Filename& plugin_pth);

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

    void on_setting_changed(const PluginIDType& plugin_id, const std::string& setting_id);
    void on_setting_changed(const std::unordered_map<PluginIDType, std::unordered_set<std::string>>& settings_map);

public:
    static std::unordered_map<PluginIDType, std::function<PluginCreatorType>> plugin_creators_;

    PluginManager& self_;
    RenderPipeline& pipeline_;
    boost::filesystem::path plugin_dir_;

    bool requires_daytime_settings_;

    std::vector<PluginIDType> plugin_ids_;
    std::unordered_set<PluginIDType> enabled_plugins_;

    std::unordered_map<PluginIDType, PluginDataType> plugin_data_map_;
};

std::unordered_map<PluginManager::PluginIDType, std::function<PluginManager::PluginCreatorType>> PluginManager::Impl::plugin_creators_;

PluginManager::Impl::Impl(PluginManager& self, RenderPipeline& pipeline): self_(self), pipeline_(pipeline)
{
    // Used by the plugin configurator and to only load the required data
    requires_daytime_settings_ = true;
}

void PluginManager::Impl::unload()
{
    self_.debug(fmt::format("Clear plugins data: {}", plugin_data_map_.size()));

    self_.on_unload();

    for (auto&& id_data: plugin_data_map_)
    {
        // delete plugin instance.
        id_data.second.instance.reset();
    }
    plugin_data_map_.clear();

    enabled_plugins_.clear();

    // NOTE: DLLs are not unloaded, yet.
}

std::unique_ptr<BasePlugin> PluginManager::Impl::load_plugin(const PluginIDType& plugin_id)
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

void PluginManager::Impl::save_overrides(const Filename& override_path)
{
    std::string output =
        "\n# Render Pipeline Plugin Configuration\n"
         "# Instead of editing this file, prefer to use the Plugin Configurator\n"
         "# Any formatting and comments will be lost\n\n"
         "enabled:\n";

    std::vector<PluginIDType> enabled_plugin_ids;

    enabled_plugin_ids.reserve(plugin_data_map_.size());

    for (const auto& id_data: plugin_data_map_)
    {
        bool found = enabled_plugins_.find(id_data.first) != enabled_plugins_.end();
        enabled_plugin_ids.push_back((found ? "A" : "B") + id_data.first);
    }

    std::sort(enabled_plugin_ids.begin(), enabled_plugin_ids.end());

    for (const auto& tag: enabled_plugin_ids)
    {
        bool found = tag[0] == 'A';
        const std::string& id = tag.substr(1);
        output += fmt::format("   {}- {}\n", found ? " " : " # ", id);
    }

    output += "\n\noverrides:\n";

    for (const auto& id : plugin_ids_)
    {
        output += std::string(4, ' ') + id + ":\n";
        for (const auto& setting_id_handle : plugin_data_map_.at(id).settings.get<1>())
            output += std::string(8, ' ') + fmt::format("{}: {}\n", setting_id_handle.key, setting_id_handle.value->get_value_as_string());
        output += "\n";
    }

    try
    {
        (*rppanda::open_write_file(override_path, false, true)) << output;
    }
    catch (const std::exception& err)
    {
        self_.error(std::string("Error writing plugin setting file: ") + err.what());
    }
}

void PluginManager::Impl::save_daytime_overrides(const Filename& override_path)
{
    // TODO
    self_.error("Not implemented.");
}

void PluginManager::Impl::load_plugin_settings(const PluginIDType& plugin_id, const Filename& plugin_pth)
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

    auto& plugin_data = plugin_data_map_[plugin_id];

    const auto& info_node = config["information"];
    plugin_data.plugin_info = BasePlugin::PluginInfo{
        info_node["category"].as<std::string>("empty_category"),
        info_node["name"].as<std::string>("empty_name"),
        info_node["author"].as<std::string>("empty_author"),
        info_node["version"].as<std::string>(""),
        info_node["description"].as<std::string>("empty_description") };

    if (config["settings"] && config["settings"].size() != 0 && !config["settings"].IsSequence())
        self_.fatal("Invalid plugin configuration, did you miss '!!omap' in 'settings'?");

    auto& settings_map = plugin_data.settings.get<1>();
    for (auto settings_node: config["settings"])
    {
        // XXX: omap of yaml-cpp is list.
        for (auto key_val: settings_node)
        {
            const auto& key = key_val.first.as<std::string>();

            auto found = settings_map.find(key);
            try
            {
                if (found == settings_map.end())
                    settings_map.insert({ key, make_setting_from_data(key_val.second) });
                else
                    settings_map.replace(found, { key, make_setting_from_data(key_val.second) });
            }
            catch (const std::exception& err)
            {
                self_.error(err.what());
            }
        }
    }

    if (requires_daytime_settings_)
    {
        auto& day_settings_map = plugin_data.day_settings.get<1>();
        for (auto daytime_settings_node: config["daytime_settings"])
        {
            // XXX: omap of yaml-cpp is list.
            for (auto key_val : daytime_settings_node)
            {
                const auto& key = key_val.first.as<std::string>();

                auto found = day_settings_map.find(key);
                try
                {
                    if (found == day_settings_map.end())
                        day_settings_map.insert({ key, make_daysetting_from_data(key_val.second) });
                    else
                        day_settings_map.replace(found, { key, make_daysetting_from_data(key_val.second) });
                }
                catch (const std::exception& err)
                {
                    self_.error(err.what());
                }
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
        if (plugin_data_map_.find(plugin_id) == plugin_data_map_.end())
        {
            self_.warn(fmt::format("Unknown plugin in plugin ({}) config.", plugin_id));
            continue;
        }

        for (const auto& id_val: id_settings.second)
        {
            const std::string setting_id(id_val.first.as<std::string>());
            auto& plugin_setting_map = plugin_data_map_.at(plugin_id).settings.get<1>();
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
        plugin_data_map_.at(plugin_id).instance->on_load();
    }
}

void PluginManager::Impl::on_stage_setup()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_stage_setup() in plugin ({}).", plugin_id));
        plugin_data_map_.at(plugin_id).instance->on_stage_setup();
    }
}

void PluginManager::Impl::on_post_stage_setup()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_post_stage_setup() in plugin ({}).", plugin_id));
        plugin_data_map_.at(plugin_id).instance->on_post_stage_setup();
    }
}

void PluginManager::Impl::on_pipeline_created()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_pipeline_created() in plugin ({}).", plugin_id));
        plugin_data_map_.at(plugin_id).instance->on_pipeline_created();
    }
}

void PluginManager::Impl::on_prepare_scene(NodePath scene)
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_prepare_scene(NodePath) in plugin ({}).", plugin_id));
        plugin_data_map_.at(plugin_id).instance->on_prepare_scene(scene);
    }
}

void PluginManager::Impl::on_pre_render_update()
{
    for (const auto& plugin_id: enabled_plugins_)
        plugin_data_map_.at(plugin_id).instance->on_pre_render_update();
}

void PluginManager::Impl::on_post_render_update()
{
    for (const auto& plugin_id: enabled_plugins_)
        plugin_data_map_.at(plugin_id).instance->on_post_render_update();
}

void PluginManager::Impl::on_shader_reload()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_shader_reload() in plugin ({}).", plugin_id));
        plugin_data_map_.at(plugin_id).instance->on_shader_reload();
    }
}

void PluginManager::Impl::on_window_resized()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_window_resized() in plugin ({}).", plugin_id));
        plugin_data_map_.at(plugin_id).instance->on_window_resized();
    }
}

void PluginManager::Impl::on_unload()
{
    for (const auto& plugin_id: enabled_plugins_)
    {
        self_.trace(fmt::format("Call on_unload() in plugin ({}).", plugin_id));
        plugin_data_map_.at(plugin_id).instance->on_unload();
    }
}

void PluginManager::Impl::on_setting_changed(const PluginIDType& plugin_id, const std::string& setting_id)
{
    auto setting = self_.get_setting_handle(plugin_id, setting_id);
    if (!setting)
    {
        self_.warn(fmt::format("Got invalid setting value: {} / {}", plugin_id, setting_id));
        return;
    }

    if (enabled_plugins_.find(plugin_id) == enabled_plugins_.end())
        return;

    if (setting->is_runtime() || setting->is_shader_runtime())
        plugin_data_map_.at(plugin_id).instance->on_setting_changed(setting_id);

    if (setting->is_shader_runtime())
    {
        self_.init_defines();
        pipeline_.get_stage_mgr()->write_autoconfig();
        plugin_data_map_.at(plugin_id).instance->reload_shaders();
    }
}

void PluginManager::Impl::on_setting_changed(const std::unordered_map<PluginIDType, std::unordered_set<std::string>>& settings_map)
{
    bool at_least_shader_runtime = false;
    for (const auto& plugin_id_settings: settings_map)
    {
        const auto& plugin_id = plugin_id_settings.first;

        if (enabled_plugins_.find(plugin_id) == enabled_plugins_.end())
            continue;

        auto plugin_data_found = plugin_data_map_.find(plugin_id);
        if (plugin_data_found == plugin_data_map_.end())
            continue;

        auto& plugin_setting_map = plugin_data_found->second.settings.get<1>();
        auto& plugin_instance = plugin_data_map_.at(plugin_id).instance;

        for (const auto& setting_id: plugin_id_settings.second)
        {
            auto setting_found = plugin_setting_map.find(setting_id);
            if (setting_found == plugin_setting_map.end())
                continue;

            const auto is_runtime = setting_found->value->is_runtime();
            const auto is_shader_runtime = setting_found->value->is_shader_runtime();

            if (is_runtime || is_shader_runtime)
                plugin_instance->on_setting_changed(setting_id);

            at_least_shader_runtime |= is_shader_runtime;
        }
    }

    if (at_least_shader_runtime)
    {
        self_.init_defines();
        pipeline_.get_stage_mgr()->write_autoconfig();

        if (settings_map.size() > 1)
            pipeline_.reload_shaders();
        else
            plugin_data_map_.at(settings_map.begin()->first).instance->reload_shaders();
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

    // cache IDs
    impl_->plugin_ids_.reserve(impl_->plugin_data_map_.size());
    for (const auto& id_data : impl_->plugin_data_map_)
        impl_->plugin_ids_.push_back(id_data.first);
    std::sort(impl_->plugin_ids_.begin(), impl_->plugin_ids_.end());

    trace(fmt::format("Found {} plugin settings.", impl_->plugin_data_map_.size()));

    debug("Creating plugin instances ..");
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        if (impl_->plugin_data_map_.find(plugin_id) == impl_->plugin_data_map_.end())
        {
            error(fmt::format("Cannot find plugin ({}) in plugin directory.", plugin_id));
            disable_plugin(plugin_id);
            continue;
        }

        auto handle = impl_->load_plugin(plugin_id);

        if (handle)
            impl_->plugin_data_map_.at(plugin_id).instance.swap(handle);
        else
            disable_plugin(plugin_id);
    }
}

void PluginManager::disable_plugin(const PluginIDType& plugin_id)
{
    warn(fmt::format("Disabling plugin ({}).", plugin_id));
    if (impl_->enabled_plugins_.find(plugin_id) != impl_->enabled_plugins_.end())
        impl_->enabled_plugins_.erase(plugin_id);

    for (const auto& id_data: impl_->plugin_data_map_)
    {
        const auto& plugins = id_data.second.instance->get_required_plugins();
        if (std::find(plugins.begin(), plugins.end(), plugin_id) != plugins.end())
            disable_plugin(id_data.first);
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

void PluginManager::load_plugin_settings(const PluginIDType& plugin_id, const Filename& plugin_pth)
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
            const auto& plugin_day_setting_map = impl_->plugin_data_map_.at(plugin_id).day_settings.get<1>();

            auto found = plugin_day_setting_map.find(setting_id);
            if (found == plugin_day_setting_map.end())
            {
                warn(fmt::format("Unknown daytime override: {}:{}", plugin_id, setting_id));
                continue;
            }

            std::vector<std::vector<LVecBase2>> control_points;
            for (const auto& curve_index_points: id_points.second)
            {
                control_points.push_back({});
                auto& current_points = control_points.back();
                for (const auto& points: curve_index_points)
                {
                    current_points.push_back(LVecBase2(points[0].as<PN_stdfloat>(), points[1].as<PN_stdfloat>()));
                }
            }

            found->value->set_control_points(control_points);
        }
    }
}

void PluginManager::save_overrides(const Filename& override_path)
{
    impl_->save_overrides(override_path);
}

void PluginManager::save_daytime_overrides(const Filename& override_path)
{
    impl_->save_daytime_overrides(override_path);
}

void PluginManager::set_plugin_enabled(const PluginIDType& plugin_id, bool enabled)
{
    if (enabled)
        impl_->enabled_plugins_.insert(plugin_id);
    else
        impl_->enabled_plugins_.erase(plugin_id);
}

void PluginManager::reset_plugin_settings(const PluginIDType& plugin_id)
{
    for (auto&& setting_id_handle : impl_->plugin_data_map_.at(plugin_id).settings.get<1>())
        setting_id_handle.value->set_value(setting_id_handle.value->get_default());
}

bool PluginManager::is_plugin_enabled(const PluginIDType& plugin_id) const
{
    return impl_->enabled_plugins_.find(plugin_id) != impl_->enabled_plugins_.end();
}

void PluginManager::init_defines()
{
    for (const auto& plugin_id: impl_->enabled_plugins_)
    {
        const auto& pluginsettings_vector = impl_->plugin_data_map_.at(plugin_id).settings.get<0>();
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

BasePlugin* PluginManager::get_instance(const PluginIDType& plugin_id) const
{
    return impl_->plugin_data_map_.at(plugin_id).instance.get();
}

const std::vector<PluginManager::PluginIDType>& PluginManager::get_plugin_ids() const
{
    return impl_->plugin_ids_;
}

const std::unordered_set<PluginManager::PluginIDType>& PluginManager::get_enabled_plugins() const
{
    return impl_->enabled_plugins_;
}

BaseType* PluginManager::get_setting_handle(const PluginIDType& plugin_id, const std::string& setting_id)
{
    auto plugin_data_found = impl_->plugin_data_map_.find(plugin_id);
    if (plugin_data_found == impl_->plugin_data_map_.end())
        return nullptr;

    auto& plugin_setting_map = plugin_data_found->second.settings.get<1>();
    auto setting_found = plugin_setting_map.find(setting_id);
    if (setting_found == plugin_setting_map.end())
        return nullptr;

    return setting_found->value.get();
}

const BaseType* PluginManager::get_setting_handle(const PluginIDType& plugin_id, const std::string& setting_id) const
{
    auto plugin_data_found = impl_->plugin_data_map_.find(plugin_id);
    if (plugin_data_found == impl_->plugin_data_map_.end())
        return nullptr;

    auto& plugin_setting_map = plugin_data_found->second.settings.get<1>();
    auto setting_found = plugin_setting_map.find(setting_id);
    if (setting_found == plugin_setting_map.end())
        return nullptr;

    return setting_found->value.get();
}

const BasePlugin::PluginInfo& PluginManager::get_plugin_info(const PluginIDType& plugin_id) const noexcept
{
    try
    {
        return impl_->plugin_data_map_.at(plugin_id).plugin_info;
    }
    catch (...)
    {
        static const BasePlugin::PluginInfo null;
        error(fmt::format("Plugin ({}) does NOT exist!", plugin_id));
        return null;
    }
}

const PluginManager::DaySettingsDataType* PluginManager::get_day_settings(const PluginIDType& plugin_id) const
{
    auto found = impl_->plugin_data_map_.find(plugin_id);
    if (found == impl_->plugin_data_map_.end())
        return nullptr;
    else
        return &found->second.day_settings;
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

void PluginManager::on_setting_changed(const PluginIDType& plugin_id, const std::string& setting_id)
{
    impl_->on_setting_changed(plugin_id, setting_id);
}

void PluginManager::on_setting_changed(const std::unordered_map<PluginIDType, std::unordered_set<std::string>>& settings_map)
{
    impl_->on_setting_changed(settings_map);
}

}
