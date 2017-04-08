#include <render_pipeline/rpcore/stages/shadow_stage.h>

#include <graphicsBuffer.h>

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/render_target.h>

namespace rpcore {

ShadowStage::RequireType ShadowStage::required_inputs_;
ShadowStage::RequireType ShadowStage::required_pipes_;

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

}	// namespace rpcore
