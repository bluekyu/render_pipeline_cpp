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

#include "smaa_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

SMAAStage::RequireType SMAAStage::required_inputs;
SMAAStage::RequireType SMAAStage::required_pipes = { "ShadedScene", "GBuffer", "CombinedVelocity" };

SMAAStage::ProduceType SMAAStage::get_produced_pipes(void) const
{
    if (use_reprojection_)
    {
        return{
            ShaderInput("ShadedScene", resolve_target_->get_color_tex()),
            ShaderInput("SMAAPostResolve", resolve_target_->get_color_tex()),
        };
    }
    else
    {
        return{
            ShaderInput("ShadedScene", neighbor_target_->get_color_tex()),
        };
    }
}

SMAAStage::SMAAStage(rpcore::RenderPipeline& pipeline, bool use_reprojection): RenderStage(pipeline, "SMAAStage"), use_reprojection_(use_reprojection)
{
    if (use_reprojection_)
    {
        if (std::find(required_pipes.begin(), required_pipes.end(), "PreviousFrame::SMAAPostResolve") == required_pipes.end())
            required_pipes.push_back("PreviousFrame::SMAAPostResolve");
    }
    jitter_index_ = PTA_int::empty_array(1);
}

void SMAAStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    // Edge detection
    edge_target_ = create_target("EdgeDetection");
    edge_target_->add_color_attachment();
    if (stereo_mode_)
        edge_target_->set_layers(2);
    edge_target_->prepare_buffer();
    edge_target_->set_clear_color(LColor(0, 0, 0, 0));

    // Weight blending
    blend_target_ = create_target("BlendWeights");
    blend_target_->add_color_attachment(8, true);
    if (stereo_mode_)
        blend_target_->set_layers(2);
    blend_target_->prepare_buffer();

    blend_target_->set_shader_input(ShaderInput("EdgeTex", edge_target_->get_color_tex()));
    blend_target_->set_shader_input(ShaderInput("AreaTex", area_tex_));
    blend_target_->set_shader_input(ShaderInput("SearchTex", search_tex_));
    blend_target_->set_shader_input(ShaderInput("jitterIndex", jitter_index_));

    // Neighbor blending
    neighbor_target_ = create_target("NeighborBlending");
    neighbor_target_->add_color_attachment(16);
    if (stereo_mode_)
        neighbor_target_->set_layers(2);
    neighbor_target_->prepare_buffer();
    neighbor_target_->set_shader_input(ShaderInput("BlendTex", blend_target_->get_color_tex()));

    // Resolving
    if (use_reprojection_)
    {
        resolve_target_ = create_target("Resolve");
        resolve_target_->add_color_attachment(16);
        if (stereo_mode_)
            resolve_target_->set_layers(2);
        resolve_target_->prepare_buffer();
        resolve_target_->set_shader_input(ShaderInput("jitterIndex", jitter_index_));

        // Set initial textures
        resolve_target_->set_shader_input(ShaderInput("CurrentTex", neighbor_target_->get_color_tex()));
    }
}

void SMAAStage::reload_shaders(void)
{
    edge_target_->set_shader(load_plugin_shader({ "edge_detection.frag.glsl" }, stereo_mode_));
    blend_target_->set_shader(load_plugin_shader({ "blending_weights.frag.glsl" }, stereo_mode_));
    neighbor_target_->set_shader(load_plugin_shader({ "neighborhood_blending.frag.glsl" }, stereo_mode_));
    if (use_reprojection_)
        resolve_target_->set_shader(load_plugin_shader({ "resolve_smaa.frag.glsl" }, stereo_mode_));
}

std::string SMAAStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
