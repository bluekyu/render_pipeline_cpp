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

#include "render_pipeline/rppanda/showbase/loader.hpp"

#include <audioManager.h>
#include <loader.h>
#include <audioLoadRequest.h>
#include <fontPool.h>
#include <staticTextFont.h>
#include <dynamicTextFont.h>
#include <texturePool.h>
#include <shaderPool.h>

#include <spdlog/fmt/ostr.h>

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

#if !defined(_MSC_VER) || _MSC_VER >= 1900
Loader::Loader(Loader&&) = default;
#endif

Loader::~Loader() = default;

#if !defined(_MSC_VER) || _MSC_VER >= 1900
Loader& Loader::operator=(Loader&&) = default;
#endif

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
        throw std::runtime_error(std::string("Could not load model file(s): ") + model_path.c_str());

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
            throw std::runtime_error(fmt::format("Could not load model file(s): {}", model_path));

        result.push_back(nodepath);
    }

    return result;
}

PT(TextFont) Loader::load_font(const std::string& model_path,
    boost::optional<float> space_advance,  boost::optional<float> line_height,
    boost::optional<float> point_size,
    boost::optional<float> pixels_per_unit, boost::optional<float> scale_factor,
    boost::optional<int> texture_margin, boost::optional<float> poly_margin,
    boost::optional<SamplerState::FilterType> min_filter,
    boost::optional<SamplerState::FilterType> mag_filter,
    boost::optional<int> anisotropic_degree,
    boost::optional<LColor> color,
    boost::optional<float> outline_width,
    float outline_feather,
    LColor outline_color,
    boost::optional<TextFont::RenderMode> render_mode,
    bool ok_missing)
{
    PT(TextFont) font = FontPool::load_font(model_path);
    if (!font)
    {
        if (!ok_missing)
            throw std::runtime_error(fmt::format("Could not load font file: {}", model_path));

        // If we couldn't load the model, at least return an
        // empty font.
        font = new StaticTextFont(new PandaNode("empty"));
    }

    // The following properties may only be set for dynamic fonts.
    if (font->is_of_type(DynamicTextFont::get_class_type()))
    {
        DynamicTextFont* dfont = DCAST(DynamicTextFont, font);

        if (point_size)
            dfont->set_point_size(point_size.value());

        if (pixels_per_unit)
            dfont->set_pixels_per_unit(pixels_per_unit.value());

        if (scale_factor)
            dfont->set_scale_factor(scale_factor.value());
        
        if (texture_margin)
            dfont->set_texture_margin(texture_margin.value());

        if (poly_margin)
            dfont->set_poly_margin(poly_margin.value());

        if (min_filter)
            dfont->set_minfilter(min_filter.value());

        if (mag_filter)
            dfont->set_magfilter(mag_filter.value());

        if (anisotropic_degree)
            dfont->set_anisotropic_degree(anisotropic_degree.value());

        if (color)
        {
            dfont->set_fg(color.value());

            // This means we want the background to match the
            // foreground color, but transparent.
            dfont->set_bg(LColor(color.value().get_xyz(), 0));
        }

        if (outline_width)
        {
            dfont->set_outline(outline_color, outline_width.value(), outline_feather);

            // This means we want the background to match the
            // outline color, but transparent.
            dfont->set_bg(LColor(outline_color.get_xyz(), 0));
        }

        if (render_mode)
            dfont->set_render_mode(render_mode.value());
    }

    if (line_height)
    {
        // If the line height is specified, it overrides whatever
        // the font itself thinks the line height should be.This
        // and spaceAdvance should be set last, since some of the
        // other parameters can cause these to be reset to their
        // default.
        font->set_line_height(line_height.value());
    }

    if (space_advance)
        font->set_space_advance(space_advance.value());

    return font;
}

Texture* Loader::load_texture(const Filename& texture_path, boost::optional<Filename> alpha_path,
    bool read_mipmaps, bool ok_missing,
    boost::optional<SamplerState::FilterType> min_filter,
    boost::optional<SamplerState::FilterType> mag_filter,
    boost::optional<int> anisotropic_degree, const LoaderOptions& loader_options,
    boost::optional<bool> multiview)
{
    LoaderOptions this_options(loader_options);

    if (multiview)
    {
        auto flags = this_options.get_texture_flags();
        if (multiview.value())
            flags |= LoaderOptions::TF_multiview;
        else
            flags &= ~LoaderOptions::TF_multiview;
        this_options.set_texture_flags(flags);
    }

    Texture* texture;
    if (alpha_path)
    {
        rppanda_showbase_cat.debug() << "Loading texture: " << texture_path.c_str() << " " << alpha_path.value().c_str() << std::endl;
        texture = TexturePool::load_texture(texture_path, alpha_path.value(), 0, 0, read_mipmaps, this_options);
    }
    else
    {
        rppanda_showbase_cat.debug() << "Loading texture: " << texture_path.c_str() << std::endl;
        texture = TexturePool::load_texture(texture_path, 0, read_mipmaps, this_options);
    }

    if (!texture && !ok_missing)
    {
        throw std::runtime_error(fmt::format("Could not load texture: {}", texture_path.c_str()));
    }

    if (min_filter)
        texture->set_minfilter(min_filter.value());
    if (mag_filter)
        texture->set_magfilter(mag_filter.value());
    if (anisotropic_degree)
        texture->set_anisotropic_degree(anisotropic_degree.value());

    return texture;
}

Texture* Loader::load_3d_texture(const Filename& texture_pattern,
    bool read_mipmaps, bool ok_missing,
    boost::optional<SamplerState::FilterType> min_filter,
    boost::optional<SamplerState::FilterType> mag_filter,
    boost::optional<int> anisotropic_degree, const LoaderOptions& loader_options,
    boost::optional<bool> multiview, int num_views)
{
    rppanda_showbase_cat.debug() << "Loading 3-D texture: " << texture_pattern.c_str() << std::endl;

    LoaderOptions this_options(loader_options);

    if (multiview)
    {
        auto flags = this_options.get_texture_flags();
        if (multiview.value())
            flags |= LoaderOptions::TF_multiview;
        else
            flags &= ~LoaderOptions::TF_multiview;
        this_options.set_texture_flags(flags);
        this_options.set_texture_num_views(num_views);
    }

    Texture* texture = TexturePool::load_3d_texture(texture_pattern, read_mipmaps, this_options);
    if (!texture && !ok_missing)
    {
        throw std::runtime_error(fmt::format("Could not load 3-D texture: {}", texture_pattern.c_str()));
    }

    if (min_filter)
        texture->set_minfilter(min_filter.value());
    if (mag_filter)
        texture->set_magfilter(mag_filter.value());
    if (anisotropic_degree)
        texture->set_anisotropic_degree(anisotropic_degree.value());

    return texture;
}

Texture* Loader::load_2d_texture_array(const Filename& texture_pattern,
    bool read_mipmaps, bool ok_missing,
    boost::optional<SamplerState::FilterType> min_filter,
    boost::optional<SamplerState::FilterType> mag_filter,
    boost::optional<int> anisotropic_degree, const LoaderOptions& loader_options,
    boost::optional<bool> multiview, int num_views)
{
    rppanda_showbase_cat.debug() << "Loading 3-D texture array: " << texture_pattern.c_str() << std::endl;

    LoaderOptions this_options(loader_options);

    if (multiview)
    {
        auto flags = this_options.get_texture_flags();
        if (multiview.value())
            flags |= LoaderOptions::TF_multiview;
        else
            flags &= ~LoaderOptions::TF_multiview;
        this_options.set_texture_flags(flags);
        this_options.set_texture_num_views(num_views);
    }

    Texture* texture = TexturePool::load_2d_texture_array(texture_pattern, read_mipmaps, this_options);
    if (!texture && !ok_missing)
    {
        throw std::runtime_error(fmt::format("Could not load 2-D texture array: {}", texture_pattern.c_str()));
    }

    if (min_filter)
        texture->set_minfilter(min_filter.value());
    if (mag_filter)
        texture->set_magfilter(mag_filter.value());
    if (anisotropic_degree)
        texture->set_anisotropic_degree(anisotropic_degree.value());

    return texture;
}

Texture* Loader::load_cube_map(const Filename& texture_pattern,
    bool read_mipmaps, bool ok_missing,
    boost::optional<SamplerState::FilterType> min_filter,
    boost::optional<SamplerState::FilterType> mag_filter,
    boost::optional<int> anisotropic_degree, const LoaderOptions& loader_options,
    boost::optional<bool> multiview)
{
    rppanda_showbase_cat.debug() << "Loading cube map: " << texture_pattern.c_str() << std::endl;

    LoaderOptions this_options(loader_options);

    if (multiview)
    {
        auto flags = this_options.get_texture_flags();
        if (multiview.value())
            flags |= LoaderOptions::TF_multiview;
        else
            flags &= ~LoaderOptions::TF_multiview;
        this_options.set_texture_flags(flags);
    }

    Texture* texture = TexturePool::load_cube_map(texture_pattern, read_mipmaps, this_options);
    if (!texture && !ok_missing)
    {
        throw std::runtime_error(fmt::format("Could not load cube map: {}", texture_pattern.c_str()));
    }

    if (min_filter)
        texture->set_minfilter(min_filter.value());
    if (mag_filter)
        texture->set_magfilter(mag_filter.value());
    if (anisotropic_degree)
        texture->set_anisotropic_degree(anisotropic_degree.value());

    return texture;
}

void Loader::unload_texture(Texture* texture)
{
    rppanda_showbase_cat.debug() << "Unloading texture: " << *texture << std::endl;
    TexturePool::release_texture(texture);
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

    for (auto&& path: sound_path)
        result.push_back(manager->get_sound(path, positional));

    return result;
}

void Loader::unload_sfx(AudioSound* sfx)
{
    if (sfx)
    {
        const auto& manager_list = impl_->base_.get_sfx_manager_list();
        if (!manager_list.empty())
            manager_list[0]->uncache_sound(sfx->get_name());
    }
}

CPT(Shader) Loader::load_shader(const Filename& shader_path, bool ok_missing)
{
    CPT(Shader) shader = ShaderPool::load_shader(shader_path);
    if (!shader && ok_missing)
        throw std::runtime_error(fmt::format("Could not load shader file: {}", shader_path.c_str()));
    return shader;
}

void Loader::unload_shader(const Filename& shader_path)
{
    ShaderPool::release_shader(shader_path);
}

}
