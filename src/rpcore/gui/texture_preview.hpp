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

#include <render_pipeline/rpcore/gui/draggable_window.hpp>

namespace rpcore {

class Sprite;
class LabeledCheckbox;
class Slider;

class TexturePreview : public DraggableWindow
{
public:
    TexturePreview(NodePath parent);

    /** "Presents" a given texture and shows the window. */
    void present(Texture* tex);

private:
    void create_components() override;

    void set_slice();

    void set_mip();

    void set_brightness();

    void set_enable_tonemap(bool arg);

    NodePath _content_node;
    Texture* _current_tex;

    std::shared_ptr<Sprite> _preview_image;
    std::shared_ptr<LabeledCheckbox> _tonemap_box;

    std::shared_ptr<Text> _mip_text;
    std::shared_ptr<Text> _slice_text;
    std::shared_ptr<Text> _bright_text;

    std::shared_ptr<Slider> _mip_slider;
    std::shared_ptr<Slider> _slice_slider;
    std::shared_ptr<Slider> _bright_slider;
};

}
