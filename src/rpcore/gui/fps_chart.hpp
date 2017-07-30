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

#include <render_pipeline/rpcore/rpobject.hpp>

class ComputeNode;
class GenericAsyncTask;

namespace rpcore {

class RenderPipeline;
class Image;
class Sprite;
class Text;

/** Widget to show the FPS as a chart . */
class FPSChart: public RPObject
{
public:
    FPSChart(RenderPipeline* pipeline, NodePath parent);
    ~FPSChart(void);

private:
    /** Internal method to init the widgets components. */
    void create_components(void);

    /**
     * Gets called after the pipeline initialized, this extracts the
     * exposure texture from the stage manager.
     */
    static AsyncTask::DoneStatus late_init(GenericAsyncTask* task, void* user_data);

    /** Updates the widget. */
    static AsyncTask::DoneStatus update(GenericAsyncTask* task, void* user_data);

    RenderPipeline* _pipeline;
    NodePath _parent;
    NodePath _node;

    std::shared_ptr<Image> _storage_buffer = nullptr;
    PTA_int _store_index;
    PTA_float _current_ftime;
    PTA_float _chart_ms_max;
    std::shared_ptr<Image> _display_tex = nullptr;
    Sprite* _display_img = nullptr;

    Text* _display_txt = nullptr;
    Text* _display_txt_bottom = nullptr;

    PT(ComputeNode) _cshader_node = nullptr;
    NodePath _cshader_np;
    PT(Shader) _cshader;

    PT(ComputeNode) _update_shader_node = nullptr;
    NodePath _update_shader_np;
    PT(Shader) _ushader;
};

}
