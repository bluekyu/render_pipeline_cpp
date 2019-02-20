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

#include "render_pipeline/rpcore/image.hpp"

#include "render_pipeline/rpcore/render_target.hpp"

namespace rpcore {

const std::unordered_map<std::string, Image::ComponentFormatType> Image::FORMAT_MAPPING = {
    { "R11G11B10",  { Texture::T_float, Texture::F_r11_g11_b10 } },
    { "RGBA8",      { Texture::T_unsigned_byte, Texture::F_rgba8 } },
    { "RGBA16",     { Texture::T_float, Texture::F_rgba16 } },
    { "RGBA32",     { Texture::T_float, Texture::F_rgba32 } },
    { "R8",         { Texture::T_unsigned_byte, Texture::F_red } },
    { "R8UI",       { Texture::T_unsigned_byte, Texture::F_red } },
    { "R16",        { Texture::T_float, Texture::F_r16 } },
    { "R16UI",      { Texture::T_unsigned_short, Texture::F_r16i } },
    { "R32",        { Texture::T_float, Texture::F_r32 } },
    { "R32I",       { Texture::T_int, Texture::F_r32i } },
};

std::vector<Image*> Image::REGISTERED_IMAGES;

std::unique_ptr<Image> Image::create_buffer(const std::string& name, int size, const std::string& component_format)
{
    auto img = std::make_unique<Image>("ImgBuffer-" + name);
    img->setup_buffer(size, component_format);
    return img;
}

void Image::setup_buffer(int size, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_buffer_texture(size, comp_type_format.first, comp_type_format.second, GeomEnums::UH_static);
}

std::unique_ptr<Image> Image::create_counter(const std::string& name)
{
    return Image::create_buffer(name, 1, "R32I");
}

void Image::setup_counter()
{
    Image::setup_buffer(1, "R32I");
}

std::unique_ptr<Image> Image::create_2d(const std::string& name, int w, int h, const std::string& component_format)
{
    auto img = std::make_unique<Image>("Img2D-" + name);
    img->setup_2d(w, h, component_format);
    return img;
}

void Image::setup_2d(int w, int h, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_2d_texture(w, h, comp_type_format.first, comp_type_format.second);
}

std::unique_ptr<Image> Image::create_2d_array(const std::string& name, int w, int h, int slices, const std::string& component_format)
{
    auto img = std::make_unique<Image>(std::string("Img2DArr-") + name);
    img->setup_2d_array(w, h, slices, component_format);
    return img;
}

void Image::setup_2d_array(int w, int h, int slices, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_2d_texture_array(w, h, slices, comp_type_format.first, comp_type_format.second);
}

std::unique_ptr<Image> Image::create_3d(const std::string& name, int w, int h, int slices, const std::string& component_format)
{
    auto img = std::make_unique<Image>(std::string("Img3D-") + name);
    img->setup_3d(w, h, slices, component_format);
    return img;
}

void Image::setup_3d(int w, int h, int slices, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_3d_texture(w, h, slices, comp_type_format.first, comp_type_format.second);
}

std::unique_ptr<Image> Image::create_cube(const std::string& name, int size, const std::string& component_format)
{
    auto img = std::make_unique<Image>(std::string("ImgCube-") + name);
    img->setup_cube(size, component_format);
    return img;
}

void Image::setup_cube(int size, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_cube_map(size, comp_type_format.first, comp_type_format.second);
}

std::unique_ptr<Image> Image::create_cube_array(const std::string& name, int size, int num_cubemaps, const std::string& component_format)
{
    auto img = std::make_unique<Image>(std::string("ImgCubeArr-") + name);
    img->setup_cube_array(size, num_cubemaps, component_format);
    return img;
}

void Image::setup_cube_array(int size, int num_cubemaps, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_cube_map_array(size, num_cubemaps, comp_type_format.first, comp_type_format.second);
}

const Image::ComponentFormatType& Image::convert_texture_format(const std::string& comp_type)
{
    try
    {
        return FORMAT_MAPPING.at(comp_type);
    }
    catch (const std::out_of_range& err)
    {
        RPObject::global_error("Image", std::string("Unsupported texture component format: ") + comp_type);
        throw err;
    }
}

Image::Image(const std::string& name): texture_(Texture::make_texture())
{
    texture_->set_name(name);

    Image::REGISTERED_IMAGES.push_back(this);
    texture_->set_clear_color(0);
    texture_->clear_image();
    sort_ = RenderTarget::CURRENT_SORT;
}

Image::~Image()
{
    Image::REGISTERED_IMAGES.erase(std::find(Image::REGISTERED_IMAGES.begin(), Image::REGISTERED_IMAGES.end(), this));
}

}
