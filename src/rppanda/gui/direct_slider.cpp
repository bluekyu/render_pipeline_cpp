/**
 * Copyright (c) 2008, Carnegie Mellon University.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Carnegie Mellon University nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * (This is the Modified BSD License.  See also
 * http://www.opensource.org/licenses/bsd-license.php )
 */

/**
 * This is C++ porting codes of direct/src/gui/DirectSlider.py
 */

#include "render_pipeline/rppanda/gui/direct_slider.hpp"

#include <pgSliderBar.h>

#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

TypeHandle DirectSlider::type_handle_;

DirectSlider::Options::Options()
{
    state = DGG_NORMAL;
    frame_color = LColor(0.6, 0.6, 0.6, 1);
    orientation = DGG_HORIZONTAL;

    thumb_options = std::make_shared<DirectButton::Options>();
}

DirectSlider::DirectSlider(NodePath parent, const std::shared_ptr<Options>& options): DirectSlider(new PGSliderBar(""), parent, options, get_class_type())
{
}

DirectSlider::DirectSlider(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle):
    DirectFrame(gui_item, parent, define_options(options), type_handle)
{
    options->thumb_options->border_width = options->border_width;
    thumb_ = new DirectButton(*this, options->thumb_options);
    create_component("thumb", boost::any(thumb_));

    if (!thumb_->get_frame_size() && thumb_->get_bounds() == LVecBase4(0.0))
    {
        // Compute a default frameSize for the thumb.
        const auto& f = options->frame_size.value();
        if (options->orientation == DGG_HORIZONTAL)
            thumb_->set_frame_size(LVecBase4(f[0]*0.05, f[1]*0.05, f[2], f[3]));
        else
            thumb_->set_frame_size(LVecBase4(f[0], f[1], f[2]*0.05, f[3]*0.05));
    }

    get_gui_item()->set_thumb_button(thumb_->get_gui_item());

    // Bind command function
    this->bind(DGG_ADJUST, [this](const Event*) { command_func(); });

    // Call option initialization functions
    if (is_exact_type(type_handle))
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

DirectSlider::~DirectSlider() = default;

PGSliderBar* DirectSlider::get_gui_item() const
{
    return DCAST(PGSliderBar, _gui_item);
}

void DirectSlider::set_range(LVecBase2 range)
{
    // Try to preserve the value across a setRange call.
    const auto& opt = std::dynamic_pointer_cast<Options>(_options);
    opt->range = range;

    PGSliderBar* item = get_gui_item();
    float v = opt->value;
    item->set_range(range[0], range[1]);
    item->set_value(v);
}

void DirectSlider::set_value(float value)
{
    get_gui_item()->set_value(std::dynamic_pointer_cast<Options>(_options)->value=value);
}

float DirectSlider::get_value() const
{
    return get_gui_item()->get_value();
}

float DirectSlider::get_ratio() const
{
    return get_gui_item()->get_ratio();
}

void DirectSlider::set_scroll_size(float scroll_size)
{
    get_gui_item()->set_scroll_size(std::dynamic_pointer_cast<Options>(_options)->scroll_size=scroll_size);
}

void DirectSlider::set_page_size(float page_size)
{
    get_gui_item()->set_page_size(std::dynamic_pointer_cast<Options>(_options)->page_size=page_size);
}

void DirectSlider::set_orientation(const std::string& orientation)
{
    std::dynamic_pointer_cast<Options>(_options)->orientation = orientation;
    if (orientation == DGG_HORIZONTAL)
        get_gui_item()->set_axis(LVector3(1, 0, 0));
    else if (orientation == DGG_VERTICAL)
        get_gui_item()->set_axis(LVector3(0, 0, 1));
    else
        throw std::runtime_error(std::string("Invalid value for orientation: ") + orientation);
}

const std::shared_ptr<DirectSlider::Options>& DirectSlider::define_options(const std::shared_ptr<Options>& options)
{
    if (options->orientation == DGG_VERTICAL)
    {
        // These are the default options for a vertical layout.
        if (!options->frame_size)
            options->frame_size = LVecBase4(-0.08, 0.08, -1, 1);
        if (!options->frame_visible_scale)
            options->frame_visible_scale = LVecBase2(0.25, 1);
    }
    else
    {
        // These are the default options for a horizontal layout.
        if (!options->frame_size)
            options->frame_size = LVecBase4(-1, 1, -0.08, 0.08);
        if (!options->frame_visible_scale)
            options->frame_visible_scale = LVecBase2(1, 0.25);
    }

    return options;
}

void DirectSlider::command_func()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);

    // Store the updated value in self['value']
    options->value = get_gui_item()->get_value();
    if (options->command)
        options->command();
}

void DirectSlider::initialise_options(const std::shared_ptr<Options>& options)
{
    DirectFrame::initialise_options(options);

    f_init_ = true;
    set_range(options->range);
    set_value(options->value);
    set_scroll_size(options->scroll_size);
    set_page_size(options->page_size);
    set_orientation(options->orientation);
    f_init_ = false;
}

}
