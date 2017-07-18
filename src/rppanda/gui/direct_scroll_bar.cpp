/**
 * This is C++ porting codes of direct/src/gui/DirectScrollBar.py
 */

#include <render_pipeline/rppanda/gui/direct_scroll_bar.hpp>

#include <pgSliderBar.h>

#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

const std::type_info& DirectScrollBar::_type_handle(typeid(DirectScrollBar));

DirectScrollBar::Options::Options(void)
{
    state = NORMAL;
    frame_color = LColor(0.6, 0.6, 0.6, 1.0);
    orientation = HORIZONTAL;

    thumb_options = std::make_shared<DirectButton::Options>();
    inc_button_options = std::make_shared<DirectButton::Options>();
    dec_button_options = std::make_shared<DirectButton::Options>();
}

DirectScrollBar::DirectScrollBar(NodePath parent, const std::shared_ptr<Options>& options): DirectScrollBar(new PGSliderBar(""), parent, options, get_class_type())
{
}

DirectScrollBar::DirectScrollBar(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle):
    DirectFrame(gui_item, parent, define_options(options), type_handle)
{
    options->thumb_options->border_width = options->border_width;
    _thumb = std::make_shared<DirectButton>(*this, options->thumb_options);
    create_component("thumb", boost::any(_thumb));

    options->inc_button_options->border_width = options->border_width;
    _inc_button = std::make_shared<DirectButton>(*this, options->inc_button_options);
    create_component("incButton", boost::any(_thumb));

    options->dec_button_options->border_width = options->border_width;
    _dec_button = std::make_shared<DirectButton>(*this, options->dec_button_options);
    create_component("decButton", boost::any(_thumb));

    if (!_dec_button->get_frame_size() && _dec_button->get_bounds() == LVecBase4(0))
    {
        const auto& f = options->frame_size.get();
        if (options->orientation == HORIZONTAL)
            _dec_button->set_frame_size(LVecBase4(f[0]*0.05, f[1]*0.05, f[2], f[3]));
        else
            _dec_button->set_frame_size(LVecBase4(f[0], f[1], f[2]*0.05, f[3]*0.05));
    }

    if (!_inc_button->get_frame_size() && _inc_button->get_bounds() == LVecBase4(0))
    {
        const auto& f = options->frame_size.get();
        if (options->orientation == HORIZONTAL)
            _inc_button->set_frame_size(LVecBase4(f[0]*0.05, f[1]*0.05, f[2], f[3]));
        else
            _inc_button->set_frame_size(LVecBase4(f[0], f[1], f[2]*0.05, f[3]*0.05));
    }

    PGSliderBar* item = get_gui_item();
    item->set_thumb_button(_thumb->get_gui_item());
    item->set_left_button(_dec_button->get_gui_item());
    item->set_right_button(_inc_button->get_gui_item());

    // Bind command function
    this->bind(ADJUST, command_func, this);

    // Call option initialization functions
    if (get_class_type() == type_handle)
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

PGSliderBar* DirectScrollBar::get_gui_item(void) const
{
    return DCAST(PGSliderBar, _gui_item);
}

void DirectScrollBar::set_range(const LVecBase2& range)
{
    const auto& opt = std::dynamic_pointer_cast<Options>(_options);
    opt->range = range;

    PGSliderBar* item = get_gui_item();
    float v = opt->value;
    item->set_range(range[0], range[1]);
    item->set_value(v);
}

void DirectScrollBar::set_value(PN_stdfloat value)
{
    get_gui_item()->set_value(std::dynamic_pointer_cast<Options>(_options)->value=value);
}

void DirectScrollBar::set_scroll_size(PN_stdfloat scroll_size)
{
    get_gui_item()->set_scroll_size(std::dynamic_pointer_cast<Options>(_options)->scroll_size=scroll_size);
}

void DirectScrollBar::set_page_size(PN_stdfloat page_size)
{
    get_gui_item()->set_page_size(std::dynamic_pointer_cast<Options>(_options)->page_size=page_size);
}

void DirectScrollBar::set_orientation(const std::string& orientation)
{
    std::dynamic_pointer_cast<Options>(_options)->orientation = orientation;
    if (orientation == HORIZONTAL)
        get_gui_item()->set_axis(LVector3(1, 0, 0));
    else if (orientation == VERTICAL)
        get_gui_item()->set_axis(LVector3(0, 0, -1));
    else if (orientation == VERTICAL_INVERTED)
        get_gui_item()->set_axis(LVector3(0, 0, 1));
    else
        throw std::runtime_error(std::string("Invalid value for orientation: ") + orientation);
}

void DirectScrollBar::set_manage_buttons(bool manage_buttons)
{
    get_gui_item()->set_manage_pieces(std::dynamic_pointer_cast<Options>(_options)->manage_buttons=manage_buttons);
}

void DirectScrollBar::set_resize_thumb(bool resize_thumb)
{
    get_gui_item()->set_resize_thumb(std::dynamic_pointer_cast<Options>(_options)->resize_thumb=resize_thumb);
}

void DirectScrollBar::command_func(const Event* ev, void* user_data)
{
    DirectScrollBar* self = reinterpret_cast<DirectScrollBar*>(user_data);

    // Store the updated value in self['value']
    std::dynamic_pointer_cast<Options>(self->_options)->value = self->get_gui_item()->get_value();

    // TODO: implements
}

const std::shared_ptr<DirectScrollBar::Options>& DirectScrollBar::define_options(const std::shared_ptr<Options>& options)
{
    if (!options->frame_size)
    {
        if ((options->orientation == VERTICAL || options->orientation == VERTICAL_INVERTED))
        {
            // These are the default options for a vertical layout.
            options->frame_size = LVecBase4(-0.04f, 0.04f, -0.5f, 0.5f);
        }
        else
        {
            // These are the default options for a horizontal layout.
            options->frame_size = LVecBase4(-0.5f, 0.5f, -0.04f, 0.04f);
        }
    }

    return options;
}

void DirectScrollBar::initialise_options(const std::shared_ptr<Options>& options)
{
    DirectFrame::initialise_options(options);

    f_init_ = true;
    set_range(options->range);
    set_value(options->value);
    set_scroll_size(options->scroll_size);
    set_page_size(options->page_size);
    set_orientation(options->orientation);
    set_manage_buttons(options->manage_buttons);
    set_resize_thumb(options->resize_thumb);
    f_init_ = false;
}

}
