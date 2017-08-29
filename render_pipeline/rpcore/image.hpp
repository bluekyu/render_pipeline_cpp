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

#include <texture.h>

#include <unordered_map>
#include <utility>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpcore {

/**
 * This is a wrapper arround the Texture class from Panda3D, which keeps
 * track of all images and registers / unregisters them aswell as counting
 * the memory used. This is used by all classes instead of pandas builtin
 * Texture class.
 */
class RENDER_PIPELINE_DECL Image : public RPObject
{
public:
    using ComponentFormatType = std::pair<Texture::ComponentType, Texture::Format>;

    static const std::unordered_map<std::string, ComponentFormatType> FORMAT_MAPPING;

    // All registered images
    static std::vector<Image*> REGISTERED_IMAGES;

    /** Creates a new buffer texture. */
    static std::shared_ptr<Image> create_buffer(const std::string& name, int size, const std::string& component_format);

    /** Creates a new 1x1 R32I texture to be used as an atomic counter. */
    static std::shared_ptr<Image> create_counter(const std::string& name);

    /** Creates a new 2D texture. */
    static std::shared_ptr<Image> create_2d(const std::string& name, int w, int h, const std::string& component_format);

    /** Creates a new 2D-array texture. */
    static std::shared_ptr<Image> create_2d_array(const std::string& name, int w, int h, int slices, const std::string& component_format);

    /** Creates a new 3D texture. */
    static std::shared_ptr<Image> create_3d(const std::string& name, int w, int h, int slices, const std::string& component_format);

    /** Creates a new cubemap */
    static std::shared_ptr<Image> create_cube(const std::string& name, int size, const std::string& component_format);

    /** Creates a new cubemap. */
    static std::shared_ptr<Image> create_cube_array(const std::string& name, int size, int num_cubemaps, const std::string& component_format);

    static const ComponentFormatType& convert_texture_format(const std::string& comp_type);
    static std::string convert_texture_format(ComponentFormatType comp_format_type);

    Image(const std::string& name);
    ~Image();

    void setup_buffer(int size, const std::string& component_format);
    void setup_counter();
    void setup_2d(int w, int h, const std::string& component_format);
    void setup_2d_array(int w, int h, int slices, const std::string& component_format);
    void setup_3d(int w, int h, int slices, const std::string& component_format);
    void setup_cube(int size, const std::string& component_format);
    void setup_cube_array(int size, int num_cubemaps, const std::string& component_format);

    int get_sort() const;

    std::string get_texture_format() const;

    Texture* get_texture() const;

    void set_clear_color(const LColor& color);
    void clear_image();

    void set_x_size(int x_size);
    void set_y_size(int y_size);
    void set_z_size(int z_size);

    void set_minfilter(Texture::FilterType filter);
    void set_magfilter(Texture::FilterType filter);

    void set_wrap_u(Texture::WrapMode wrap);
    void set_wrap_v(Texture::WrapMode wrap);
    void set_wrap_w(Texture::WrapMode wrap);

private:
    int sort_;
    PT(Texture) texture_;
};

// ************************************************************************************************

inline int Image::get_sort() const
{
    return sort_;
}

inline Texture* Image::get_texture() const
{
    return texture_;
}

inline void Image::set_clear_color(const LColor& color)
{
    texture_->set_clear_color(color);
}

inline void Image::clear_image()
{
    texture_->clear_image();
}

inline void Image::set_x_size(int x_size)
{
    texture_->set_x_size(x_size);
}

inline void Image::set_y_size(int y_size)
{
    texture_->set_y_size(y_size);
}

inline void Image::set_z_size(int z_size)
{
    texture_->set_z_size(z_size);
}

inline void Image::set_minfilter(Texture::FilterType filter)
{
    texture_->set_minfilter(filter);
}

inline void Image::set_magfilter(Texture::FilterType filter)
{
    texture_->set_magfilter(filter);
}

inline void Image::set_wrap_u(Texture::WrapMode wrap)
{
    texture_->set_wrap_u(wrap);
}

inline void Image::set_wrap_v(Texture::WrapMode wrap)
{
    texture_->set_wrap_v(wrap);
}

inline void Image::set_wrap_w(Texture::WrapMode wrap)
{
    texture_->set_wrap_w(wrap);
}

}
