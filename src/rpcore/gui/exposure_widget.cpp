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

#include "rpcore/gui/exposure_widget.hpp"

#include <computeNode.h>

#include "render_pipeline/rppanda/gui/direct_frame.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rpcore/loader.hpp"

namespace rpcore {

ExposureWidget::ExposureWidget(RenderPipeline* pipeline, NodePath parent): RPObject("ExposureWidget"), pipeline_(pipeline), parent_(parent)
{
    node_ = parent_.attach_new_node("ExposureWidgetNode");

    create_components();
}

ExposureWidget::~ExposureWidget() = default;

void ExposureWidget::create_components()
{
    node_.hide();

    // Create the texture where the gui component is rendered inside
    storage_tex_ = Image::create_2d("ExposureDisplay", 140, 20, "RGBA8");
    storage_tex_->set_clear_color(LColor(0.2, 0.6, 1.0, 1.0));
    storage_tex_->clear_image();

    auto option = std::make_shared<rppanda::DirectFrame::Options>();
    option->frame_color = LColor(0.1, 0.1, 0.1, 1.0);
    option->frame_size = LVecBase4(200, 0, -10, -85);
    option->pos = LVecBase3(0, 0, 0);
    bg_frame_ = new rppanda::DirectFrame(node_, option);

    display_img_ = std::make_unique<Sprite>(storage_tex_->get_texture(), 140, 20, node_, 20, 50);

    display_txt_ = std::make_unique<Text>("CURRENT EXPOSURE", node_, 160, 40, 13, "right", LVecBase3(0.8f));

    // Create the shader which generates the visualization texture
    _cshader_node = new ComputeNode("ExposureWidget");
    _cshader_node->add_dispatch(140 / 10, 20 / 4, 1);

    cshader_np_ = node_.attach_new_node(_cshader_node);

    // Defer the further loading
    Globals::base->get_task_mgr()->do_method_later(1.0f, std::bind(&ExposureWidget::late_init, this, std::placeholders::_1),
        "ExposureLateInit");
}

AsyncTask::DoneStatus ExposureWidget::late_init(rppanda::FunctionalTask* task)
{
    StageManager* stage_mgr = pipeline_->get_stage_mgr();

    const auto& pipe = stage_mgr->get_pipe("Exposure");
    if (pipe == ShaderInput::get_blank())
    {
        debug("Disabling exposure widget, could not find the exposure data.");
        node_.remove_node();
        return AsyncTask::DS_done;
    }

    node_.show();

    Texture* exposure_tex = pipe.get_texture();
    cshader_ = RPLoader::load_shader({"/$$rp/shader/visualize_exposure.compute.glsl"});
    cshader_np_.set_shader(cshader_);
    cshader_np_.set_shader_input("DestTex", storage_tex_->get_texture());
    cshader_np_.set_shader_input("ExposureTex", exposure_tex);

    return AsyncTask::DS_done;
}

}
