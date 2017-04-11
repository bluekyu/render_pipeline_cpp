#include "apply_envprobes_stage.h"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/stages/ambient_stage.h>

namespace rpplugins {

ApplyEnvprobesStage::RequireType ApplyEnvprobesStage::required_inputs = { "EnvProbes" };
ApplyEnvprobesStage::RequireType ApplyEnvprobesStage::required_pipes = { "GBuffer", "PerCellProbes", "CellIndices" };

ApplyEnvprobesStage::ApplyEnvprobesStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ApplyEnvprobesStage")
{
}

ApplyEnvprobesStage::ProduceType ApplyEnvprobesStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("EnvmapAmbientSpec", _target->get_color_tex()),
		ShaderInput("EnvmapAmbientDiff", _target->get_aux_tex(0)),
	};
}

void ApplyEnvprobesStage::create(void)
{
	stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

	_target = create_target("ApplyEnvmap");
	_target->add_color_attachment(16, true);
	_target->add_aux_attachment(16);
	if (stereo_mode_)
		_target->set_layers(2);
	_target->prepare_buffer();
	rpcore::AmbientStage::get_global_required_pipes().push_back("EnvmapAmbientSpec");
	rpcore::AmbientStage::get_global_required_pipes().push_back("EnvmapAmbientDiff");
}

void ApplyEnvprobesStage::reload_shaders(void)
{
	_target->set_shader(load_plugin_shader({"apply_envprobes.frag.glsl"}, stereo_mode_));
}

std::string ApplyEnvprobesStage::get_plugin_id(void) const
{
	return RPCPP_PLUGIN_ID_STRING;
}

}	// namespace rpplugins
