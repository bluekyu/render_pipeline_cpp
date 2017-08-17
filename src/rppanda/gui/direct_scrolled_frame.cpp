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
 * This is C++ porting codes of direct/src/gui/DirectScrollFrame.py
 */

#include "render_pipeline/rppanda/gui/direct_scrolled_frame.hpp"

#include <pgScrollFrame.h>

#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

TypeHandle DirectScrolledFrame::type_handle_;

DirectScrolledFrame::Options::Options()
{
    frame_size = LVecBase4f(-0.5f, 0.5f, -0.5f, 0.5f);
    border_width = LVecBase2f(0.01f, 0.01f);

    vertical_scroll_options = std::make_shared<DirectScrollBar::Options>();
    horizontal_scroll_options = std::make_shared<DirectScrollBar::Options>();
}

DirectScrolledFrame::DirectScrolledFrame(NodePath parent, const std::shared_ptr<Options>& options): DirectScrolledFrame(new PGScrollFrame(""), parent, options, get_class_type())
{
}

DirectScrolledFrame::DirectScrolledFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle):
    DirectFrame(gui_item, parent, define_options(options), type_handle)
{
    float w = options->scroll_bar_width;

    options->vertical_scroll_options->border_width = options->border_width;
    options->vertical_scroll_options->frame_size = LVecBase4f(-w / 2.0f, w / 2.0f, -1, 1);
    options->vertical_scroll_options->orientation = VERTICAL;
    _vertical_scroll = new DirectScrollBar(*this, options->vertical_scroll_options);
    create_component("verticalScroll", boost::any(_vertical_scroll));

    options->horizontal_scroll_options->border_width = options->border_width;
    options->horizontal_scroll_options->frame_size = LVecBase4f(-1, 1, -w / 2.0f, w / 2.0f);
    options->horizontal_scroll_options->orientation = HORIZONTAL;
    _horizontal_scroll = new DirectScrollBar(*this, options->horizontal_scroll_options);
    create_component("horizontalScroll", boost::any(_horizontal_scroll));

    PGScrollFrame* item = get_gui_item();
    item->set_vertical_slider(_vertical_scroll->get_gui_item());
    item->set_horizontal_slider(_horizontal_scroll->get_gui_item());

    _canvas = NodePath(item->get_canvas_node());

    // Call option initialization functions
    if (is_exact_type(type_handle))
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

PGScrollFrame* DirectScrolledFrame::get_gui_item() const
{
    return DCAST(PGScrollFrame, _gui_item);
}

void DirectScrolledFrame::set_canvas_size(const LVecBase4f& canvas_size)
{
    get_gui_item()->set_virtual_frame(std::dynamic_pointer_cast<Options>(_options)->canvas_size=canvas_size);
}

void DirectScrolledFrame::set_manage_scroll_bars(bool manage_scroll_bars)
{
    get_gui_item()->set_manage_pieces(std::dynamic_pointer_cast<Options>(_options)->manage_scroll_bars=manage_scroll_bars);
}

void DirectScrolledFrame::set_auto_hide_scroll_bars(bool auto_hide_scroll_bars)
{
    get_gui_item()->set_auto_hide(std::dynamic_pointer_cast<Options>(_options)->auto_hide_scroll_bars=auto_hide_scroll_bars);
}

const std::shared_ptr<DirectScrolledFrame::Options>& DirectScrolledFrame::define_options(const std::shared_ptr<Options>& options)
{
    return options;
}

void DirectScrolledFrame::initialise_options(const std::shared_ptr<Options>& options)
{
    DirectFrame::initialise_options(options);

    f_init_ = true;
    set_canvas_size(options->canvas_size);
    set_manage_scroll_bars(options->manage_scroll_bars);
    set_auto_hide_scroll_bars(options->auto_hide_scroll_bars);
    f_init_ = false;
}

}
