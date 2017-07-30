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

#include "render_pipeline/rpcore/gui/checkbox.hpp"

#include "render_pipeline/rpcore/loader.hpp"
#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rpcore {

Checkbox::Checkbox(const Parameters& params): RPObject("Checkbox")
{
    const std::string prefix = params.radio ? "radiobox" : "checkbox";

    Texture* checked_img;
    Texture* unchecked_img;
    if (params.enabled)
    {
        checked_img = RPLoader::load_texture(std::string("/$$rp/data/gui/") + prefix + "_checked.png");
        unchecked_img = RPLoader::load_texture(std::string("/$$rp/data/gui/") + prefix + "_default.png");
    }
    else
    {
        checked_img = RPLoader::load_texture(std::string("/$$rp/data/gui/") + prefix + "_disabled.png");
        unchecked_img = checked_img;
    }

    // Set near filter, otherwise textures look like crap
    for (Texture* tex: {checked_img, unchecked_img})
    {
        tex->set_minfilter(SamplerState::FT_linear);
        tex->set_magfilter(SamplerState::FT_linear);
        tex->set_wrap_u(SamplerState::WM_clamp);
        tex->set_wrap_v(SamplerState::WM_clamp);
        tex->set_anisotropic_degree(0);
    }

    // TODO: implement
    auto node_options = std::make_shared<rppanda::DirectCheckBox::Options>();
    node_options->pos = LVecBase3f(params.x+11, 1, -params.y-8);
    node_options->scale = LVecBase3f(10 / 2.0f, 1.0f, 10 / 2.0f);
    node_options->checked_image = std::make_shared<rppanda::ImageInput>(checked_img);
    node_options->unchecked_image = std::make_shared<rppanda::ImageInput>(unchecked_img);
    node_options->image = { std::make_shared<rppanda::ImageInput>(unchecked_img) };
    //node_options->extra_args = ;
    node_options->state = rppanda::NORMAL;
    node_options->relief = rppanda::FLAT;
    node_options->command = std::bind(&Checkbox::update_status, this, std::placeholders::_1);

    _node = new rppanda::DirectCheckBox(params.parent, node_options);
    _node->set_frame_color(LColorf(0));
    _node->set_frame_size(LVecBase4f(-2.6f, 2.0f + params.expand_width / 7.5f, -2.35f, 2.5f));
    _node->set_transparency(TransparencyAttrib::M_alpha);

    _callback = params.callback;
    _extra_args = params.extra_args;

    if (params.checked)
        set_checked(true, false);
}

bool Checkbox::is_checked(void) const
{
    return _node->is_checked();
}

void Checkbox::update_status(void* args)
{
    const bool status = _node->is_checked();

    // TODO: implement
    //if (!status)

    if (_callback)
        _callback(status, _extra_args);
}

void Checkbox::set_checked(bool val, bool do_callback)
{
    _node->set_checked(val);

    if (val)
        _node->set_image(_node->get_checked_image());
    else
        _node->set_image(_node->get_unchecked_image());

    if (do_callback && _callback)
        _callback(val, _extra_args);
}

}
