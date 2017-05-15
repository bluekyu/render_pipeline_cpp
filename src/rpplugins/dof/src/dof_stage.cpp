#include "dof_stage.h"

#include <render_pipeline/rpcore/render_target.h>

namespace rpplugins {

DoFStage::RequireType DoFStage::required_inputs;
DoFStage::RequireType DoFStage::required_pipes = { "ShadedScene", "GBuffer", "DownscaledDepth" };

DoFStage::ProduceType DoFStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", target_merge_->get_color_tex()),
	};
}

void DoFStage::create(void)
{
    target_prefilter_ = create_target("PrefilterDoF");
    //target_prefilter_->set_size("50%");
    target_prefilter_->add_color_attachment(16, true);
    target_prefilter_->prepare_buffer();

    const int tile_size = 32;
    tile_target_ = create_target("FetchVertDOF");
    tile_target_->set_size(-1, -tile_size);
    tile_target_->add_color_attachment(LVecBase3i(16, 16, 0));
    tile_target_->prepare_buffer();
    tile_target_->set_shader_input(ShaderInput("PrecomputedCoC", target_prefilter_->get_color_tex()));

    tile_target_horiz_ = create_target("FetchHorizDOF");
    tile_target_horiz_->set_size(-tile_size);
    tile_target_horiz_->add_color_attachment(LVecBase3i(16, 16, 0));
    tile_target_horiz_->prepare_buffer();
    tile_target_horiz_->set_shader_input(ShaderInput("SourceTex", tile_target_->get_color_tex()));

    minmax_target_ = create_target("DoFNeighborMinMax");
    minmax_target_->set_size(-tile_size);
    minmax_target_->add_color_attachment(LVecBase3i(16, 16, 0));
    minmax_target_->prepare_buffer();
    minmax_target_->set_shader_input(ShaderInput("TileMinMax", tile_target_horiz_->get_color_tex()));

    presort_target_ = create_target("DoFPresort");
    presort_target_->add_color_attachment(LVecBase3i(11, 11, 10));
    presort_target_->prepare_buffer();
    presort_target_->set_shader_input(ShaderInput("TileMinMax", minmax_target_->get_color_tex()));
    presort_target_->set_shader_input(ShaderInput("PrecomputedCoC", target_prefilter_->get_color_tex()));

    target_ = create_target("ComputeDoF");
    // target_->set_size("50%");
    target_->add_color_attachment(16, true);
    target_->prepare_buffer();
    target_->set_shader_input(ShaderInput("PresortResult", presort_target_->get_color_tex()));
    target_->set_shader_input(ShaderInput("PrecomputedCoC", target_prefilter_->get_color_tex()));
    target_->set_shader_input(ShaderInput("TileMinMax", minmax_target_->get_color_tex()));

    target_merge_ = create_target("MergeDoF");
    target_merge_->add_color_attachment(16);
    target_merge_->prepare_buffer();
    target_merge_->set_shader_input(ShaderInput("SourceTex", target_->get_color_tex()));

    // self.target_upscale.set_shader_input("SourceTex", self.target.color_tex)
    // self.target_upscale.set_shader_input("upscaleWeights", Vec2(0.001, 0.001))
}

void DoFStage::reload_shaders(void)
{
    tile_target_->set_shader(load_plugin_shader({ "fetch_dof_minmax.frag.glsl" }));
    tile_target_horiz_->set_shader(load_plugin_shader({ "fetch_dof_minmax_horiz.frag.glsl" }));
    minmax_target_->set_shader(load_plugin_shader({ "fetch_dof_tile_neighbors.frag.glsl" }));
    presort_target_->set_shader(load_plugin_shader({ "dof_presort.frag.glsl" }));
    target_prefilter_->set_shader(load_plugin_shader({ "prefilter_dof.frag.glsl" }));
    target_->set_shader(load_plugin_shader({ "compute_dof.frag.glsl" }));
    target_merge_->set_shader(load_plugin_shader({ "merge_dof.frag.glsl" }));
    // self.target_upscale.shader = self.load_plugin_shader(
    //     "/$$rp/shader/bilateral_upscale.frag.glsl")
}

std::string DoFStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
