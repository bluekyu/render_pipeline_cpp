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

#include "render_pipeline/rpcore/stages/upscale_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/globals.hpp"

namespace rpcore {

UpscaleStage::RequireType UpscaleStage::required_inputs;
UpscaleStage::RequireType UpscaleStage::required_pipes = { "ShadedScene" };

UpscaleStage::ProduceType UpscaleStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", target_->get_color_tex()),
    };
}

void UpscaleStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("Upscale");
    target_->set_size(Globals::native_resolution);
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();

    set_cropping(pipeline_.get_setting<bool>("pipeline.screen_cropping", false));
}

void UpscaleStage::set_dimensions(void)
{
    target_->set_size(Globals::native_resolution);
}

void UpscaleStage::reload_shaders(void)
{
    target_->set_shader(load_shader({"upscale_stage.frag.glsl"}, stereo_mode_));
}

void UpscaleStage::set_cropping(bool enable)
{
    target_->set_shader_input(ShaderInput("use_cropping", LVecBase4i(enable ? 1 : 0)));
}

std::string UpscaleStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
