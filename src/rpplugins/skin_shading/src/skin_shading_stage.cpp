#include "skin_shading_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

SkinShadingStage::RequireType SkinShadingStage::required_inputs;
SkinShadingStage::RequireType SkinShadingStage::required_pipes = { "ShadedScene", "GBuffer" };

SkinShadingStage::ProduceType SkinShadingStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", _final_tex.p()),
	};
}

void SkinShadingStage::create(void)
{
	Texture* current_tex = nullptr;
	_blur_targets.clear();

	for (int i = 0; i < 3; ++i)
	{
		auto target_h = create_target(std::string("BlurH-") + std::to_string(i));
		target_h->add_color_attachment(16);
		target_h->prepare_buffer();
		target_h->set_shader_input(ShaderInput("direction", LVecBase2i(1, 0)));
		if (current_tex)
			target_h->set_shader_input(ShaderInput("ShadedScene", current_tex), true);
		current_tex = target_h->get_color_tex();

		auto target_v = create_target(std::string("BlurV-") + std::to_string(i));
		target_v->add_color_attachment(16);
		target_v->prepare_buffer();
		target_v->set_shader_input(ShaderInput("ShadedScene", current_tex), true);
		target_v->set_shader_input(ShaderInput("direction", LVecBase2i(0, 1)));
		current_tex = target_v->get_color_tex();

		_blur_targets.push_back(target_h);
		_blur_targets.push_back(target_v);
	}
	_final_tex = current_tex;
}

void SkinShadingStage::reload_shaders(void)
{
	PT(Shader) blur_shader = load_plugin_shader({"sssss_blur.frag.glsl"});
	for (auto& target: _blur_targets)
		target->set_shader(blur_shader);
}

std::string SkinShadingStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
