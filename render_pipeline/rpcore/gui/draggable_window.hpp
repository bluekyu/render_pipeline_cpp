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

#include <render_pipeline/rpcore/gui/text.hpp>

class GenericAsyncTask;

namespace rppanda {
class DirectFrame;
class DirectButton;
}

namespace rpcore {

/** This is a simple draggable but not resizeable window. */
class DraggableWindow : public RPObject
{
public:
    DraggableWindow(int width=800, int height=500, const std::string& title="Window", NodePath parent={});
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

    int _width;
    int _height;
    bool _visible = true;
    NodePath _node;

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

    void set_pos(const LVecBase2f& pos);

    std::string _title;
    NodePath _parent;
    bool _dragging = false;
    LVecBase2f _drag_offset = LVecBase2f(0);
    LVecBase2f _pos = LVecBase2f(0);

    float _context_scale;
    float _context_width;
    float _context_height;
    LVecBase2f _set_pos;

    Text* _window_title = nullptr;
    PT(rppanda::DirectFrame) _border_frame = nullptr;
    PT(rppanda::DirectFrame) _background = nullptr;
    PT(rppanda::DirectFrame) _title_bar = nullptr;
    PT(rppanda::DirectButton) _btn_close = nullptr;
};

// ************************************************************************************************

inline void DraggableWindow::show()
{
    _visible = true;
    center_on_screen();
    _node.show();
}

inline void DraggableWindow::hide()
{
    _visible = false;
    stop_drag(nullptr, this);
    _node.hide();
}

inline void DraggableWindow::stop_drag(GenericAsyncTask* ev, bool clean_exit, void* user_data)
{
    stop_drag(nullptr, user_data);
}

/** Moves the window to the specified position. */
inline void DraggableWindow::set_pos(const LVecBase2f& pos)
{
    _pos = pos;
    _pos.set_x((std::max)(_pos.get_x(), -_width + 100.0f));
    _pos.set_y((std::max)(_pos.get_y(), 25.0f));
    _pos.set_x((std::min)(_pos.get_x(), _context_width - 100.0f));
    _pos.set_y((std::min)(_pos.get_y(), _context_height - 50.0f));
    _node.set_pos(_pos.get_x(), 1, -_pos.get_y());
}

}
