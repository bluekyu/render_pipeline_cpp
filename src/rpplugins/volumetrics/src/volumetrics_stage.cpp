#include "volumetrics_stage.h"

#include <render_pipeline/rpcore/render_target.h>

namespace rpplugins {

VolumetricsStage::RequireType VolumetricsStage::required_inputs;
VolumetricsStage::RequireType VolumetricsStage::required_pipes = { "ShadedScene", "GBuffer" };

VolumetricsStage::ProduceType VolumetricsStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", _target_combine->get_color_tex()),
	};
}

void VolumetricsStage::create(void)
{
	if (_enable_volumetric_shadows)
	{
		_target = create_target("ComputeVolumetrics");
		_target->set_size(-2);
		_target->add_color_attachment(16, true);
		_target->prepare_buffer();

		_target_upscale = create_target("Upscale");
		_target_upscale->add_color_attachment(16, true);
		_target_upscale->prepare_buffer();

		_target_upscale->set_shader_input(ShaderInput("SourceTex", _target->get_color_tex()));
		_target_upscale->set_shader_input(ShaderInput("upscaleWeights", LVecBase2f(0.001, 0.001)));
	}

	_target_combine = create_target("CombineVolumetrics");
	_target_combine->add_color_attachment(16);
	_target_combine->prepare_buffer();

	if (_enable_volumetric_shadows)
		_target_combine->set_shader_input(ShaderInput("VolumetricsTex", _target_upscale->get_color_tex()));
}

void VolumetricsStage::reload_shaders(void)
{
	if (_enable_volumetric_shadows)
	{
		_target->set_shader(load_plugin_shader({"compute_volumetric_shadows.frag.glsl"}));
		_target_upscale->set_shader(load_plugin_shader({"/$$rp/shader/bilateral_upscale.frag.glsl"}));
	}

	_target_combine->set_shader(load_plugin_shader({"apply_volumetrics.frag.glsl"}));
}

std::string VolumetricsStage::get_plugin_id(void) const
{
	return PLUGIN_ID_STRING;
}

}	// namespace rpplugins
