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

#include "ao_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

SkyAOStage::RequireType SkyAOStage::required_inputs = { "SkyAOCapturePosition" };
SkyAOStage::RequireType SkyAOStage::required_pipes = { "SkyAOHeight", "GBuffer" };

SkyAOStage::ProduceType SkyAOStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("SkyAO", target_upscale_->get_color_tex()),
    };
}

void SkyAOStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("ComputeSkyAO");
    target_->set_size(-2);
    target_->add_color_attachment(LVecBase4i(16, 0, 0, 0));
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();

    target_upscale_ = create_target("UpscaleSkyAO");
    target_upscale_->add_color_attachment(LVecBase4i(16, 0, 0, 0));
    if (stereo_mode_)
        target_upscale_->set_layers(2);
    target_upscale_->prepare_buffer();

    target_upscale_->set_shader_input(ShaderInput("SourceTex", target_->get_color_tex()));
    target_upscale_->set_shader_input(ShaderInput("upscaleWeights", LVecBase2f(0.001f, 0.001f)));
}

void SkyAOStage::reload_shaders(void)
{
    target_->set_shader(load_plugin_shader({"compute_sky_ao.frag.glsl"}, stereo_mode_));

    if (stereo_mode_)
        target_upscale_->set_shader(load_plugin_shader({ "/$$rp/shader/bilateral_upscale_stereo.frag.glsl" }, stereo_mode_));
    else
        target_upscale_->set_shader(load_plugin_shader({ "/$$rp/shader/bilateral_upscale.frag.glsl" }));
}

std::string SkyAOStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
