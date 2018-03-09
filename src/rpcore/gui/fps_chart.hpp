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

namespace rppanda {
class FunctionalTask;
}

namespace rpcore {

class RenderPipeline;
class Image;
class Sprite;
class Text;

/** Widget to show the FPS as a chart . */
class FPSChart : public RPObject
{
public:
    FPSChart(RenderPipeline* pipeline, NodePath parent);
    ~FPSChart();

private:
    /** Internal method to init the widgets components. */
    void create_components();

    /**
     * Gets called after the pipeline initialized, this extracts the
     * exposure texture from the stage manager.
     */
    AsyncTask::DoneStatus late_init(rppanda::FunctionalTask* task);

    /** Updates the widget. */
    AsyncTask::DoneStatus update(rppanda::FunctionalTask* task);

    RenderPipeline* pipeline_;
    NodePath parent_;
    NodePath node_;

    std::shared_ptr<Image> storage_buffer_;
    PTA_int store_index_;
    PTA_float current_ftime_;
    PTA_float chart_ms_max_;
    std::shared_ptr<Image> display_tex_;
    std::unique_ptr<Sprite> display_img_;

    std::unique_ptr<Text> display_txt_;
    std::unique_ptr<Text> display_txt_bottom_;

    PT(ComputeNode) cshader_node_;
    NodePath cshader_np_;
    PT(Shader) cshader_;

    PT(ComputeNode) update_shader_node_;
    NodePath update_shader_np_;
    PT(Shader) ushader_;
};

}
