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

#include "rpcore/gui/texture_preview.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rpcore/gui/labeled_checkbox.hpp"
#include "render_pipeline/rpcore/gui/slider.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rppanda/gui/direct_slider.hpp"

#include "rpcore/util/display_shader_builder.hpp"

namespace rpcore {

TexturePreview::TexturePreview(RenderPipeline* pipeline, NodePath parent):
    DraggableWindow(1600, 900, "Texture Viewer", parent), _pipeline(pipeline)
{
    create_components();
}

void TexturePreview::present(Texture* tex)
{
    _current_tex = tex;
    set_title(tex->get_name());

    // Remove old content
    _content_node.node()->remove_all_children();

    const int w = tex->get_x_size();
    const int h = tex->get_y_size();
    int display_w = _width - 40.0f;
    int display_h = _height - 110.0f;
    if (h > 1)
    {
        float scale_x = display_w / float(w);
        float scale_y = display_h / float(h);
        float scale_f = (std::min)(scale_x, scale_y);
        display_w = scale_f * w;
        display_h = scale_f * h;
    }

    std::shared_ptr<Sprite> image = std::make_shared<Sprite>(tex, display_w, display_h, _content_node, 20, 90, false, true, false);

    std::string description;

    // Image size
    description += (boost::format("%d x %d x %d") % tex->get_x_size() % tex->get_y_size() % tex->get_z_size()).str();

    // Image type
    description += (boost::format(", %s, %s") %
        boost::to_upper_copy(Texture::format_format(tex->get_format())) %
        boost::to_upper_copy(Texture::format_component_type(tex->get_component_type()))
        ).str();

    {
        Text::Parameters params;
        params.text = description;
        params.parent = _content_node;
        params.x = 17.0f;
        params.y = 70.0f;
        params.size = 16;
        params.align = "left";
        params.color = LVecBase3(0.6f, 0.6f, 0.6f);
        Text desc_text(params);
    }

    size_t estimated_bytes = tex->estimate_texture_memory();
    const std::string& size_desc = (boost::format("Estimated memory: %2.2f MB") %
        (estimated_bytes / (1024.0f * 1024.0f))).str();

    {
        Text::Parameters params;
        params.text = size_desc;
        params.parent = _content_node;
        params.x = _width - 20.0f;
        params.y = 70.0f;
        params.size = 18;
        params.align = "right";
        params.color = LVecBase3(0.34f, 0.564f, 0.192f);
        Text size_desc_text(params);
    }

    int x_pos = static_cast<int>(size_desc.length()) * 9 + 140;

    // Slider for viewing different mipmaps
    if (tex->uses_mipmaps())
    {
        Slider::Parameters _mip_slider_params;
        _mip_slider_params.parent = _content_node;
        _mip_slider_params.size = 140;
        _mip_slider_params.min_value = 0;
        _mip_slider_params.max_value = tex->get_expected_num_mipmap_levels() - 1;
        _mip_slider_params.callback = std::bind(&TexturePreview::set_mip, this, std::placeholders::_1);
        _mip_slider_params.x = x_pos;
        _mip_slider_params.y = 65;
        _mip_slider_params.value = 0;
        _mip_slider = std::make_shared<Slider>(_mip_slider_params);

        x_pos += 140 + 5;

        Text::Parameters params;
        params.text = "MIP: 5";
        params.parent = _content_node;
        params.x = x_pos;
        params.y = 72;
        params.size = 18;
        params.color = LVecBase3(1.0f, 0.4f, 0.4f);
        params.may_change = true;
        _mip_text = std::make_shared<Text>(params);

        x_pos += 50 + 30;
    }

    // Slider for viewing different Z-layers
    if (tex->get_z_size() > 1)
    {
        Slider::Parameters _slice_slider_params;
        _slice_slider_params.parent = _content_node;
        _slice_slider_params.size = 250;
        _slice_slider_params.min_value = 0;
        _slice_slider_params.max_value = tex->get_z_size() - 1;
        _slice_slider_params.callback = std::bind(&TexturePreview::set_slice, this, std::placeholders::_1);
        _slice_slider_params.x = x_pos;
        _slice_slider_params.y = 65;
        _slice_slider_params.value = 0;
        _slice_slider = std::make_shared<Slider>(_slice_slider_params);

        x_pos += 250 + 5;

        Text::Parameters params;
        params.text = "Z: 5";
        params.parent = _content_node;
        params.x = x_pos;
        params.y = 72;
        params.size = 18;
        params.color = LVecBase3(0.4f, 1.0f, 0.4f);
        params.may_change = true;
        _slice_text = std::make_shared<Text>(params);

        x_pos += 50 + 30;
    }

    {
        // Slider to adjust brightness
        Slider::Parameters _bright_slider_params;
        _bright_slider_params.parent = _content_node;
        _bright_slider_params.size = 140;
        _bright_slider_params.min_value = -14;
        _bright_slider_params.max_value = 14;
        _bright_slider_params.callback = std::bind(&TexturePreview::set_brightness, this, std::placeholders::_1);
        _bright_slider_params.x = x_pos;
        _bright_slider_params.y = 65;
        _bright_slider_params.value = 0;
        _bright_slider = std::make_shared<Slider>(_bright_slider_params);
    }

    x_pos += 140 + 5;
    {
        Text::Parameters params;
        params.text = "Bright: 1";
        params.parent = _content_node;
        params.x = x_pos;
        params.y = 72;
        params.size = 18;
        params.align = "left";
        params.color = LVecBase3(0.4f, 0.4f, 1.0f);
        params.may_change = true;
        _bright_text = std::make_shared<Text>(params);
    }
    x_pos += 100 + 30;

    // Slider to enable reinhard tonemapping
    {
        LabeledCheckbox::Parameters params;
        params.parent = _content_node;
        params.x = x_pos;
        params.y = 60;
        params.text = "Tonemap";
        params.text_color = LVecBase3(1.0f, 0.4f, 0.4f);
        params.checked = false;
        params.callback = std::bind(&TexturePreview::set_enable_tonemap, this, std::placeholders::_1, std::placeholders::_2);
        params.text_size = 18;
        params.expand_width = 90;
        _tonemap_box = std::make_shared<LabeledCheckbox>(params);
    }
    x_pos += 90 + 30;

    auto image_np = image->get_node();
    image_np->set_shader_input("slice", 0);
    image_np->set_shader_input("mipmap", 0);
    image_np->set_shader_input("brightness", 1);
    image_np->set_shader_input("tonemap", false);

    PT(Shader) preview_shader = DisplayShaderBuilder::build(tex, display_w, display_h);
    image->set_shader(preview_shader);

    _preview_image = image;

    show();
}

void TexturePreview::set_slice(void* extra_args)
{
    int idx = int(_slice_slider->get_value());
    _preview_image->set_shader_input(ShaderInput("slice", LVecBase4i(idx, 0, 0, 0)));
    _slice_text->set_text(std::string("Z: ") + std::to_string(idx));
}

void TexturePreview::set_mip(void* extra_args)
{
    int idx = int(_mip_slider->get_value());
    _preview_image->set_shader_input(ShaderInput("mipmap", LVecBase4i(idx, 0, 0, 0)));
    _mip_text->set_text(std::string("MIP: ") + std::to_string(idx));
}

void TexturePreview::set_brightness(void* extra_args)
{
    float val = _bright_slider->get_value();
    float scale = std::pow(2.0f, val);
    _bright_text->set_text((boost::format("Bright: %.3f") % scale).str());
    _preview_image->set_shader_input(ShaderInput("brightness", LVecBase4(scale, 0, 0, 0)));
}

void TexturePreview::set_enable_tonemap(bool arg, void* extra_args)
{
    _preview_image->set_shader_input(ShaderInput("tonemap", LVecBase4i(arg, 0, 0, 0)));
}

void TexturePreview::create_components(void)
{
    DraggableWindow::create_components();
    _content_node = _node.attach_new_node("content");
}

}
