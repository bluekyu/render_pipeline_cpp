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
 * This is C++ porting codes of direct/src/gui/DirectGuiBase.py
 */

#include "render_pipeline/rppanda/gui/direct_gui_base.hpp"

#include <pgItem.h>
#include <dconfig.h>
#include <texturePool.h>
#include <nodePathCollection.h>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include "rppanda/gui/config_rppanda_gui.hpp"

namespace rppanda {

TypeHandle DirectGuiBase::type_handle_;

boost::any& DirectGuiBase::create_component(const std::string& component_name, boost::any&& component)
{
    // Check for invalid component name
    if (component_name.find("_") != component_name.npos)
    {
        const std::string& msg = fmt::format("Component name \"{}\" must not contain \"_\"", component_name);
        rppanda_gui_cat.error() << msg << std::endl;
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
    component_info_[component_name] = component;

    return component_info_.at(component_name);
}

boost::any& DirectGuiBase::get_component(const std::string& name)
{
    return component_info_.at(name);
}

void DirectGuiBase::remove_component(const std::string& name)
{
    component_info_.erase(name);
}

void DirectGuiBase::bind(const std::string& ev_name, const Messenger::EventFunction& func)
{
    const std::string& gEvent = ev_name + get_gui_id();
    if (ConfigVariableBool("debug-directgui-msgs", false).get_value())
    {
        std::cout << gEvent << std::endl;
    }

    this->accept(gEvent, func);
}

void DirectGuiBase::unbind(const std::string& ev_name)
{
    this->ignore(ev_name + get_gui_id());
}

// ************************************************************************************************
TypeHandle DirectGuiWidget::type_handle_;

bool DirectGuiWidget::_snap_to_grid = false;
float DirectGuiWidget::_grid_spacing = 0.05f;
bool DirectGuiWidget::gui_edit = ConfigVariableBool("direct-gui-edit", false).get_value();
std::string DirectGuiWidget::inactive_init_state = gui_edit ? NORMAL : DISABLED;

DirectGuiWidget::Options::Options()
{
    state = NORMAL;
    relief = FLAT;
}

DirectGuiWidget::DirectGuiWidget(NodePath parent, const std::shared_ptr<Options>& options): DirectGuiWidget(new PGItem(""), parent, options, get_class_type())
{
}

DirectGuiWidget::DirectGuiWidget(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle):
    _gui_item(gui_item), _options(define_options(options))
{
    // Attach button to parent and make that self
    if (parent.is_empty())
        parent = ShowBase::get_global_ptr()->get_aspect_2d();
    NodePath::operator=(std::move(parent.attach_new_node(_gui_item, options->sort_order)));

    // Override automatically generated guiId
    //if (_gui_id.empty())
    gui_id_ = _gui_item->get_id();

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
    set_name(type_handle.get_name() + "-" + get_gui_id());

    for (int k=0; k < options->num_states; ++k)
    {
        // Create
        _state_node_path.push_back(_gui_item->get_state_def(k));

        // Initialize frame style
        _frame_style.push_back(PGFrameStyle());
    }

    // For holding bounds info
    ll_ = LPoint3(0);
    ur_ = LPoint3(0);

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
    if (is_exact_type(type_handle))
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

void DirectGuiWidget::enable_edit()
{
    this->bind(B2PRESS, [this](const Event*) { edit_start(); });
    this->bind(B2RELEASE, [this](const Event*) { edit_stop(); });
    this->bind(PRINT, [this](const Event* ev) {
        int indent = 0;
        if (ev->get_num_parameters() >= 1 && ev->get_parameter(0).is_int())
            indent = ev->get_parameter(0).get_int_value();
        print_config(indent);
    });
}

void DirectGuiWidget::disable_edit()
{
    this->unbind(B2PRESS);
    this->unbind(B2RELEASE);
    this->unbind(PRINT);
}

void DirectGuiWidget::edit_start()
{
    ShowBase* base = ShowBase::get_global_ptr();

    // TODO: implement
    //dgw->remove_task("guiEditTask");
    //const LPoint3f& widget2_render2d = dgw->get_pos(base->get_render_2d());
    //LPoint3f mouse2_render2d(
}

void DirectGuiWidget::edit_stop()
{
    // TODO: implement
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

void DirectGuiWidget::reset_frame_size()
{
    if (!f_init_)
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
        bounds_ = _options->frame_size.get();
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
        bounds_[0] - bw[0],
        bounds_[1] + bw[0],
        bounds_[2] - bw[1],
        bounds_[3] + bw[1]);
}

void DirectGuiWidget::set_frame_size(const LVecBase4& frame_size)
{
    _options->frame_size = frame_size;
    set_frame_size();
}

LVecBase4 DirectGuiWidget::get_bounds(int state)
{
    _state_node_path[state].calc_tight_bounds(ll_, ur_);
    // Scale bounds to give a pad around graphics
    LVector3 vec_right(LVector3::right());
    LVector3 vec_up(LVector3::up());
    PN_stdfloat left = vec_right.dot(ll_);
    PN_stdfloat right = vec_right.dot(ur_);
    PN_stdfloat bottom = vec_up.dot(ll_);
    PN_stdfloat top = vec_up.dot(ur_);
    ll_ = LPoint3(left, 0.0f, bottom);
    ur_ = LPoint3(right, 0.0f, top);
    bounds_ = LVecBase4(
        ll_[0] - _options->pad[0],
        ur_[0] + _options->pad[0],
        ll_[2] - _options->pad[1],
        ur_[2] + _options->pad[1]);
    return bounds_;
}

PGFrameStyle::Type DirectGuiWidget::get_frame_type(int state)
{
    return _frame_style[state].get_type();
}

void DirectGuiWidget::update_frame_style()
{
    if (!f_init_)
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
        Messenger::get_global_instance()->send(PRINT + npc.get_path(k).get_name(), EventParameter(indent+2));
}

const std::shared_ptr<DirectGuiWidget::Options>& DirectGuiWidget::define_options(const std::shared_ptr<Options>& options)
{
    if (!options->frame_visible_scale)
        options->frame_visible_scale = LVecBase2f(1.0f);

    return options;
}

void DirectGuiWidget::initialise_options(const std::shared_ptr<Options>& options)
{
    f_init_ = true;
    set_state(options->state);
    set_relief(options->relief);
    set_border_width(options->border_width);
    set_border_uv_width(options->border_uv_width);
    set_frame_size();
    set_frame_color(options->frame_color);
    set_frame_texture(options->frame_texture);
    set_frame_visible_scale(options->frame_visible_scale.get());
    set_pad(options->pad);
    f_init_ = false;
}

void DirectGuiWidget::frame_initialise_func()
{
    // Now allow changes to take effect
    update_frame_style();
    if (!_options->frame_size)
        reset_frame_size();
}

}
