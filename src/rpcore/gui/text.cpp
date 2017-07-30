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

Text::Text(const Parameters& params): RPObject("Text")
{
    ::TextNode::Alignment align_mode = ::TextNode::Alignment::A_left;

    if (params.align == "center")
        align_mode = ::TextNode::Alignment::A_center;
    else if (params.align == "right")
        align_mode = ::TextNode::Alignment::A_right;

    TextFont* font = params.font;
    if (!font)
    {
        font = Globals::font;
        assert(font);
    }

    _initial_pos = LVecBase2f(params.x, -params.y);

    rppanda::OnscreenText::Parameters node_params;
    node_params.text = params.text;
    node_params.parent = params.parent;
    node_params.pos = _initial_pos;
    node_params.scale = params.size;
    node_params.align = align_mode;
    node_params.fg = LColorf(params.color, 1.0f);
    node_params.font = font;
    node_params.may_change = params.may_change;
    _node = rppanda::OnscreenText(node_params);
}

}
