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

#include "render_pipeline/rpcore/gui/draggable_window.hpp"

#include <genericAsyncTask.h>
#include <graphicsWindow.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/gui/direct_frame.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"
#include "render_pipeline/rppanda/gui/direct_button.hpp"

namespace rpcore {

DraggableWindow::DraggableWindow(int width, int height, const std::string& title, NodePath parent): RPObject(std::string("Window-")+title),
    _width(width), _height(height), _title(title)
{
    _parent = parent.is_empty() ? Globals::base->get_pixel_2d() : parent;
}

DraggableWindow::~DraggableWindow(void)
{
    delete _window_title;
}

void DraggableWindow::center_on_screen(void)
{
    _context_scale = 1.0f / _parent.get_sx();
    _context_width = Globals::native_resolution.get_x() * _context_scale;
    _context_height = Globals::native_resolution.get_y() * _context_scale;
    set_pos(LVecBase2f((_context_width - _width) / 2.0f, (_context_height - _height) / 2.0f));
}

void DraggableWindow::set_title(const std::string& title)
{
    _title = title;
    _window_title->set_text(title);
}

void DraggableWindow::create_components(void)
{
    _node = _parent.attach_new_node("Window");
    _node.set_pos(_pos.get_x(), 1, -_pos.get_y());
    int border_px = 1;
    const LVecBase4f border_frame_size(-border_px, _width + border_px, border_px, -_height - border_px);

    auto border_frame_options = std::make_shared<rppanda::DirectFrame::Options>();
    border_frame_options->pos = LVecBase3f(0, 1, 0);
    border_frame_options->frame_size = border_frame_size;
    border_frame_options->frame_color = LColorf(24 / 255.0, 131 / 255.0, 215 / 255.0, 1);
    border_frame_options->state = rppanda::NORMAL;
    _border_frame = new rppanda::DirectFrame(_node, border_frame_options);

    auto background_options = std::make_shared<rppanda::DirectFrame::Options>();
    background_options->pos = LVecBase3f(0, 1, 0);
    background_options->frame_size = LVecBase4f(0, _width, 0, -_height);
    background_options->frame_color = LColorf(0.1f, 0.1f, 0.1f, 1.0f);
    _background = new rppanda::DirectFrame(_node, background_options);

    auto title_bar_options = std::make_shared<rppanda::DirectFrame::Options>();
    title_bar_options->pos = LVecBase3f(0, 1, 0);
    title_bar_options->frame_size = LVecBase4f(0, _width, 0, -25);
    title_bar_options->frame_color = LColorf(1, 1, 1, 1);
    title_bar_options->state = rppanda::NORMAL;
    _title_bar = new rppanda::DirectFrame(_node, title_bar_options);

    Text::Parameters params;
    params.text = _title;
    params.parent = _node;
    params.x = 8.0f;
    params.y = 17.0f;
    params.size = 13.0f;
    params.align = "left";
    params.color = LVecBase3f(0.15f);
    params.may_change = true;
    _window_title = new Text(params);

    auto btn_close_options = std::make_shared<rppanda::DirectButton::Options>();
    btn_close_options->relief = rppanda::FLAT;
    btn_close_options->press_effect = true;
    btn_close_options->pos = LVecBase3f(_width - 22, 1, -12);
    btn_close_options->frame_color = LColorf(1.0f, 0.2f, 0.2f, 0.5f);
    btn_close_options->scale = LVecBase3f(45 / 2.0f, 1.0f, 24 / 2.0f);
    btn_close_options->image = { std::make_shared<rppanda::ImageInput>(std::string("/$$rp/data/gui/close_window.png")) };
    _btn_close = new rppanda::DirectButton(_node, btn_close_options);

    // Init bindings
    _btn_close->set_transparency(TransparencyAttrib::M_alpha);
    _btn_close->bind(rppanda::B1CLICK, request_close, this);
    _btn_close->bind(rppanda::WITHIN, on_close_btn_hover, this);
    _btn_close->bind(rppanda::WITHOUT, on_close_btn_out, this);
    _title_bar->bind(rppanda::B1PRESS, start_drag, this);
    _title_bar->bind(rppanda::B1RELEASE, stop_drag, this);
}

void DraggableWindow::start_drag(const Event* ev, void* user_data)
{
    DraggableWindow* self = reinterpret_cast<DraggableWindow*>(user_data);
    self->_dragging = true;
    self->_node.detach_node();
    self->_node.reparent_to(self->_parent);
    GenericAsyncTask* task = Globals::base->add_task(on_tick, self, "UIWindowDrag");
    task->set_upon_death(stop_drag);

    self->_drag_offset = self->_pos - self->get_mouse_pos();
}

void DraggableWindow::on_close_btn_hover(const Event* ev, void* user_data)
{
    reinterpret_cast<DraggableWindow*>(user_data)->_btn_close->set_image(std::make_shared<rppanda::ImageInput>(std::string("/$$rp/data/gui/close_window_hover.png")));
}

void DraggableWindow::on_close_btn_out(const Event* ev, void* user_data)
{
    reinterpret_cast<DraggableWindow*>(user_data)->_btn_close->set_image(std::make_shared<rppanda::ImageInput>(std::string("/$$rp/data/gui/close_window.png")));
}

void DraggableWindow::request_close(const Event* ev, void* user_data)
{
    reinterpret_cast<DraggableWindow*>(user_data)->hide();
}

void DraggableWindow::stop_drag(const Event* ev, void* user_data)
{
    Globals::base->remove_task("UIWindowDrag");
    reinterpret_cast<DraggableWindow*>(user_data)->_dragging = false;
}

LVecBase2 DraggableWindow::get_mouse_pos(void) const
{
    float mouse_x = (float)Globals::base->get_win()->get_pointer(0).get_x();
    float mouse_y = (float)Globals::base->get_win()->get_pointer(0).get_y();
    return LVecBase2(mouse_x, mouse_y) * _context_scale;
}

AsyncTask::DoneStatus DraggableWindow::on_tick(GenericAsyncTask* task, void* user_data)
{
    DraggableWindow* self = reinterpret_cast<DraggableWindow*>(user_data);
    self->set_pos(self->get_mouse_pos() + self->_drag_offset);
    return AsyncTask::DS_cont;
}

}
