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

#pragma once

#include <asyncTask.h>
#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.hpp>

class GenericAsyncTask;

namespace rppanda {
class DirectFrame;
class DirectButton;
}

namespace rpcore {

class Text;

/** This is a simple draggable but not resizeable window. */
class RENDER_PIPELINE_DECL DraggableWindow : public RPObject
{
public:
    struct RENDER_PIPELINE_DECL Default
    {
        static const int width = 800;
        static const int height = 500;
        static const std::string title;
    };

public:
    DraggableWindow(int width=Default::width, int height=Default::height,
                    const std::string& title=Default::title, NodePath parent={});
    ~DraggableWindow();

    /** Centers the window on screen. */
    void center_on_screen();

    /** Sets the window title. */
    void set_title(const std::string& title);

    /** Shows the window. */
    void show();

    /** Hides the window. */
    void hide();

protected:
    /** Creates the window components. */
    virtual void create_components();

    int width_;
    int height_;
    bool visible_ = true;
    NodePath node_;

private:
    /** Gets called when the user starts dragging the window. */
    static void start_drag(const Event* ev, void* user_data);

    /** Internal method when the close button got hovered. */
    static void on_close_btn_hover(const Event* ev, void* user_data);

    /** Internal method when the close button is no longer hovered. */
    static void on_close_btn_out(const Event* ev, void* user_data);

    /** This method gets called when the close button gets clicked. */
    static void request_close(const Event* ev, void* user_data);

    /** Gets called when the user stops dragging the window. */
    static void stop_drag(const Event* ev, void* user_data);
    static void stop_drag(GenericAsyncTask* ev, bool clean_exit, void* user_data);

    /**
     * Internal helper function to get the mouse position, scaled by
     * the context scale.
     */
    LVecBase2 get_mouse_pos() const;

    /** Task which updates the window while being dragged. */
    static AsyncTask::DoneStatus on_tick(GenericAsyncTask* task, void* user_data);

    void set_pos(const LVecBase2& pos);

    std::string title_;
    NodePath parent_;
    bool dragging_ = false;
    LVecBase2 drag_offset_ = LVecBase2(0);
    LVecBase2 pos_ = LVecBase2(0);

    float context_scale_;
    float context_width_;
    float context_height_;
    LVecBase2 set_pos_;

    std::unique_ptr<Text> window_title_;
    PT(rppanda::DirectFrame) border_frame_ = nullptr;
    PT(rppanda::DirectFrame) background_ = nullptr;
    PT(rppanda::DirectFrame) title_bar_ = nullptr;
    PT(rppanda::DirectButton) btn_close_ = nullptr;
};

// ************************************************************************************************

inline void DraggableWindow::show()
{
    visible_ = true;
    center_on_screen();
    node_.show();
}

inline void DraggableWindow::hide()
{
    visible_ = false;
    stop_drag(nullptr, this);
    node_.hide();
}

inline void DraggableWindow::stop_drag(GenericAsyncTask* ev, bool clean_exit, void* user_data)
{
    stop_drag(nullptr, user_data);
}

/** Moves the window to the specified position. */
inline void DraggableWindow::set_pos(const LVecBase2& pos)
{
    pos_ = pos;
    pos_.set_x((std::max)(pos_.get_x(), -width_ + 100.0f));
    pos_.set_y((std::max)(pos_.get_y(), 25.0f));
    pos_.set_x((std::min)(pos_.get_x(), context_width_ - 100.0f));
    pos_.set_y((std::min)(pos_.get_y(), context_height_ - 50.0f));
    node_.set_pos(pos_.get_x(), 1, -pos_.get_y());
}

}
