#include "smaa_stage.h"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/globals.h>

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
	return PLUGIN_ID_STRING;
}

}	// namespace rpplugins
