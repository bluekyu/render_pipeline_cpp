/**
 * Render Pipeline C++
 *
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
    Loader(const Loader&) = delete;
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    Loader(Loader&&);
#endif

    ~Loader(void);

    Loader& operator=(const Loader&) = delete;
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    Loader& operator=(Loader&&);
#endif

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

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle Loader::get_class_type(void)
{
    return type_handle_;
}

inline void Loader::init_type(void)
{
    DirectObject::init_type();
    register_type(type_handle_, "rppanda::Loader", DirectObject::get_class_type());
}

inline TypeHandle Loader::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle Loader::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
