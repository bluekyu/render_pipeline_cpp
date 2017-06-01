#include "tonemapping_stage.h"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

TonemappingStage::RequireType TonemappingStage::required_inputs;
TonemappingStage::RequireType TonemappingStage::required_pipes = { "ShadedScene" };

TonemappingStage::ProduceType TonemappingStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", target_->get_color_tex()),
	};
}

void TonemappingStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("Tonemap");
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();
}

void TonemappingStage::reload_shaders(void)
{
	target_->set_shader(load_plugin_shader({"apply_tonemap.frag.glsl"}, stereo_mode_));
}

std::string TonemappingStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
