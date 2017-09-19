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

#include <nodePath.h>
#include <AsyncTask.h>

#include <vector>

#include <render_pipeline/rpcore/rpobject.hpp>

class SceneGraphAnalyzer;

namespace rppanda {
class FunctionalTask;
}

namespace rpcore {

class TextNode;
class RenderPipeline;
class Sprite;
class BufferViewer;
class PipeViewer;
class ErrorMessageDisplay;
class ExposureWidget;
class FPSChart;
class PixelInspector;

class Debugger : public RPObject
{
public:
    Debugger(RenderPipeline* pipeline);
    ~Debugger();

    bool use_advanced_info() const;

    /** Creates the gui components. */
    void create_components();

    /** Updates the gui. */
    void update();

    /** Analyzes the scene graph to provide useful information. */
    AsyncTask::DoneStatus collect_scene_data(rppanda::FunctionalTask* task);

    /** Creates the stats overlay. */
    void create_stats();

    /** Creates the hints like keybindings and when reloading shaders. */
    void create_hints();

    /** Sets whether the shader reload hint is visible. */
    void set_reload_hint_visible(bool flag);

    /** Handles the window resize, repositions the GUI elements to fit on screen. */
    void handle_window_resize();

    /** Inits the debugger keybindings. */
    void init_keybindings();

    /** Shows / Hides the gui. */
    void toggle_gui_visible();

    /** Shows / Hides the keybindings gui. */
    void toggle_keybindings_visible();

    /** Updates the stats overlay. */
    AsyncTask::DoneStatus update_stats(rppanda::FunctionalTask* task);

    ErrorMessageDisplay* get_error_msg_handler() const;

private:
    RenderPipeline* pipeline;
    NodePath fullscreen_node;
    NodePath overlay_node;
    NodePath exposure_node;
    NodePath fps_node;
    SceneGraphAnalyzer* _analyzer = nullptr;

    BufferViewer* _buffer_viewer = nullptr;
    PipeViewer* _pipe_viewer = nullptr;
    ErrorMessageDisplay* _error_msg_handler = nullptr;
    Sprite* _pipeline_logo = nullptr;
    Sprite* _hint_reloading = nullptr;
    Sprite* keybinding_instructions_ = nullptr;
    rpcore::TextNode* keybinding_text_ = nullptr;
    ExposureWidget* _exposure_widget = nullptr;
    FPSChart* _fps_widget = nullptr;
    PixelInspector* _pixel_widget = nullptr;

    int debugger_width;
    bool debugger_visible;
    float gui_scale;

    std::vector<rpcore::TextNode*> debug_lines;
};

inline ErrorMessageDisplay* Debugger::get_error_msg_handler() const
{
    return _error_msg_handler;
}

}
