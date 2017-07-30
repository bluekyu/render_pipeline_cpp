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

#include "render_pipeline/rpcore/gui/slider.hpp"

#include "render_pipeline/rppanda/gui/direct_slider.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rpcore {

Slider::Slider(const Parameters& params): RPObject("Slider")
{
    // TODO: implement
    // Scale has to be 2.0, otherwise there will be an error.
    auto options = std::make_shared<rppanda::DirectSlider::Options>();
    options->pos = LVecBase3(params.size * 0.5 + params.x, 1, -params.y);
    options->range = LVecBase2(params.min_value, params.max_value);
    options->value = params.value;
    options->page_size = params.page_size;
    options->scale = 2.0f;
    options->command=params.callback;
    options->extra_args = params.extra_args;
    options->frame_color = LColor(0.0, 0.0, 0.0, 1);
    options->frame_size = LVecBase4(-params.size * 0.25, params.size * 0.25, -5, 5);
    options->relief = rppanda::FLAT;
    options->thumb_options->frame_color = LColor(0.35, 0.53, 0.2, 1.0);
    options->thumb_options->relief = rppanda::FLAT;
    options->thumb_options->frame_size = LVecBase4(-2.5, 2.5, -5.0, 5.0);
    node_ = new rppanda::DirectSlider(params.parent, options);
}

float Slider::get_value(void) const
{
    return node_->get_value();
}

void Slider::set_value(float value)
{
    node_->set_value(value);
}

}
