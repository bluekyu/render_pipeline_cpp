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

#include "render_pipeline/rpcore/gui/text.hpp"

#include <map>

#include "textNode.h"
#include "textFont.h"

#include "render_pipeline/rpcore/globals.hpp"

namespace rpcore {

const float Text::Default::size = 10;
const std::string Text::Default::align("left");
const LVecBase3 Text::Default::color(1);

Text::Text(const std::string& text, NodePath parent, float x, float y, float size,
    const std::string& align, const LVecBase3& color, bool may_change, TextFont* font): RPObject("Text")
{
    ::TextNode::Alignment align_mode = ::TextNode::Alignment::A_left;

    if (align == "center")
        align_mode = ::TextNode::Alignment::A_center;
    else if (align == "right")
        align_mode = ::TextNode::Alignment::A_right;

    if (!font)
    {
        font = Globals::font;
        assert(font);
    }

    initial_pos_ = LVecBase2f(x, -y);

    rppanda::OnscreenText::Parameters node_params;
    node_params.text = text;
    node_params.parent = parent;
    node_params.pos = initial_pos_;
    node_params.scale = size;
    node_params.align = align_mode;
    node_params.fg = LColorf(color, 1.0f);
    node_params.font = font;
    node_params.may_change = may_change;
    node_ = rppanda::OnscreenText(node_params);
}

}
