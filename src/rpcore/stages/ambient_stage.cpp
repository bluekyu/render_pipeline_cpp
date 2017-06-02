#include <render_pipeline/rpcore/stages/ambient_stage.hpp>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpcore {

AmbientStage::RequireType AmbientStage::required_inputs_ = {
	"DefaultEnvmap", "PrefilteredBRDF", "PrefilteredMetalBRDF", "PrefilteredCoatBRDF" };
AmbientStage::RequireType AmbientStage::required_pipes_ = { "ShadedScene", "GBuffer" };

AmbientStage::ProduceType AmbientStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", target_->get_color_tex()),
		ShaderInput("PostAmbientScene", target_->get_color_tex()),
	};
}

void AmbientStage::create(void)
{
	stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("AmbientStage");
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();
}

void AmbientStage::reload_shaders(void)
{
	target_->set_shader(load_shader({"ambient_stage.frag.glsl"}, stereo_mode_));
}

std::string AmbientStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
