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

#include "rpcore/gui/texture_preview.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rpcore/gui/labeled_checkbox.hpp"
#include "render_pipeline/rpcore/gui/slider.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rppanda/gui/direct_slider.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"

#include "rpcore/util/display_shader_builder.hpp"

namespace rpcore {

TexturePreview::TexturePreview(NodePath parent):
    DraggableWindow(1600, 900, "Texture Viewer", parent)
{
    create_components();
}

TexturePreview::~TexturePreview() = default;

void TexturePreview::present(Texture* tex)
{
    _current_tex = tex;
    set_title(tex->get_name());

    // Remove old content
    _content_node.node()->remove_all_children();

    const int w = tex->get_x_size();
    const int h = tex->get_y_size();
    int display_w = width_ - 40.0f;
    int display_h = height_ - 110.0f;
    if (h > 1)
    {
        float scale_x = display_w / static_cast<float>(w);
        float scale_y = display_h / static_cast<float>(h);
        float scale_f = (std::min)(scale_x, scale_y);
        display_w = scale_f * w;
        display_h = scale_f * h;
    }

    auto image = std::make_unique<Sprite>(tex, display_w, display_h, _content_node, 20, 90, false, true, false);

    std::string description;

    // Image size
    description += (boost::format("%d x %d x %d") % tex->get_x_size() % tex->get_y_size() % tex->get_z_size()).str();

    // Image type
    description += (boost::format(", %s, %s") %
        boost::to_upper_copy(Texture::format_format(tex->get_format())) %
        boost::to_upper_copy(Texture::format_component_type(tex->get_component_type()))
        ).str();

    Text desc_text(description, _content_node, 17.0f, 70.0f, 16, "left", LVecBase3(0.6f, 0.6f, 0.6f));

    size_t estimated_bytes = tex->estimate_texture_memory();
    const std::string& size_desc = (boost::format("Estimated memory: %2.2f MB") %
        (estimated_bytes / (1024.0f * 1024.0f))).str();

    Text size_desc_text(size_desc, _content_node, width_-20.0f, 70.0f, 18, "right",
        LVecBase3(0.34f, 0.564f, 0.192f));

    int x_pos = static_cast<int>(size_desc.length()) * 9 + 140;

    // Slider for viewing different mipmaps
    if (tex->uses_mipmaps())
    {
        _mip_slider = std::make_unique<Slider>(x_pos, 65, _content_node, 140, 0,
            tex->get_expected_num_mipmap_levels()-1, 0, Slider::Default::page_size,
            std::bind(&TexturePreview::set_mip, this));

        x_pos += 140 + 5;

        _mip_text = std::make_unique<Text>("MIP: 5", _content_node, x_pos, 72, 18,
            Text::Default::align,
            LVecBase3(1.0f, 0.4f, 0.4f), true);

        x_pos += 50 + 30;
    }

    // Slider for viewing different Z-layers
    if (tex->get_z_size() > 1)
    {
        _slice_slider = std::make_unique<Slider>(x_pos, 65, _content_node, 250, 0,
            tex->get_z_size() - 1, 0, Slider::Default::page_size,
            std::bind(&TexturePreview::set_slice, this));

        x_pos += 250 + 5;

        _slice_text = std::make_unique<Text>("Z: 5", _content_node, x_pos, 72, 18,
            Text::Default::align,
            LVecBase3(0.4f, 1.0f, 0.4f), true);

        x_pos += 50 + 30;
    }

    // Slider to adjust brightness
    _bright_slider = std::make_unique<Slider>(x_pos, 65, _content_node, 140, -14, 14,
        0, Slider::Default::page_size,
        std::bind(&TexturePreview::set_brightness, this));

    x_pos += 140 + 5;
    _bright_text = std::make_unique<Text>("Bright: 1", _content_node, x_pos, 72, 18,
        Text::Default::align, LVecBase3(0.4f, 0.4f, 1.0f), true);
    x_pos += 100 + 30;

    // Slider to enable reinhard tonemapping
    _tonemap_box = std::make_unique<LabeledCheckbox>(_content_node, x_pos, 60,
        std::bind(&TexturePreview::set_enable_tonemap, this, std::placeholders::_1),
        false, "Tonemap", 18, false, LVecBase3(1.0f, 0.4f, 0.4f), 90);
    x_pos += 90 + 30;

    auto image_np = image->get_node();
    image_np->set_shader_input("slice", LVecBase4i(0));
    image_np->set_shader_input("mipmap", LVecBase4i(0));
    image_np->set_shader_input("brightness", LVecBase4i(1));
    image_np->set_shader_input("tonemap", LVecBase4i(0));

    PT(Shader) preview_shader = DisplayShaderBuilder::build(tex, display_w, display_h);
    image->set_shader(preview_shader);

    _preview_image = std::move(image);

    show();
}

void TexturePreview::set_slice()
{
    int idx = static_cast<int>(_slice_slider->get_value());
    _preview_image->set_shader_input(ShaderInput("slice", LVecBase4i(idx, 0, 0, 0)));
    _slice_text->set_text(std::string("Z: ") + std::to_string(idx));
}

void TexturePreview::set_mip()
{
    int idx = static_cast<int>(_mip_slider->get_value());
    _preview_image->set_shader_input(ShaderInput("mipmap", LVecBase4i(idx, 0, 0, 0)));
    _mip_text->set_text(std::string("MIP: ") + std::to_string(idx));
}

void TexturePreview::set_brightness()
{
    float val = _bright_slider->get_value();
    float scale = std::pow(2.0f, val);
    _bright_text->set_text((boost::format("Bright: %.3f") % scale).str());
    _preview_image->set_shader_input(ShaderInput("brightness", LVecBase4(scale, 0, 0, 0)));
}

void TexturePreview::set_enable_tonemap(bool arg)
{
    _preview_image->set_shader_input(ShaderInput("tonemap", LVecBase4i(arg, 0, 0, 0)));
}

void TexturePreview::create_components()
{
    DraggableWindow::create_components();
    _content_node = node_.attach_new_node("content");
}

}
