#include <render_pipeline/rppanda/gui/direct_gui_base.h>
#include <render_pipeline/rppanda/showbase/showbase.h>

#include <regex>

#include <pgItem.h>
#include <dconfig.h>
#include <texturePool.h>
#include <nodePathCollection.h>
#include <throw_event.h>

#include <boost/core/typeinfo.hpp>

#include <spdlog/fmt/fmt.h>

#include "rppanda/config_rppanda.h"

namespace rppanda {

const std::type_info& DirectGuiBase::_type_handle(typeid(DirectGuiBase));

boost::any& DirectGuiBase::create_component(const std::string& component_name, boost::any&& component)
{
    // Check for invalid component name
    if (component_name.find("_") != component_name.npos)
    {
        const std::string& msg = fmt::format("Component name \"{}\" must not contain \"_\"", component_name);
        rppanda_cat.error() << msg << std::endl;
        throw std::runtime_error(msg);
    }

	// Get construction keywords


	// Find any keyword arguments for this component
	//const std::string component_prefix(component_name + "_");
	//const size_t name_len = component_prefix.size();

	// First, walk through the option list looking for arguments
	// than refer to this component's group.

	// Return None if no widget class is specified
	if (component.empty())
	{
		static boost::any empty;
		return empty;
	}

	// Create the widget
	_component_info[component_name] = component;

	return _component_info.at(component_name);
}

boost::any& DirectGuiBase::get_component(const std::string& name)
{
	return _component_info.at(name);
}

void DirectGuiBase::remove_component(const std::string& name)
{
	_component_info.erase(name);
}

void DirectGuiBase::bind(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data)
{
	const std::string& gEvent = ev_name + get_gui_id();
	if (ConfigVariableBool("debug-directgui-msgs", false).get_value())
	{
		std::cout << gEvent << std::endl;
	}

	this->accept(gEvent, func, user_data);
}

void DirectGuiBase::unbind(const std::string& ev_name)
{
	this->ignore(ev_name + get_gui_id());
}

// ************************************************************************************************
const std::type_info& DirectGuiWidget::_type_handle(typeid(DirectGuiWidget));

bool DirectGuiWidget::_snap_to_grid = false;
float DirectGuiWidget::_grid_spacing = 0.05f;
bool DirectGuiWidget::gui_edit = ConfigVariableBool("direct-gui-edit", false).get_value();
std::string DirectGuiWidget::inactive_init_state = gui_edit ? NORMAL : DISABLED;

DirectGuiWidget::Options::Options(void)
{
	state = NORMAL;
	relief = FLAT;
}

DirectGuiWidget::DirectGuiWidget(NodePath parent, const std::shared_ptr<Options>& options): DirectGuiWidget(new PGItem(""), parent, options, get_class_type())
{
}

DirectGuiWidget::DirectGuiWidget(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle):
	_gui_item(gui_item), _options(define_options(options))
{
	// Attach button to parent and make that self
	if (parent.is_empty())
		parent = ShowBase::get_global_ptr()->get_aspect_2d();
	NodePath::operator=(std::move(parent.attach_new_node(_gui_item, options->sort_order)));

	// Override automatically generated guiId
	//if (_gui_id.empty())
	_gui_id = _gui_item->get_id();

	// Update pose to initial values
	if (options->pos)
		set_pos(options->pos.get());
	if (options->hpr)
		set_hpr(options->hpr.get());
	if (options->scale)
		set_scale(options->scale.get());
	if (options->color)
		set_color(options->color.get());

	// Initialize names
	// Putting the class name in helps with debugging.
	const std::regex re("^class ([A-Za-z_][A-Za-z0-9_]*::)*(.*)$");
	const std::string demangled_name(boost::core::demangled_name(type_handle));
	std::smatch results;
	std::regex_search(demangled_name, results, re);
	set_name(results[results.size()-1].str() + "-" + get_gui_id());

	for (int k=0; k < options->num_states; ++k)
	{
		// Create
		_state_node_path.push_back(_gui_item->get_state_def(k));

		// Initialize frame style
		_frame_style.push_back(PGFrameStyle());
	}

	// For holding bounds info
	_ll = LPoint3(0);
	_ur = LPoint3(0);

	// Is drag and drop enabled?
	if (options->enable_edit && gui_edit)
		enable_edit();

	// Set up event handling
	int suppress_flags = 0;
	if (options->suppress_mouse)
	{
		suppress_flags |= MouseWatcherRegion::SF_mouse_button;
		suppress_flags |= MouseWatcherRegion::SF_mouse_position;
	}
	if (options->suppress_keys)
	{
		suppress_flags |= MouseWatcherRegion::SF_other_button;
	}
	_gui_item->set_suppress_flags(suppress_flags);

	// Call option initialization functions
	if (get_class_type() == type_handle)
	{
		initialise_options(options);
		frame_initialise_func();
	}
}

void DirectGuiWidget::enable_edit(void)
{
	this->bind(B2PRESS, edit_start, this);
	this->bind(B2RELEASE, edit_stop, this);
	this->bind(PRINT, [](const Event* ev, void* user_data) {
		int indent = 0;
		if (ev->get_num_parameters() >= 1 && ev->get_parameter(0).is_int())
			indent = ev->get_parameter(0).get_int_value();
		reinterpret_cast<DirectGuiWidget*>(user_data)->print_config(indent);
	}, this);
}

void DirectGuiWidget::disable_edit(void)
{
	this->unbind(B2PRESS);
	this->unbind(B2RELEASE);
	this->unbind(PRINT);
}

void DirectGuiWidget::edit_start(const Event* ev, void* user_data)
{
	DirectGuiWidget* dgw = reinterpret_cast<DirectGuiWidget*>(user_data);
	ShowBase* base = ShowBase::get_global_ptr();

	// TODO: implement
	//dgw->remove_task("guiEditTask");
	//const LPoint3f& widget2_render2d = dgw->get_pos(base->get_render_2d());
	//LPoint3f mouse2_render2d(
}

void DirectGuiWidget::edit_stop(const Event* ev, void* user_data)
{
	// TODO: implement
	DirectGuiWidget* dgw = reinterpret_cast<DirectGuiWidget*>(user_data);
	//dgw->remove_task("guiEditTask");
}

void DirectGuiWidget::set_state(const std::string& state)
{
	_options->state = state;
	if (state == NORMAL)
		_gui_item->set_active(true);
	else
		_gui_item->set_active(false);
}

void DirectGuiWidget::set_state(bool state)
{
	_options->state = state ? NORMAL : DISABLED;
	_gui_item->set_active(state);
}

void DirectGuiWidget::reset_frame_size(void)
{
	if (!_f_init)
		set_frame_size(true);
}

void DirectGuiWidget::set_frame_size(bool clear_frame)
{
	// Use ready state to determine frame Type
	PGFrameStyle::Type frame_type = get_frame_type();
	LVecBase2 bw(0, 0);
	if (_options->frame_size)
	{
		//  Use user specified bounds
		_bounds = _options->frame_size.get();
	}
	else
	{
		if (clear_frame && (frame_type != PGFrameStyle::Type::T_none))
		{
			_frame_style[0].set_type(PGFrameStyle::Type::T_none);
			_gui_item->set_frame_style(0, _frame_style[0]);
			// To force an update of the button
			_gui_item->get_state_def(0);
		}
		// Clear out frame before computing bounds
		get_bounds();
		// Restore frame style if necessary
		if (frame_type != PGFrameStyle::Type::T_none)
		{
			_frame_style[0].set_type(frame_type);
			_gui_item->set_frame_style(0, _frame_style[0]);
		}

		if ((frame_type != PGFrameStyle::Type::T_none) && (frame_type != PGFrameStyle::Type::T_flat))
		{
			bw = get_border_width();
		}
	}

	// Set frame to new dimensions
	_gui_item->set_frame(
		_bounds[0] - bw[0],
		_bounds[1] + bw[0],
		_bounds[2] - bw[1],
		_bounds[3] + bw[1]);
}

void DirectGuiWidget::set_frame_size(const LVecBase4& frame_size)
{
	_options->frame_size = frame_size;
	set_frame_size();
}

LVecBase4 DirectGuiWidget::get_bounds(int state)
{
	_state_node_path[state].calc_tight_bounds(_ll, _ur);
	// Scale bounds to give a pad around graphics
	LVector3 vec_right(LVector3::right());
	LVector3 vec_up(LVector3::up());
	PN_stdfloat left = vec_right.dot(_ll);
	PN_stdfloat right = vec_right.dot(_ur);
	PN_stdfloat bottom = vec_up.dot(_ll);
	PN_stdfloat top = vec_up.dot(_ur);
	_ll = LPoint3(left, 0.0f, bottom);
	_ur = LPoint3(right, 0.0f, top);
	_bounds = LVecBase4(
		_ll[0] - _options->pad[0],
		_ur[0] - _options->pad[0],
		_ll[2] - _options->pad[1],
		_ur[2] - _options->pad[1]);
	return _bounds;
}

PGFrameStyle::Type DirectGuiWidget::get_frame_type(int state)
{
	return _frame_style[state].get_type();
}

void DirectGuiWidget::update_frame_style(void)
{
	if (!_f_init)
	{
		for (int k=0; k < _options->num_states; ++k)
		{
			_gui_item->set_frame_style(k, _frame_style[k]);
		}
	}
}

void DirectGuiWidget::set_relief(const std::string& relief_name)
{
	// Convert string to frame style int
	set_relief(FrameStyleDict.at(relief_name));
}

void DirectGuiWidget::set_relief(PGFrameStyle::Type relief)
{
	_options->relief = relief;

	// Set style
	if (relief == RAISED)
	{
		for (int i=0; i < _options->num_states; ++i)
		{
			if (std::find(_options->inverted_frames.begin(), _options->inverted_frames.end(), i) != std::end(_options->inverted_frames))
				_frame_style[1].set_type(SUNKEN);
			else
				_frame_style[i].set_type(RAISED);
		}
	}
	else if (relief == SUNKEN)
	{
		for (int i=0; i < _options->num_states; ++i)
		{
			if (std::find(_options->inverted_frames.begin(), _options->inverted_frames.end(), i) != std::end(_options->inverted_frames))
				_frame_style[1].set_type(RAISED);
			else
				_frame_style[i].set_type(SUNKEN);
		}
	}
	else
	{
		for (int i=0; i < _options->num_states; ++i)
			_frame_style[i].set_type(relief);
	}
	// Apply styles
	update_frame_style();
}

void DirectGuiWidget::set_frame_color(const LColor& frame_color)
{
	_options->frame_color = frame_color;
	for (int k=0; k < _options->num_states; ++k)
		_frame_style[k].set_color(frame_color);
	update_frame_style();
}

void DirectGuiWidget::set_frame_texture(Texture* texture)
{
	set_frame_texture(std::vector<PT(Texture)>({texture}));
}

void DirectGuiWidget::set_frame_texture(const std::string& texture_path)
{
	set_frame_texture(TexturePool::load_texture(texture_path));
}

void DirectGuiWidget::set_frame_texture(const std::vector<std::string>& texture_paths)
{
	std::vector<PT(Texture)> textures;
	for (const auto& texture_path: texture_paths)
		textures.push_back(TexturePool::load_texture(texture_path));
	set_frame_texture(textures);
}

void DirectGuiWidget::set_frame_texture(const std::vector<PT(Texture)>& textures)
{
	_options->frame_texture = textures;

	PT(Texture) texture;
	for (int i=0; i < _options->num_states; ++i)
	{
		if (i >= textures.size())
			texture = textures.empty() ? nullptr : textures.back();
		else
			texture = textures[i];

		if (texture)
			_frame_style[i].set_texture(texture);
		else
			_frame_style[i].clear_texture();
	}
	update_frame_style();
}

void DirectGuiWidget::set_frame_visible_scale(const LVecBase2& scale)
{
	_options->frame_visible_scale = scale;
	for (int i = 0; i < _options->num_states; ++i)
		_frame_style[i].set_visible_scale(scale);
	update_frame_style();
}

void DirectGuiWidget::set_border_width(const LVecBase2& border_width)
{
	_options->border_width = border_width;
	for (int i = 0; i < _options->num_states; ++i)
		_frame_style[i].set_width(border_width);
	update_frame_style();
}

void DirectGuiWidget::set_border_uv_width(const LVecBase2& border_uv_width)
{
	_options->border_uv_width = border_uv_width;
	for (int i=0; i < _options->num_states; ++i)
		_frame_style[i].set_uv_width(border_uv_width);
	update_frame_style();
}

void DirectGuiWidget::print_config(int indent)
{
	std::string space(' ', indent);

    std::cout << fmt::format("{}{} - DirectGuiWidget", space, get_gui_id()) << std::endl;
	std::cout << space << "Pos:   " << get_pos() << std::endl;
	std::cout << space << "Scale: " << get_scale() << std::endl;
	
	// Print out children info
	const auto& npc = get_children();
	for (int k=0, k_end=npc.get_num_paths(); k < k_end; ++k)
		throw_event_directly(*EventHandler::get_global_event_handler(), PRINT + npc.get_path(k).get_name(), EventParameter(indent+2));
}

const std::shared_ptr<DirectGuiWidget::Options>& DirectGuiWidget::define_options(const std::shared_ptr<Options>& options)
{
	if (!options->frame_visible_scale)
		options->frame_visible_scale = LVecBase2f(1.0f);

	return options;
}

void DirectGuiWidget::initialise_options(const std::shared_ptr<Options>& options)
{
	_f_init = true;
	set_state(options->state);
	set_relief(options->relief);
	set_border_width(options->border_width);
	set_border_uv_width(options->border_uv_width);
	set_frame_size();
	set_frame_color(options->frame_color);
	set_frame_texture(options->frame_texture);
	set_frame_visible_scale(options->frame_visible_scale.get());
	reset_frame_size();
	_f_init = false;
}

void DirectGuiWidget::frame_initialise_func(void)
{
	// Now allow changes to take effect
	update_frame_style();
	if (!_options->frame_size)
		reset_frame_size();
}

}	// namespace rppanda
