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
#include <asyncTask.h>

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
class RenderModeSelector;

class RENDER_PIPELINE_DECL Debugger : public RPObject
{
public:
    Debugger(RenderPipeline* pipeline);
    ~Debugger();

    bool is_advanced_info_used() const;

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
    bool is_gui_hidden() const;
    void show_gui(bool show);

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
    std::unique_ptr<SceneGraphAnalyzer> analyzer_;

    std::unique_ptr<BufferViewer> buffer_viewer_;
    std::unique_ptr<PipeViewer> pipe_viewer_;
    std::unique_ptr<RenderModeSelector> rm_selector_;
    std::unique_ptr<ErrorMessageDisplay> error_msg_handler_;
    std::unique_ptr<Sprite> pipeline_logo_;
    std::unique_ptr<Sprite> hint_reloading_;
    std::unique_ptr<Sprite> keybinding_instructions_;
    std::unique_ptr<rpcore::TextNode> keybinding_text_;
    std::unique_ptr<ExposureWidget> exposure_widget_;
    std::unique_ptr<FPSChart> fps_widget_;
    std::unique_ptr<PixelInspector> pixel_widget_;

    int debugger_width;
    bool debugger_visible;
    float gui_scale;

    std::vector<std::unique_ptr<rpcore::TextNode>> debug_lines_;
};

inline bool Debugger::is_gui_hidden() const
{
    return fullscreen_node.is_hidden();
}

inline ErrorMessageDisplay* Debugger::get_error_msg_handler() const
{
    return error_msg_handler_.get();
}

}
