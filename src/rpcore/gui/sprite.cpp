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

#include "render_pipeline/rpcore/gui/sprite.hpp"

#include "render_pipeline/rpcore/loader.hpp"

namespace rpcore {

Sprite::Sprite(Texture* image, int w, int h, NodePath parent, float x, float y,
    bool transparent, bool near_filter, bool any_filter): RPObject("Sprite")
{
    init(image, w, h, parent, x, y, transparent, near_filter, any_filter);
}

Sprite::Sprite(const std::string& image_path, int w, int h, NodePath parent, float x, float y,
    bool transparent, bool near_filter, bool any_filter):
    Sprite(RPLoader::load_texture(image_path), w, h, parent, x, y, transparent, near_filter, any_filter)
{
}

Sprite::Sprite(const std::string& image_path, NodePath parent, float x, float y,
    bool transparent, bool near_filter, bool any_filter): RPObject("Sprite")
{
    Texture* tex = RPLoader::load_texture(image_path);
    init(tex, tex->get_x_size(), tex->get_y_size(), parent, x, y, transparent, near_filter, any_filter);
}

void Sprite::init(Texture* image, int w, int h, NodePath parent, float x, float y,
    bool transparent, bool near_filter, bool any_filter)
{
    _width = w;
    _height = h;

    _initial_pos = translate_pos(x, y);

    node_ = new rppanda::OnscreenImage(std::make_shared<rppanda::ImageInput>(image), parent);
    node_->set_pos(_initial_pos);
    node_->set_scale(_width / 2.0f, 1.0f, _height / 2.0f);

    if (transparent)
        node_->set_transparency(TransparencyAttrib::M_alpha);

    Texture* tex = node_->get_texture();

    // Apply a near filter, but only if the parent has no scale, otherwise
    // it will look weird
    if (near_filter && any_filter && parent.get_sx() == 1.0f)
    {
        tex->set_minfilter(SamplerState::FT_nearest);
        tex->set_magfilter(SamplerState::FT_nearest);
    }

    if (any_filter)
    {
        tex->set_anisotropic_degree(8);
        tex->set_wrap_u(SamplerState::WM_clamp);
        tex->set_wrap_v(SamplerState::WM_clamp);
    }
}

}
