#include "ssr_stage.h"

#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/stages/ambient_stage.h>

namespace rpplugins {

SSRStage::RequireType SSRStage::required_inputs;
SSRStage::RequireType SSRStage::required_pipes = {
    "ShadedScene", "CombinedVelocity", "GBuffer",
    "DownscaledDepth", "PreviousFrame::PostAmbientScene",
    "PreviousFrame::SSRSpecular", "PreviousFrame::SceneDepth" };

SSRStage::ProduceType SSRStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("SSRSpecular", _target_resolve->get_color_tex()),
	};
}

void SSRStage::create(void)
{
    _target = create_target("ComputeSSR");
    _target->set_size(-2);
    _target->add_color_attachment(LVecBase4i(16, 16, 0, 0));
    _target->prepare_buffer();

    _target->get_color_tex()->set_minfilter(SamplerState::FT_nearest);
    _target->get_color_tex()->set_magfilter(SamplerState::FT_nearest);

    _target_velocity = create_target("ReflectionVelocity");
    _target_velocity->add_color_attachment(LVecBase4i(16, 16, 0, 0));
    _target_velocity->prepare_buffer();
    _target_velocity->set_shader_input(ShaderInput("TraceResult", _target->get_color_tex()));

    _target_reproject_lighting = create_target("CopyLighting");
    _target_reproject_lighting->add_color_attachment(16, true);
    _target_reproject_lighting->prepare_buffer();

    _target_upscale = create_target("UpscaleSSR");
    _target_upscale->add_color_attachment(16, true);
    _target_upscale->prepare_buffer();
    _target_upscale->set_shader_input(ShaderInput("SourceTex", _target->get_color_tex()));
    _target_upscale->set_shader_input(ShaderInput("LastFrameColor", _target_reproject_lighting->get_color_tex()));

    _target_resolve = create_target("ResolveSSR");
    _target_resolve->add_color_attachment(16, true);
    _target_resolve->prepare_buffer();
    _target_resolve->set_shader_input(ShaderInput("CurrentTex", _target_upscale->get_color_tex()));
    _target_resolve->set_shader_input(ShaderInput("VelocityTex", _target_velocity->get_color_tex()));

    rpcore::AmbientStage::get_global_required_pipes().push_back("SSRSpecular");
}

void SSRStage::reload_shaders(void)
{
    _target->set_shader(load_plugin_shader({ "ssr_trace.frag.glsl" }));
    _target_velocity->set_shader(load_plugin_shader({ "reflection_velocity.frag.glsl" }));
    _target_reproject_lighting->set_shader(load_plugin_shader({ "reproject_lighting.frag.glsl" }));
    _target_upscale->set_shader(load_plugin_shader({ "upscale_bilateral_brdf.frag.glsl" }));
    _target_resolve->set_shader(load_plugin_shader({ "resolve_ssr.frag.glsl" }));
}

std::string SSRStage::get_plugin_id(void) const
{
	return RPCPP_PLUGIN_ID_STRING;
}

}	// namespace rpplugins
