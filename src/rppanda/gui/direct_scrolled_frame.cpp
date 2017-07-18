/**
 * This is C++ porting codes of direct/src/gui/DirectScrollFrame.py
 */

#include <render_pipeline/rppanda/gui/direct_scrolled_frame.hpp>

#include <pgScrollFrame.h>

#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

const std::type_info& DirectScrolledFrame::_type_handle(typeid(DirectScrolledFrame));

DirectScrolledFrame::Options::Options(void)
{
    frame_size = LVecBase4f(-0.5f, 0.5f, -0.5f, 0.5f);
    border_width = LVecBase2f(0.01f, 0.01f);

    vertical_scroll_options = std::make_shared<DirectScrollBar::Options>();
    horizontal_scroll_options = std::make_shared<DirectScrollBar::Options>();
}

DirectScrolledFrame::DirectScrolledFrame(NodePath parent, const std::shared_ptr<Options>& options): DirectScrolledFrame(new PGScrollFrame(""), parent, options, get_class_type())
{
}

DirectScrolledFrame::DirectScrolledFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle):
    DirectFrame(gui_item, parent, define_options(options), type_handle)
{
    float w = options->scroll_bar_width;

    options->vertical_scroll_options->border_width = options->border_width;
    options->vertical_scroll_options->frame_size = LVecBase4f(-w / 2.0f, w / 2.0f, -1, 1);
    options->vertical_scroll_options->orientation = VERTICAL;
    _vertical_scroll = std::make_shared<DirectScrollBar>(*this, options->vertical_scroll_options);
    create_component("verticalScroll", boost::any(_vertical_scroll));

    options->horizontal_scroll_options->border_width = options->border_width;
    options->horizontal_scroll_options->frame_size = LVecBase4f(-1, 1, -w / 2.0f, w / 2.0f);
    options->horizontal_scroll_options->orientation = HORIZONTAL;
    _horizontal_scroll = std::make_shared<DirectScrollBar>(*this, options->horizontal_scroll_options);
    create_component("horizontalScroll", boost::any(_horizontal_scroll));

    PGScrollFrame* item = get_gui_item();
    item->set_vertical_slider(_vertical_scroll->get_gui_item());
    item->set_horizontal_slider(_horizontal_scroll->get_gui_item());

    _canvas = NodePath(item->get_canvas_node());

    // Call option initialization functions
    if (get_class_type() == type_handle)
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

PGScrollFrame* DirectScrolledFrame::get_gui_item(void) const
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

    _f_init = true;
    set_canvas_size(options->canvas_size);
    set_manage_scroll_bars(options->manage_scroll_bars);
    set_auto_hide_scroll_bars(options->auto_hide_scroll_bars);
    _f_init = false;
}

}
