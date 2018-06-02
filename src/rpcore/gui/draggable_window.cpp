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

#include <graphicsWindow.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/gui/direct_frame.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"
#include "render_pipeline/rppanda/gui/direct_button.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"

namespace rpcore {

const std::string DraggableWindow::Default::title = "Window";

DraggableWindow::DraggableWindow(int width, int height, const std::string& title, NodePath parent): RPObject(std::string("Window-")+title),
    width_(width), height_(height), title_(title)
{
    parent_ = parent.is_empty() ? Globals::base->get_pixel_2d() : parent;
}

DraggableWindow::~DraggableWindow() = default;

void DraggableWindow::center_on_screen()
{
    context_scale_ = 1.0f / parent_.get_sx();
    context_width_ = Globals::native_resolution.get_x() * context_scale_;
    context_height_ = Globals::native_resolution.get_y() * context_scale_;
    set_pos(LVecBase2((context_width_ - width_) / 2.0f, (context_height_ - height_) / 2.0f));
}

void DraggableWindow::set_title(const std::string& title)
{
    title_ = title;
    window_title_->set_text(title);
}

void DraggableWindow::create_components()
{
    node_ = parent_.attach_new_node("Window");
    node_.set_pos(pos_.get_x(), 1, -pos_.get_y());
    int border_px = 1;
    const LVecBase4f border_frame_size(-border_px, width_ + border_px, border_px, -height_ - border_px);

    auto border_frame_options = std::make_shared<rppanda::DirectFrame::Options>();
    border_frame_options->pos = LVecBase3f(0, 1, 0);
    border_frame_options->frame_size = border_frame_size;
    border_frame_options->frame_color = LColorf(24 / 255.0, 131 / 255.0, 215 / 255.0, 1);
    border_frame_options->state = rppanda::DGG_NORMAL;
    border_frame_ = new rppanda::DirectFrame(node_, border_frame_options);

    auto background_options = std::make_shared<rppanda::DirectFrame::Options>();
    background_options->pos = LVecBase3f(0, 1, 0);
    background_options->frame_size = LVecBase4f(0, width_, 0, -height_);
    background_options->frame_color = LColorf(0.1f, 0.1f, 0.1f, 1.0f);
    background_ = new rppanda::DirectFrame(node_, background_options);

    auto title_bar_options = std::make_shared<rppanda::DirectFrame::Options>();
    title_bar_options->pos = LVecBase3f(0, 1, 0);
    title_bar_options->frame_size = LVecBase4f(0, width_, 0, -25);
    title_bar_options->frame_color = LColorf(1, 1, 1, 1);
    title_bar_options->state = rppanda::DGG_NORMAL;
    title_bar_ = new rppanda::DirectFrame(node_, title_bar_options);

    window_title_ = std::make_unique<Text>(title_, node_, 8.0f, 17.0f, 13.0f, "left", LVecBase3f(0.15f), true);

    auto btn_close_options = std::make_shared<rppanda::DirectButton::Options>();
    btn_close_options->relief = rppanda::DGG_FLAT;
    btn_close_options->press_effect = true;
    btn_close_options->pos = LVecBase3f(width_ - 22, 1, -12);
    btn_close_options->frame_color = LColorf(1.0f, 0.2f, 0.2f, 0.5f);
    btn_close_options->scale = LVecBase3f(45 / 2.0f, 1.0f, 24 / 2.0f);
    btn_close_options->image = { std::make_shared<rppanda::ImageInput>(std::string("/$$rp/data/gui/close_window.png")) };
    btn_close_ = new rppanda::DirectButton(node_, btn_close_options);

    // Init bindings
    btn_close_->set_transparency(TransparencyAttrib::M_alpha);
    btn_close_->bind(rppanda::DGG_B1CLICK, [this](const Event*) { request_close(); });
    btn_close_->bind(rppanda::DGG_WITHIN, [this](const Event*) { on_close_btn_hover(); });
    btn_close_->bind(rppanda::DGG_WITHOUT, [this](const Event*) { on_close_btn_out(); });
    title_bar_->bind(rppanda::DGG_B1PRESS, [this](const Event*) { start_drag(); });
    title_bar_->bind(rppanda::DGG_B1RELEASE, [this](const Event*) { stop_drag(); });
}

void DraggableWindow::start_drag()
{
    dragging_ = true;
    node_.detach_node();
    node_.reparent_to(parent_);

    Globals::base->get_task_mgr()->add(
        [this](rppanda::FunctionalTask* task) { return on_tick(task); },
        "UIWindowDrag", boost::none, {}, boost::none, boost::none,
        [this](rppanda::FunctionalTask*, bool) { stop_drag(); });

    drag_offset_ = pos_ - get_mouse_pos();
}

void DraggableWindow::on_close_btn_hover()
{
    btn_close_->set_image(std::make_shared<rppanda::ImageInput>(std::string("/$$rp/data/gui/close_window_hover.png")));
}

void DraggableWindow::on_close_btn_out()
{
    btn_close_->set_image(std::make_shared<rppanda::ImageInput>(std::string("/$$rp/data/gui/close_window.png")));
}

void DraggableWindow::request_close()
{
    hide();
}

void DraggableWindow::stop_drag()
{
    Globals::base->get_task_mgr()->remove("UIWindowDrag");
    dragging_ = false;
}

LVecBase2 DraggableWindow::get_mouse_pos() const
{
    float mouse_x = static_cast<float>(Globals::base->get_win()->get_pointer(0).get_x());
    float mouse_y = static_cast<float>(Globals::base->get_win()->get_pointer(0).get_y());
    return LVecBase2(mouse_x, mouse_y) * context_scale_;
}

AsyncTask::DoneStatus DraggableWindow::on_tick(rppanda::FunctionalTask* task)
{
    set_pos(get_mouse_pos() + drag_offset_);
    return AsyncTask::DS_cont;
}

}
