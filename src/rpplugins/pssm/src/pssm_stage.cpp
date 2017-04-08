#include "pssm_stage.h"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/globals.h>

namespace rpplugins {

PSSMStage::RequireType PSSMStage::required_inputs;
PSSMStage::RequireType PSSMStage::required_pipes = { "ShadedScene", "PSSMShadowAtlas", "GBuffer", "PSSMShadowAtlasPCF" };

PSSMStage::ProduceType PSSMStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", _target->get_color_tex()),
	};
}

void PSSMStage::create(void)
{
	stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    _enabled = true;
    _target_shadows = create_target("FilterPSSM");
    _target_shadows->add_color_attachment(LVecBase4i(8, 0, 0, 0));
	if (stereo_mode_)
		_target_shadows->set_layers(2);
    _target_shadows->prepare_buffer();
    _target_shadows->get_color_tex()->set_clear_color(LColorf(0));

    _target = create_target("ApplyPSSMShadows");
    _target->add_color_attachment(LVecBase4i(16));
	if (stereo_mode_)
		_target->set_layers(2);
    _target->prepare_buffer();

    _target->set_shader_input(ShaderInput("PrefilteredShadows", _target_shadows->get_color_tex()));
}

void PSSMStage::set_render_shadows(bool enabled)
{
    _target_shadows->set_active(enabled);
    if (enabled != _enabled)
    {
        _target_shadows->get_color_tex()->clear_image();
        _enabled = enabled;
    }
}

void PSSMStage::reload_shaders(void)
{
	_target_shadows->set_shader(load_plugin_shader({"filter_pssm_shadows.frag.glsl"}, stereo_mode_));
	_target->set_shader(load_plugin_shader({"apply_sun_shading.frag.glsl"}, stereo_mode_));
}

std::string PSSMStage::get_plugin_id(void) const
{
	return PLUGIN_ID_STRING;
}

}	// namespace rpplugins
