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
#include <genericAsyncTask.h>

#include <vector>

#include <render_pipeline/rpcore/rpobject.hpp>

class SceneGraphAnalyzer;

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

class Debugger: public RPObject
{
public:
    Debugger(RenderPipeline* pipeline);
    ~Debugger(void);

    bool use_advanced_info(void) const;

    /** Creates the gui components. */
    void create_components(void);

    /** Updates the gui. */
    void update(void);

    /** Analyzes the scene graph to provide useful information. */
    AsyncTask::DoneStatus collect_scene_data(GenericAsyncTask* task, void* user_data);

    /** Creates the stats overlay. */
    void create_stats(void);

    /** Creates the hints like keybindings and when reloading shaders. */
    void create_hints(void);

    /** Sets whether the shader reload hint is visible. */
    void set_reload_hint_visible(bool flag);

    /** Handles the window resize, repositions the GUI elements to fit on screen. */
    void handle_window_resize(void);

    /** Inits the debugger keybindings. */
    void init_keybindings(void);

    /** Shows / Hides the gui. */
    void toggle_gui_visible(void);

    /** Shows / Hides the keybindings gui. */
    void toggle_keybindings_visible(void);

    /** Updates the stats overlay. */
    static AsyncTask::DoneStatus update_stats(GenericAsyncTask* task, void* user_data);

    ErrorMessageDisplay* get_error_msg_handler(void) const;

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

inline ErrorMessageDisplay* Debugger::get_error_msg_handler(void) const
{
    return _error_msg_handler;
}

}
