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

#include <render_pipeline/rpcore/gui/draggable_window.hpp>

class GenericAsyncTask;

namespace rppanda {
class DirectScrolledFrame;
}

namespace rpcore {

class RenderPipeline;
class StageManager;

/** Small tool which displays the order of the graphic pipes. */
class PipeViewer: public DraggableWindow
{
public:
    PipeViewer(RenderPipeline* pipeline, NodePath parent);

    void toggle();

private:
    static AsyncTask::DoneStatus update_task(GenericAsyncTask* task, void* user_data);

    /**
     * Reads the pipes and stages from the stage manager and renders those
     * into the window.
     */
    void populate_content();

    /** Internal method to create the window components. */
    virtual void create_components();

    RenderPipeline* _pipeline;
    int _scroll_width = 8000;
    int _scroll_height = 3000;
    bool _created = false;

    PT(rppanda::DirectScrolledFrame) _content_frame;
    NodePath _content_node;
    NodePath _pipe_node;
    NodePath _stage_node;


    NodePath _pipe_descriptions;
};

}
