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

#include "render_pipeline/rpcore/gui/checkbox.hpp"
#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rpcore {

LabeledCheckbox::LabeledCheckbox(NodePath parent, float x, float y, const std::function<void(bool, void*)>& chb_callback,
    void* chb_args, bool chb_checked, const std::string& text, float text_size,
    bool radio, const boost::optional<LVecBase3>& text_color, int expand_width, bool enabled): RPObject("LabeledCheckbox")
{
    if (!text_color)
        text_color_ = LVecBase3(1);
    else
        text_color_ = text_color.get();

    if (!enabled)
        text_color_ = LVecBase3(1.0f, 0.0f, 0.28f);

    checkbox_ = new Checkbox(parent, x, y, chb_callback, chb_args, radio, expand_width, chb_checked, enabled);

    Text::Parameters text_params;
    text_params.text = text;
    text_params.parent = parent;
    text_params.x = x + 26;
    text_params.y = y + 9;
    text_params.size = text_size;
    text_params.align = "left";
    text_params.color = text_color_;
    text_params.may_change = true;
    text_ = new Text(text_params);

    if (enabled)
    {
        checkbox_->get_node()->bind(rppanda::WITHIN, on_node_enter, this);
        checkbox_->get_node()->bind(rppanda::WITHOUT, on_node_leave, this);
    }
}

LabeledCheckbox::~LabeledCheckbox(void)
{
    delete text_;
    delete checkbox_;
}

void LabeledCheckbox::on_node_enter(const Event* ev, void* user_data)
{
    LabeledCheckbox* lc = reinterpret_cast<LabeledCheckbox*>(user_data);
    lc->text_->get_node().set_fg(LColorf(lc->text_color_[0] + 0.1f, lc->text_color_[1] + 0.1f,
        lc->text_color_[2] + 0.1f, 1.0f));
}

void LabeledCheckbox::on_node_leave(const Event* ev, void* user_data)
{
    LabeledCheckbox* lc = reinterpret_cast<LabeledCheckbox*>(user_data);
    lc->text_->get_node().set_fg(LColorf(lc->text_color_[0], lc->text_color_[1], lc->text_color_[2], 1.0f));
}

}
