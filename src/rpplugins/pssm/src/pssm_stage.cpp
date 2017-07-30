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

#include "pssm_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

PSSMStage::RequireType PSSMStage::required_inputs;
PSSMStage::RequireType PSSMStage::required_pipes = { "ShadedScene", "PSSMShadowAtlas", "GBuffer", "PSSMShadowAtlasPCF" };

PSSMStage::ProduceType PSSMStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", _target->get_color_tex()),
    };
}

void PSSMStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    _enabled = true;
    _target_shadows = create_target("FilterPSSM");
    _target_shadows->add_color_attachment(LVecBase4i(8, 0, 0, 0));
    if (stereo_mode_)
        _target_shadows->set_layers(2);
    _target_shadows->prepare_buffer();
    _target_shadows->get_color_tex()->set_clear_color(LColorf(0));

    _target = create_target("ApplyPSSMShadows");
    _target->add_color_attachment(LVecBase4i(16));
    if (stereo_mode_)
        _target->set_layers(2);
    _target->prepare_buffer();

    _target->set_shader_input(ShaderInput("PrefilteredShadows", _target_shadows->get_color_tex()));
}

void PSSMStage::set_render_shadows(bool enabled)
{
    _target_shadows->set_active(enabled);
    if (enabled != _enabled)
    {
        _target_shadows->get_color_tex()->clear_image();
        _enabled = enabled;
    }
}

void PSSMStage::reload_shaders(void)
{
    _target_shadows->set_shader(load_plugin_shader({"filter_pssm_shadows.frag.glsl"}, stereo_mode_));
    _target->set_shader(load_plugin_shader({"apply_sun_shading.frag.glsl"}, stereo_mode_));
}

std::string PSSMStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
