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

#include "render_pipeline/rpcore/stages/shadow_stage.hpp"

#include <graphicsBuffer.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/render_target.hpp"

namespace rpcore {

ShadowStage::RequireType ShadowStage::required_inputs_;
ShadowStage::RequireType ShadowStage::required_pipes_;

ShadowStage::ShadowStage(RenderPipeline& pipeline): RenderStage(pipeline, "ShadowStage")
{
    size_ = 4096;
}

ShadowStage::ProduceType ShadowStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadowAtlas", target_->get_depth_tex()),
        ShaderInput("ShadowAtlasPCF", target_->get_depth_tex(), make_pcf_state()),
    };
}

SamplerState ShadowStage::make_pcf_state(void) const
{
    SamplerState state;
    state.set_minfilter(SamplerState::FT_shadow);
    state.set_magfilter(SamplerState::FT_shadow);
    state.set_wrap_u(SamplerState::WM_clamp);
    state.set_wrap_v(SamplerState::WM_clamp);
    state.set_wrap_w(SamplerState::WM_clamp);
    return state;
}

GraphicsOutput* ShadowStage::get_atlas_buffer(void) const
{
    return target_->get_internal_buffer();
}

void ShadowStage::create(void)
{
    target_ = create_target("ShadowAtlas");
    target_->set_size(size_);
    target_->add_depth_attachment(16);
    target_->prepare_render(NodePath());

    // Remove all current display regions
    target_->get_internal_buffer()->remove_all_display_regions();
    target_->get_internal_buffer()->get_display_region(0)->set_active(false);

    // Disable the target, and also disable depth clear
    target_->set_active(false);
    target_->get_internal_buffer()->set_clear_depth_active(false);
    target_->get_display_region()->set_clear_depth_active(false);
}

void ShadowStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

std::string ShadowStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
