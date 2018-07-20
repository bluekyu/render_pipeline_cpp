/**
 * Copyright (c) 2008, Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Carnegie Mellon University nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * (This is the Modified BSD License.  See also
 * http://www.opensource.org/licenses/bsd-license.php )
 */

/**
 * This is C++ porting codes of direct/src/showbase/Loader.py
 */

#pragma once

#include <nodePath.h>
#include <audioSound.h>
#include <textFont.h>

#include <unordered_set>

#include <boost/optional.hpp>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

class AudioManager;
class Shader;
class ModelRoot;

namespace rppanda {

class ShowBase;

class RENDER_PIPELINE_DECL Loader : public DirectObject
{
public:
    using CallbackType = std::function<void(std::vector<NodePath>&)>;

    /**
     * Returned by loadModel when used asynchronously. This class is
     * modelled after Future, and can be awaited.
     */
    class RENDER_PIPELINE_DECL Callback
    {
    public:
        Callback(Loader* loader, int num_objects, const CallbackType& callback);

        void got_object(size_t index, NodePath object);

        /** Cancels the request. Callback won't be called. */
        void cancel();

        /** Returns true if the request was cancelled. */
        bool cancelled() const;

        /** Returns true if all the requests were finished or cancelled. */
        bool done() const;

        /** Suspending the thread to wait if necessary. */
        void wait() const;

    private:
        friend class Loader;

        Loader* loader_;
        std::vector<NodePath> objects_;
        CallbackType callback_;
        std::unordered_set<AsyncTask*> requests_;
        std::vector<AsyncTask*> request_list_;
    };

public:
    Loader(ShowBase& base);
    Loader(const Loader&) = delete;
    Loader(Loader&&);

    virtual ~Loader();

    Loader& operator=(const Loader&) = delete;
    Loader& operator=(Loader&&);

    NodePath load_model(const Filename& model_path, const LoaderOptions& loader_options={},
        boost::optional<bool> no_cache=boost::none, bool allow_instance=false, boost::optional<bool> ok_missing=boost::none);

    std::vector<NodePath> load_model(const std::vector<Filename>& model_list, const LoaderOptions& loader_options={},
        boost::optional<bool> no_cache=boost::none, bool allow_instance=false, boost::optional<bool> ok_missing=boost::none);

    std::shared_ptr<Callback> load_model_async(const Filename& model_path, const LoaderOptions& loader_options={},
        boost::optional<bool> no_cache=boost::none, bool allow_instance=false, boost::optional<bool> ok_missing=boost::none,
        const CallbackType& callback={}, boost::optional<int> priority=boost::none);

    std::shared_ptr<Callback> load_model_async(const std::vector<Filename>& model_list, const LoaderOptions& loader_options={},
        boost::optional<bool> no_cache=boost::none, bool allow_instance=false, boost::optional<bool> ok_missing=boost::none,
        const CallbackType& callback={}, boost::optional<int> priority = boost::none);

    void unload_model(NodePath model);
    void unload_model(ModelRoot* model);

    /**
     * This loads a special model as a TextFont object, for rendering
     * text with a TextNode.  A font file must be either a special
     * egg file (or bam file) generated with egg-mkfont, which is
     * considered a static font, or a standard font file (like a TTF
     * file) that is supported by FreeType, which is considered a
     * dynamic font.
     */
    PT(TextFont) load_font(const std::string& model_path,
        boost::optional<float> space_advance = boost::none,
        boost::optional<float> line_height = boost::none,
        boost::optional<float> point_size = boost::none,
        boost::optional<float> pixels_per_unit = boost::none, boost::optional<float> scale_factor = boost::none,
        boost::optional<int> texture_margin = boost::none, boost::optional<float> poly_margin = boost::none,
        boost::optional<SamplerState::FilterType> min_filter = boost::none,
        boost::optional<SamplerState::FilterType> mag_filter = boost::none,
        boost::optional<int> anisotropic_degree = boost::none,
        boost::optional<LColor> color = boost::none,
        boost::optional<float> outline_width = boost::none,
        float outline_feather = 0.1,
        LColor outline_color = LColor(0, 0, 0, 1),
        boost::optional<TextFont::RenderMode> render_mode = boost::none,
        bool ok_missing = false);

    /**
     * Attempt to load a texture from the given file path using
     * TexturePool class.
     */
    Texture* load_texture(const Filename& texture_path, boost::optional<Filename> alpha_path = boost::none,
        bool read_mipmaps = false, bool ok_missing = false,
        boost::optional<SamplerState::FilterType> min_filter = boost::none,
        boost::optional<SamplerState::FilterType> mag_filter = boost::none,
        boost::optional<int> anisotropic_degree = boost::none, const LoaderOptions& loader_options = {},
        boost::optional<bool> multiview = boost::none);

    /**
     * @p texture_pattern is a string that contains a sequence of one or
     * more hash characters ('#'), which will be filled in with the
     * z-height number.  Returns a 3-D Texture object, suitable for
     * rendering volumetric textures.
     */
    Texture* load_3d_texture(const Filename& texture_pattern,
        bool read_mipmaps = false, bool ok_missing = false,
        boost::optional<SamplerState::FilterType> min_filter = boost::none,
        boost::optional<SamplerState::FilterType> mag_filter = boost::none,
        boost::optional<int> anisotropic_degree = boost::none, const LoaderOptions& loader_options = {},
        boost::optional<bool> multiview = boost::none, int num_views = 2);

    /**
     * @p texture_pattern is a string that contains a sequence of one or
     * more hash characters ('#'), which will be filled in with the
     * z-height number.  Returns a 2-D Texture array object, suitable
     * for rendering array of textures.
     */
    Texture* load_2d_texture_array(const Filename& texture_pattern,
        bool read_mipmaps = false, bool ok_missing = false,
        boost::optional<SamplerState::FilterType> min_filter = boost::none,
        boost::optional<SamplerState::FilterType> mag_filter = boost::none,
        boost::optional<int> anisotropic_degree = boost::none, const LoaderOptions& loader_options = {},
        boost::optional<bool> multiview = boost::none, int num_views = 2);

    /**
     * @p texture_pattern is a string that contains a sequence of one or
     * more hash characters ('#'), which will be filled in with the
     * face index number (0 through 6).  Returns a six-face cube map
     * Texture object.
     */
    Texture* load_cube_map(const Filename& texture_pattern,
        bool read_mipmaps = false, bool ok_missing = false,
        boost::optional<SamplerState::FilterType> min_filter = boost::none,
        boost::optional<SamplerState::FilterType> mag_filter = boost::none,
        boost::optional<int> anisotropic_degree = boost::none, const LoaderOptions& loader_options = {},
        boost::optional<bool> multiview = boost::none);

    void unload_texture(Texture* texture);

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

    void unload_sfx(AudioSound* sfx);

    CPT(Shader) load_shader(const Filename& shader_path, bool ok_missing = false);

    void unload_shader(const Filename& shader_path);

    /**
     * Performs a model.flattenStrong() operation in a sub-thread
     * (if threading is compiled into Panda).  The model may be a
     * single NodePath, or it may be a list of NodePaths.
     *
     * Each model is duplicated and flattened in the sub-thread.
     *
     * If inPlace is True, then when the flatten operation completes,
     * the newly flattened copies are automatically dropped into the
     * scene graph, in place the original models.
     *
     * If a callback is specified, then it is called after the
     * operation is finished, receiving the flattened model (or a
     * list of flattened models).
     */
    std::shared_ptr<Callback> async_flatten_strong(NodePath model, const CallbackType& callback = {});
    std::shared_ptr<Callback> async_flatten_strong(const std::vector<NodePath>& model_list, const CallbackType& callback = {});
    std::shared_ptr<Callback> async_flatten_strong_in_place(std::vector<NodePath>& model_list, const CallbackType& callback = {});

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle Loader::get_class_type()
{
    return type_handle_;
}

inline void Loader::init_type()
{
    DirectObject::init_type();
    register_type(type_handle_, "rppanda::Loader", DirectObject::get_class_type());
}

inline TypeHandle Loader::get_type() const
{
    return get_class_type();
}

inline TypeHandle Loader::force_init_type()
{
    init_type();
    return get_class_type();
}

}
