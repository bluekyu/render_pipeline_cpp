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

AOStage::RequireType AOStage::required_inputs;
AOStage::RequireType AOStage::required_pipes = {
    "GBuffer", "DownscaledDepth", "PreviousFrame::AmbientOcclusion",
    "CombinedVelocity", "PreviousFrame::SceneDepth"};

AOStage::ProduceType AOStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("AmbientOcclusion", target_resolve_->get_color_tex()),
    };
}

void AOStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("Sample");
    target_->set_size(-2);
    target_->add_color_attachment(LVecBase4i(8, 0, 0, 0));
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();

    target_upscale_ = create_target("Upscale");
    target_upscale_->add_color_attachment(LVecBase4i(8, 0, 0, 0));
    if (stereo_mode_)
        target_upscale_->set_layers(2);
    target_upscale_->prepare_buffer();

    target_upscale_->set_shader_input(ShaderInput("SourceTex", target_->get_color_tex()));
    target_upscale_->set_shader_input(ShaderInput("upscaleWeights", LVecBase2f(0.001f, 0.001f)));

    target_detail_ao_ = create_target("DetailAO");
    target_detail_ao_->add_color_attachment(LVecBase4i(8, 0, 0, 0));
    if (stereo_mode_)
        target_detail_ao_->set_layers(2);
    target_detail_ao_->prepare_buffer();
    target_detail_ao_->set_shader_input(ShaderInput("AOResult", target_upscale_->get_color_tex()));

    debug(std::string("Blur quality is ") + quality_);

    // Low
    float pixel_stretch = 2.0;
    int blur_passes = 1;

    if (quality_ == "MEDIUM")
    {
        pixel_stretch = 1.0;
        blur_passes = 2;
    }
    else if (quality_ == "HIGH")
    {
        pixel_stretch = 1.0;
        blur_passes = 3;
    }
    else if (quality_ == "ULTRA")
    {
        pixel_stretch = 1.0;
        blur_passes = 5;
    }

    blur_targets_.clear();

    Texture* current_tex = target_detail_ao_->get_color_tex();

    for (int i = 0; i < blur_passes; ++i)
    {
        auto target_blur_v = create_target(std::string("BlurV-") + std::to_string(i));
        target_blur_v->add_color_attachment(LVecBase4i(8, 0, 0, 0));
        if (stereo_mode_)
            target_blur_v->set_layers(2);
        target_blur_v->prepare_buffer();

        auto target_blur_h = create_target(std::string("BlurH-") + std::to_string(i));
        target_blur_h->add_color_attachment(LVecBase4i(8, 0, 0, 0));
        if (stereo_mode_)
            target_blur_h->set_layers(2);
        target_blur_h->prepare_buffer();

        target_blur_v->set_shader_input(ShaderInput("SourceTex", current_tex));
        target_blur_h->set_shader_input(ShaderInput("SourceTex", target_blur_v->get_color_tex()));

        target_blur_v->set_shader_input(ShaderInput("blur_direction", LVecBase2i(0, 1)));
        target_blur_h->set_shader_input(ShaderInput("blur_direction", LVecBase2i(1, 0)));

        target_blur_v->set_shader_input(ShaderInput("pixel_stretch", LVecBase4f(pixel_stretch, 0, 0, 0)));
        target_blur_h->set_shader_input(ShaderInput("pixel_stretch", LVecBase4f(pixel_stretch, 0, 0, 0)));

        current_tex = target_blur_h->get_color_tex();
        blur_targets_.push_back(target_blur_v);
        blur_targets_.push_back(target_blur_h);
    }

    target_resolve_ = create_target("ResolveAO");
    target_resolve_->add_color_attachment(LVecBase4i(8, 0, 0, 0));
    if (stereo_mode_)
        target_resolve_->set_layers(2);
    target_resolve_->prepare_buffer();
    target_resolve_->set_shader_input(ShaderInput("CurrentTex", current_tex));
}

void AOStage::reload_shaders(void)
{
    target_->set_shader(load_plugin_shader({"ao_sample.frag.glsl"}, stereo_mode_));

    if (stereo_mode_)
        target_upscale_->set_shader(load_plugin_shader({"/$$rp/shader/bilateral_upscale_stereo.frag.glsl"}, stereo_mode_));
    else
        target_upscale_->set_shader(load_plugin_shader({"/$$rp/shader/bilateral_upscale.frag.glsl"}));
    
    PT(Shader) blur_shader;
    if (stereo_mode_)
        blur_shader = load_plugin_shader({"/$$rp/shader/bilateral_blur_stereo.frag.glsl"}, stereo_mode_);
    else
        blur_shader = load_plugin_shader({"/$$rp/shader/bilateral_blur.frag.glsl"});
    for (auto& target: blur_targets_)
        target->set_shader(blur_shader);
    target_detail_ao_->set_shader(load_plugin_shader({"small_scale_ao.frag.glsl"}, stereo_mode_));
    target_resolve_->set_shader(load_plugin_shader({"resolve_ao.frag.glsl"}, stereo_mode_));
}

std::string AOStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
