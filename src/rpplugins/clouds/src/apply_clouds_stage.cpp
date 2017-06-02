#include "apply_clouds_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

ApplyCloudsStage::RequireType ApplyCloudsStage::required_inputs;
ApplyCloudsStage::RequireType ApplyCloudsStage::required_pipes = { "ShadedScene", "GBuffer" };

ApplyCloudsStage::ProduceType ApplyCloudsStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", _target_apply_clouds->get_color_tex()),
	};
}

void ApplyCloudsStage::create(void)
{
    _render_target = create_target("RaymarchVoxels");
    _render_target->set_size(-2);
    _render_target->add_color_attachment(16, true);
    _render_target->prepare_buffer();

    _upscale_target = create_target("UpscaleTarget");
    _upscale_target->add_color_attachment(16, true);
    _upscale_target->prepare_buffer();
    _upscale_target->set_shader_input(ShaderInput("upscaleWeights", LVecBase2f(0.05, 0.2)));
    _upscale_target->set_shader_input(ShaderInput("SourceTex", _render_target->get_color_tex()));


    _target_apply_clouds = create_target("MergeWithScene");
    _target_apply_clouds->add_color_attachment(16);
    _target_apply_clouds->prepare_buffer();
    _target_apply_clouds->set_shader_input(ShaderInput("CloudsTex", _upscale_target->get_color_tex()));
}

void ApplyCloudsStage::reload_shaders(void)
{
	_target_apply_clouds->set_shader(load_plugin_shader({ "apply_clouds.frag.glsl" }));
    _render_target->set_shader(load_plugin_shader({ "render_clouds.frag.glsl" }));
    _upscale_target->set_shader(load_plugin_shader({ "/$$rp/shader/bilateral_upscale.frag.glsl" }));
}

std::string ApplyCloudsStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
