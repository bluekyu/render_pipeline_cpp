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

#include "godray_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

GodrayStage::RequireType GodrayStage::required_inputs;
GodrayStage::RequireType GodrayStage::required_pipes = { "ShadedScene", "GBuffer" };

GodrayStage::GodrayStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "GodrayStage")
{
}

GodrayStage::ProduceType GodrayStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", _target->get_color_tex()),
    };
}

void GodrayStage::create(void)
{
    _target = create_target("ComputeGodrays");
    _target->add_color_attachment(16);
    _target->prepare_buffer();
}

void GodrayStage::reload_shaders(void)
{
    _target->set_shader(load_plugin_shader({"compute_godrays.frag.glsl"}));
}

std::string GodrayStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
