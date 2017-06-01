#include "ao_stage.h"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

SkyAOStage::RequireType SkyAOStage::required_inputs = { "SkyAOCapturePosition" };
SkyAOStage::RequireType SkyAOStage::required_pipes = { "SkyAOHeight", "GBuffer" };

SkyAOStage::ProduceType SkyAOStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("SkyAO", target_upscale_->get_color_tex()),
	};
}

void SkyAOStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("ComputeSkyAO");
    target_->set_size(-2);
    target_->add_color_attachment(LVecBase4i(16, 0, 0, 0));
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();

    target_upscale_ = create_target("UpscaleSkyAO");
    target_upscale_->add_color_attachment(LVecBase4i(16, 0, 0, 0));
    if (stereo_mode_)
        target_upscale_->set_layers(2);
    target_upscale_->prepare_buffer();

    target_upscale_->set_shader_input(ShaderInput("SourceTex", target_->get_color_tex()));
    target_upscale_->set_shader_input(ShaderInput("upscaleWeights", LVecBase2f(0.001f, 0.001f)));
}

void SkyAOStage::reload_shaders(void)
{
    target_->set_shader(load_plugin_shader({"compute_sky_ao.frag.glsl"}, stereo_mode_));

    if (stereo_mode_)
        target_upscale_->set_shader(load_plugin_shader({ "/$$rp/shader/bilateral_upscale_stereo.frag.glsl" }, stereo_mode_));
    else
        target_upscale_->set_shader(load_plugin_shader({ "/$$rp/shader/bilateral_upscale.frag.glsl" }));
}

std::string SkyAOStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
