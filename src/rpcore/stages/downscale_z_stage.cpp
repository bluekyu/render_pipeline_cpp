#include <render_pipeline/rpcore/stages/downscale_z_stage.hpp>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpcore {

DownscaleZStage::RequireType DownscaleZStage::required_inputs;
DownscaleZStage::RequireType DownscaleZStage::required_pipes = { "GBuffer" };

DownscaleZStage::ProduceType DownscaleZStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("DownscaledDepth", target->get_color_tex()),
	};
}

void DownscaleZStage::create(void)
{
	stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

	target = create_target("DownscaleDepth");
	target->add_color_attachment(LVecBase4i(16, 0, 0, 0));
	if (stereo_mode_)
		target->set_layers(2);
	target->prepare_buffer();
}

void DownscaleZStage::reload_shaders(void)
{
	target->set_shader(load_shader({"downscale_depth.frag.glsl"}, stereo_mode_));
}

std::string DownscaleZStage::get_plugin_id(void) const
{
	return std::string("render_pipeline_internal");
}

}
