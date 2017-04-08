#include "scattering_stage.h"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/render_target.h>

namespace rpplugins {

ScatteringStage::RequireType ScatteringStage::required_inputs = { "DefaultSkydome" };
ScatteringStage::RequireType ScatteringStage::required_pipes = { "ShadedScene", "GBuffer" };

ScatteringStage::ScatteringStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ScatteringStage")
{
}

ScatteringStage::ProduceType ScatteringStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", _target->get_color_tex()),
	};
}

void ScatteringStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

	_target = create_target("ApplyScattering");
	_target->add_color_attachment(16, true);
    if (stereo_mode_)
        _target->set_layers(2);
	_target->prepare_buffer();
}

void ScatteringStage::reload_shaders(void)
{
	_target->set_shader(load_plugin_shader({"apply_scattering.frag.glsl"}, stereo_mode_));
}

std::string ScatteringStage::get_plugin_id(void) const
{
	return PLUGIN_ID_STRING;
}

}	// namespace rpplugins
