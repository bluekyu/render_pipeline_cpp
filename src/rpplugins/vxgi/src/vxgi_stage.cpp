#include "vxgi_stage.h"

#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/stages/ambient_stage.h>

namespace rpplugins {

VXGIStage::RequireType VXGIStage::required_inputs = { "voxelGridPosition" };
VXGIStage::RequireType VXGIStage::required_pipes = {
    "ShadedScene", "SceneVoxels", "GBuffer", "ScatteringIBLSpecular",
    "ScatteringIBLDiffuse", "PreviousFrame::VXGIPostSample",
    "CombinedVelocity", "PreviousFrame::SceneDepth" };

VXGIStage::ProduceType VXGIStage::get_produced_pipes(void) const
{
	return {
		//ShaderInput("VXGISpecular", _target_spec->get_color_tex()),
        ShaderInput("VXGIDiffuse", _target_resolve->get_color_tex()),
        ShaderInput("VXGIPostSample", _target_resolve->get_color_tex()),
	};
}

void VXGIStage::create(void)
{
    // Create a target for the specular GI
    // _target_spec = create_target("SpecularGI");
    // _target_spec->add_color_attachment(16, true);
    // _target_spec->prepare_buffer();

    // Create a target for the diffuse GI
    _target_diff = create_target("DiffuseGI");
    _target_diff->set_size(-2);
    _target_diff->add_color_attachment(16);
    _target_diff->prepare_buffer();

    // Create the target which blurs the diffuse result
    _target_blur_v = create_target("BlurV");
    _target_blur_v->set_size(-2);
    _target_blur_v->add_color_attachment(16);
    _target_blur_v->prepare_buffer();
    _target_blur_v->set_shader_input(ShaderInput("SourceTex", _target_diff->get_color_tex()));

    _target_blur_h = create_target("BlurH");
    _target_blur_h->set_size(-2);
    _target_blur_h->add_color_attachment(16);
    _target_blur_h->prepare_buffer();
    _target_blur_h->set_shader_input(ShaderInput("SourceTex", _target_blur_v->get_color_tex()));

    // Set blur parameters
    _target_blur_v->set_shader_input(ShaderInput("blur_direction", LVecBase2i(0, 1)));
    _target_blur_h->set_shader_input(ShaderInput("blur_direction", LVecBase2i(1, 0)));

    // Create the target which bilateral upsamples the diffuse target
    _target_upscale_diff = create_target("UpscaleDiffuse");
    _target_upscale_diff->add_color_attachment(16);
    _target_upscale_diff->prepare_buffer();
    _target_upscale_diff->set_shader_input(ShaderInput("SourceTex", _target_blur_h->get_color_tex()));
    _target_upscale_diff->set_shader_input(ShaderInput("upscaleWeights", LVecBase2f(0.0001, 0.001)));

    _target_resolve = create_target("ResolveVXGI");
    _target_resolve->add_color_attachment(16);
    _target_resolve->prepare_buffer();
    _target_resolve->set_shader_input(ShaderInput("CurrentTex", _target_upscale_diff->get_color_tex()));

    // Make the ambient stage use the GI result
    rpcore::AmbientStage::get_global_required_pipes().push_back("VXGIDiffuse");
}

void VXGIStage::reload_shaders(void)
{
	//_target_spec->set_shader(load_plugin_shader({ "vxgi_specular.frag.glsl" }));
    _target_diff->set_shader(load_plugin_shader({"vxgi_diffuse.frag.glsl"}));
    _target_upscale_diff->set_shader(load_plugin_shader({"/$$rp/shader/bilateral_upscale.frag.glsl"}));

    PT(Shader) blur_shader = load_plugin_shader({"/$$rp/shader/bilateral_halfres_blur.frag.glsl"});
    _target_blur_v->set_shader(blur_shader);
    _target_blur_h->set_shader(blur_shader);
    _target_resolve->set_shader(load_plugin_shader({"resolve_vxgi.frag.glsl"}));
}

std::string VXGIStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
