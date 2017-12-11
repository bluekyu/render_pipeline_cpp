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

const float LabeledCheckbox::Default::text_size = 16;

LabeledCheckbox::LabeledCheckbox(NodePath parent, float x, float y, const std::function<void(bool)>& chb_callback,
    bool chb_checked, const std::string& text, float text_size,
    bool radio, const boost::optional<LVecBase3>& text_color, int expand_width, bool enabled): RPObject("LabeledCheckbox")
{
    if (!text_color)
        text_color_ = LVecBase3(1);
    else
        text_color_ = text_color.get();

    if (!enabled)
        text_color_ = LVecBase3(1.0f, 0.0f, 0.28f);

    checkbox_ = std::make_unique<Checkbox>(parent, x, y, chb_callback, radio, expand_width, chb_checked, enabled);

    text_ = std::make_unique<Text>(text, parent, x+26, y+9, text_size, Text::Default::align, text_color_, true);

    if (enabled)
    {
        checkbox_->get_node()->bind(rppanda::DGG_WITHIN, [this](const Event*) { on_node_enter(); });
        checkbox_->get_node()->bind(rppanda::DGG_WITHOUT, [this](const Event*) { on_node_leave(); });
    }
}

LabeledCheckbox::~LabeledCheckbox() = default;

void LabeledCheckbox::on_node_enter()
{
    text_->get_node().set_fg(LColorf(text_color_[0] + 0.1f, text_color_[1] + 0.1f,
        text_color_[2] + 0.1f, 1.0f));
}

void LabeledCheckbox::on_node_leave()
{
    text_->get_node().set_fg(LColorf(text_color_[0], text_color_[1], text_color_[2], 1.0f));
}

}
