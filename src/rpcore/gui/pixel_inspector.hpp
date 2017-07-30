/**
 * Render Pipeline C++
 *
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

#include <render_pipeline/rpcore/rpobject.hpp>

class GenericAsyncTask;

namespace rpcore {

class RenderPipeline;

/** Widget to analyze the rendered pixels, by zooming in. */
class PixelInspector: public RPObject
{
public:
    PixelInspector(RenderPipeline* pipeline);

    /** Shows the inspector. */
    void show(void);

    /** Shows the inspector. */
    void hide(void);

    /** Updates the pixel preview. */
    void update(void);

private:
    /** Internal method to init the widgets components. */
    void create_components(void);

    /**
     * Gets called after the pipeline initialized, this extracts the
     * exposure texture from the stage manager.
     */
    static AsyncTask::DoneStatus late_init(GenericAsyncTask* task, void* user_data);

    RenderPipeline* _pipeline;
    NodePath _node;

    NodePath _zoomer;
};

inline void PixelInspector::hide(void)
{
    _node.hide();
}

}
