#include "render_pipeline/rppanda/showbase/loader.hpp"

#include <audioManager.h>
#include <loader.h>
#include <audioLoadRequest.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/stdpy/file.hpp"

#include "rppanda/showbase/config_rppanda_showbase.hpp"

namespace rppanda {

class Loader::Impl
{
public:
    Impl(ShowBase& base);

    void pre_load_model(LoaderOptions& this_options, bool& this_ok_missing,
        boost::optional<bool> no_cache, bool allow_instance, boost::optional<bool> ok_missing);

public:
    ShowBase& base_;
    ::Loader* loader_;
};

Loader::Impl::Impl(ShowBase& base): base_(base)
{
    loader_ = ::Loader::get_global_ptr();
}

void Loader::Impl::pre_load_model(LoaderOptions& this_options, bool& this_ok_missing,
    boost::optional<bool> no_cache, bool allow_instance, boost::optional<bool> ok_missing)
{
    if (ok_missing)
    {
        this_ok_missing = ok_missing.get();
        if (this_ok_missing)
            this_options.set_flags(this_options.get_flags() & ~LoaderOptions::LF_report_errors);
        else
            this_options.set_flags(this_options.get_flags() | LoaderOptions::LF_report_errors);
    }
    else
    {
        this_ok_missing = (this_options.get_flags() & LoaderOptions::LF_report_errors) == 0;
    }

    if (no_cache)
    {
        if (no_cache.get())
            this_options.set_flags(this_options.get_flags() & ~LoaderOptions::LF_no_cache);
        else
            this_options.set_flags(this_options.get_flags() | LoaderOptions::LF_no_cache);
    }

    if (allow_instance)
        this_options.set_flags(this_options.get_flags() | LoaderOptions::LF_allow_instance);
}

// ************************************************************************************************

TypeHandle Loader::type_handle_;

Loader::Loader(ShowBase& base): impl_(std::make_unique<Impl>(base))
{
}

Loader::Loader(Loader&&) = default;

Loader::~Loader(void) = default;

Loader& Loader::operator=(Loader&&) = default;

NodePath Loader::load_model(const Filename& model_path, const LoaderOptions& loader_options,
    boost::optional<bool> no_cache, bool allow_instance, boost::optional<bool> ok_missing)
{
    rppanda_showbase_cat.debug() << "Loading model: " << model_path << std::endl;

    LoaderOptions this_options(loader_options);
    bool this_ok_missing;
    impl_->pre_load_model(this_options, this_ok_missing, no_cache, allow_instance, ok_missing);

    PT(PandaNode) node = impl_->loader_->load_sync(model_path, this_options);
    NodePath result;
    if (node)
        result = NodePath(node);

    if (!this_ok_missing && result.is_empty())
        std::runtime_error(std::string("Could not load model file(s): ") + model_path.c_str());

    return result;
}

std::vector<NodePath> Loader::load_model(const std::vector<Filename>& model_list, const LoaderOptions& loader_options,
    boost::optional<bool> no_cache, bool allow_instance, boost::optional<bool> ok_missing)
{
    rppanda_showbase_cat.debug() << "Loading model: " << join_to_string(model_list) << std::endl;

    LoaderOptions this_options(loader_options);
    bool this_ok_missing;
    impl_->pre_load_model(this_options, this_ok_missing, no_cache, allow_instance, ok_missing);

    std::vector<NodePath> result;
    for (const auto& model_path: model_list)
    {
        PT(PandaNode) node = impl_->loader_->load_sync(model_path, this_options);
        NodePath nodepath;
        if (node)
            nodepath = NodePath(node);

        if (!this_ok_missing && nodepath.is_empty())
            std::runtime_error(std::string("Could not load model file(s): ") + model_path.c_str());

        result.push_back(nodepath);
    }

    return result;
}

PT(AudioSound) Loader::load_sfx(const std::string& sound_path, bool positional)
{
    const auto& manager_list = impl_->base_.get_sfx_manager_list();
    if (!manager_list.empty())
        return load_sound(manager_list[0], sound_path, positional);
    else
        return nullptr;
}

std::vector<PT(AudioSound)> Loader::load_sfx(const std::vector<std::string>& sound_path, bool positional)
{
    const auto& manager_list = impl_->base_.get_sfx_manager_list();
    if (!manager_list.empty())
        return load_sound(manager_list[0], sound_path, positional);
    else
        return {};
}

PT(AudioSound) Loader::load_music(const std::string& sound_path, bool positional)
{
    if (auto music_manager = impl_->base_.get_music_manager())
        return load_sound(music_manager, sound_path, positional);
    else
        return nullptr;
}

std::vector<PT(AudioSound)> Loader::load_music(const std::vector<std::string>& sound_path, bool positional)
{
    if (auto music_manager = impl_->base_.get_music_manager())
        return load_sound(music_manager, sound_path, positional);
    else
        return {};
}

PT(AudioSound) Loader::load_sound(AudioManager* manager, const std::string& sound_path, bool positional)
{
    return manager->get_sound(sound_path, positional);
}

std::vector<PT(AudioSound)> Loader::load_sound(AudioManager* manager,
    const std::vector<std::string>& sound_path, bool positional)
{
    std::vector<PT(AudioSound)> result;
    result.reserve(sound_path.size());

    for (auto& path: sound_path)
        result.push_back(manager->get_sound(path, positional));

    return result;
}

}
