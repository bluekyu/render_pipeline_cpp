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

#include "render_pipeline/rpcore/stages/downscale_z_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"

namespace rpcore {

DownscaleZStage::RequireType DownscaleZStage::required_inputs;
DownscaleZStage::RequireType DownscaleZStage::required_pipes = { "GBuffer" };

DownscaleZStage::ProduceType DownscaleZStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("DownscaledDepth", target->get_color_tex()),
    };
}

void DownscaleZStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target = create_target("DownscaleDepth");
    target->add_color_attachment(LVecBase4i(16, 0, 0, 0));
    if (stereo_mode_)
        target->set_layers(2);
    target->prepare_buffer();
}

void DownscaleZStage::reload_shaders(void)
{
    target->set_shader(load_shader({ "downscale_depth.frag.glsl" }, stereo_mode_));
}

std::string DownscaleZStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
