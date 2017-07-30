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

#include "render_pipeline/rpcore/stages/gbuffer_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/util/shader_input_blocks.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rpcore {

GBufferStage::RequireType GBufferStage::required_inputs = { "DefaultEnvmap" };
GBufferStage::RequireType GBufferStage::required_pipes;

GBufferStage::ProduceType GBufferStage::get_produced_pipes(void) const
{
    return {
        make_gbuffer_ubo(),
        ShaderInput("SceneDepth", target->get_depth_tex()),
    };
}

std::shared_ptr<SimpleInputBlock> GBufferStage::make_gbuffer_ubo(void) const
{
    std::shared_ptr<SimpleInputBlock> ubo(new SimpleInputBlock("GBuffer"));
    ubo->add_input("Depth", target->get_depth_tex());
    ubo->add_input("Data0", target->get_color_tex());
    ubo->add_input("Data1", target->get_aux_tex(0));
    ubo->add_input("Data2", target->get_aux_tex(1));

    return ubo;
}

void GBufferStage::create(void)
{
    const bool stereo_mode = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target = create_target("GBuffer");
    target->add_color_attachment(16, true);
    target->add_depth_attachment(32);
    target->add_aux_attachments(16, 2);
    if (stereo_mode)
        target->set_layers(2);
    target->prepare_render(Globals::base->get_cam());
}

void GBufferStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

std::string GBufferStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
