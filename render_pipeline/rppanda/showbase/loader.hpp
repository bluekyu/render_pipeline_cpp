#pragma once

#include <nodePath.h>
#include <audioSound.h>

#include <boost/optional.hpp>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

class AudioManager;

namespace rppanda {

class ShowBase;

class RENDER_PIPELINE_DECL Loader: public DirectObject
{
public:
    Loader(ShowBase& base);
    ~Loader(void);

    NodePath load_model(const Filename& model_path, const LoaderOptions& loader_options={},
        boost::optional<bool> no_cache={}, bool allow_instance=false, boost::optional<bool> ok_missing={});

    std::vector<NodePath> load_model(const std::vector<Filename>& model_list, const LoaderOptions& loader_options={},
        boost::optional<bool> no_cache={}, bool allow_instance=false, boost::optional<bool> ok_missing={});

    PT(AudioSound) load_sfx(const std::string& sound_path, bool positional=false);

    std::vector<PT(AudioSound)> load_sfx(const std::vector<std::string>& sound_path, bool positional=false);

    PT(AudioSound) load_music(const std::string& sound_path, bool positional=false);

    std::vector<PT(AudioSound)> load_music(const std::vector<std::string>& sound_path, bool positional=false);

    /**
     * Loads one sound files, specifying the particular
     * AudioManager that should be used to load them. @p sound_path is a single filename.
     */
    PT(AudioSound) load_sound(AudioManager* manager, const std::string& sound_path, bool positional=false);

    /**
     * Loads one more sound files, specifying the particular
     * AudioManager that should be used to load them. @p sound_path is a vector of filenames.
     */
    std::vector<PT(AudioSound)> load_sound(AudioManager* manager,
        const std::vector<std::string>& sound_path, bool positional=false);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
