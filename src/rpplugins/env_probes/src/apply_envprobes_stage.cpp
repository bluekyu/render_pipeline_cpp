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

#include "apply_envprobes_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/stages/ambient_stage.hpp>

namespace rpplugins {

ApplyEnvprobesStage::RequireType ApplyEnvprobesStage::required_inputs = { "EnvProbes" };
ApplyEnvprobesStage::RequireType ApplyEnvprobesStage::required_pipes = { "GBuffer", "PerCellProbes", "CellIndices" };

ApplyEnvprobesStage::ApplyEnvprobesStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ApplyEnvprobesStage")
{
}

ApplyEnvprobesStage::ProduceType ApplyEnvprobesStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("EnvmapAmbientSpec", _target->get_color_tex()),
        ShaderInput("EnvmapAmbientDiff", _target->get_aux_tex(0)),
    };
}

void ApplyEnvprobesStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    _target = create_target("ApplyEnvmap");
    _target->add_color_attachment(16, true);
    _target->add_aux_attachment(16);
    if (stereo_mode_)
        _target->set_layers(2);
    _target->prepare_buffer();
    rpcore::AmbientStage::get_global_required_pipes().push_back("EnvmapAmbientSpec");
    rpcore::AmbientStage::get_global_required_pipes().push_back("EnvmapAmbientDiff");
}

void ApplyEnvprobesStage::reload_shaders(void)
{
    _target->set_shader(load_plugin_shader({"apply_envprobes.frag.glsl"}, stereo_mode_));
}

std::string ApplyEnvprobesStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
