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

#include "render_pipeline/rpcore/gui/labeled_checkbox.hpp"

#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rpcore {

LabeledCheckbox::LabeledCheckbox(const Parameters& params): RPObject("LabeledCheckbox")
{
    if (params.enabled)
        _text_color = params.text_color;
    else
        _text_color = LVecBase3f(1.0f, 0.0f, 0.28f);

    _checkbox = new Checkbox(params);

    Text::Parameters text_params;
    text_params.text = params.text;
    text_params.parent = params.parent;
    text_params.x = params.x + 26;
    text_params.y = params.y + 9;
    text_params.size = params.text_size;
    text_params.align = "left";
    text_params.color = _text_color;
    text_params.may_change = true;
    _text = new Text(text_params);

    if (params.enabled)
    {
        _checkbox->get_node()->bind(rppanda::WITHIN, on_node_enter, this);
        _checkbox->get_node()->bind(rppanda::WITHOUT, on_node_leave, this);
    }
}

LabeledCheckbox::~LabeledCheckbox(void)
{
    delete _text;
    delete _checkbox;
}

void LabeledCheckbox::on_node_enter(const Event* ev, void* user_data)
{
    LabeledCheckbox* lc = reinterpret_cast<LabeledCheckbox*>(user_data);
    lc->_text->get_node().set_fg(LColorf(lc->_text_color[0] + 0.1f, lc->_text_color[1] + 0.1f,
        lc->_text_color[2] + 0.1f, 1.0f));
}

void LabeledCheckbox::on_node_leave(const Event* ev, void* user_data)
{
    LabeledCheckbox* lc = reinterpret_cast<LabeledCheckbox*>(user_data);
    lc->_text->get_node().set_fg(LColorf(lc->_text_color[0], lc->_text_color[1], lc->_text_color[2], 1.0f));
}

}
