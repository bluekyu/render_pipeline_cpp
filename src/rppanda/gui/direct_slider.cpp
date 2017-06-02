#include <render_pipeline/rppanda/gui/direct_slider.hpp>

#include <pgSliderBar.h>

namespace rppanda {

const std::type_info& DirectSlider::_type_handle(typeid(DirectSlider));

DirectSlider::Options::Options(void)
{
	state = NORMAL;
	frame_color = LColor(0.6, 0.6, 0.6, 1);
	orientation = HORIZONTAL;

	thumb_options = std::make_shared<DirectButton::Options>();
}

DirectSlider::DirectSlider(NodePath parent, const std::shared_ptr<Options>& options): DirectSlider(new PGSliderBar(""), parent, options, get_class_type())
{
}

DirectSlider::DirectSlider(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle):
	DirectFrame(gui_item, parent, define_options(options), type_handle)
{
	options->thumb_options->border_width = options->border_width;
	_thumb = std::make_shared<DirectButton>(*this, options->thumb_options);
	create_component("thumb", boost::any(_thumb));

	if (!_thumb->get_frame_size() && _thumb->get_bounds() == LVecBase4(0.0))
	{
		// Compute a default frameSize for the thumb.
		const auto& f = options->frame_size.get();
		if (options->orientation == HORIZONTAL)
			_thumb->set_frame_size(LVecBase4(f[0]*0.05, f[1]*0.05, f[2], f[3]));
		else
			_thumb->set_frame_size(LVecBase4(f[0], f[1], f[2]*0.05, f[3]*0.05));
	}

	get_gui_item()->set_thumb_button(_thumb->get_gui_item());

	// Bind command function
	this->bind(ADJUST, command_func, this);

	// Call option initialization functions
	if (get_class_type() == type_handle)
	{
		initialise_options(options);
		frame_initialise_func();
	}
}

PGSliderBar* DirectSlider::get_gui_item(void) const
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

float DirectSlider::get_value(void) const
{
	return get_gui_item()->get_value();
}

float DirectSlider::get_ratio(void) const
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
	if (orientation == HORIZONTAL)
		get_gui_item()->set_axis(LVector3f(1, 0, 0));
	else if (orientation == VERTICAL)
		get_gui_item()->set_axis(LVector3f(0, 0, 1));
	else
		throw std::runtime_error(std::string("Invalid value for orientation: ") + orientation);
}

const std::shared_ptr<DirectSlider::Options>& DirectSlider::define_options(const std::shared_ptr<Options>& options)
{
	if (options->orientation == VERTICAL)
	{
		// These are the default options for a vertical layout.
		if (!options->frame_size)
			options->frame_size = LVecBase4f(-0.08, 0.08, -1, 1);
		if (!options->frame_visible_scale)
			options->frame_visible_scale = LVecBase2(0.25, 1);
	}
	else
	{
		// These are the default options for a horizontal layout.
		if (!options->frame_size)
			options->frame_size = LVecBase4f(-1, 1, -0.08, 0.08);
		if (!options->frame_visible_scale)
			options->frame_visible_scale = LVecBase2(1, 0.25);
	}

	return options;
}

void DirectSlider::command_func(const Event* ev, void* user_data)
{
	const DirectSlider* self = reinterpret_cast<DirectSlider*>(user_data);
	const auto& options = std::dynamic_pointer_cast<Options>(self->_options);

	// Store the updated value in self['value']
	options->value = self->get_gui_item()->get_value();
	if (options->command)
		options->command(options->extra_args);
}

void DirectSlider::initialise_options(const std::shared_ptr<Options>& options)
{
	DirectFrame::initialise_options(options);

	_f_init = true;
	set_range(options->range);
	set_value(options->value);
	set_scroll_size(options->scroll_size);
	set_page_size(options->page_size);
	set_orientation(options->orientation);
	_f_init = false;
}

}
