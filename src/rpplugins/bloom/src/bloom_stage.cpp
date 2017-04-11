#include "bloom_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/globals.h>

namespace rpplugins {

BloomStage::RequireType BloomStage::required_inputs;
BloomStage::RequireType BloomStage::required_pipes = { "ShadedScene" };

BloomStage::ProduceType BloomStage::get_produced_pipes(void) const
{
	return {
		ShaderInput("ShadedScene", _target_apply->get_color_tex()),
	};
}

void BloomStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

	if (_remove_fireflies)
	{
		_target_firefly = create_target("RemoveFireflies");
		_target_firefly->add_color_attachment(16);
        if (stereo_mode_)
            _target_firefly->set_layers(2);
		_target_firefly->prepare_buffer();
	}

    if (stereo_mode_)
	    _scene_target_img = rpcore::Image::create_2d_array("BloomDownsample", 0, 0, 2, "RGBA16");
    else
        _scene_target_img = rpcore::Image::create_2d("BloomDownsample", 0, 0, "RGBA16");
	_scene_target_img->set_minfilter(SamplerState::FT_linear_mipmap_linear);
	_scene_target_img->set_magfilter(SamplerState::FT_linear);
	_scene_target_img->set_wrap_u(SamplerState::WM_clamp);
	_scene_target_img->set_wrap_v(SamplerState::WM_clamp);
	_scene_target_img->set_clear_color(LColorf(0.1f, 0.0f, 0.0f, 1.0f));
	_scene_target_img->clear_image();

	_target_extract = create_target("ExtractBrightSpots");
	_target_extract->prepare_buffer();
    _target_extract->set_shader_input(ShaderInput("DestTex", _scene_target_img->get_texture(), false, true, -1, 0));

	if (_remove_fireflies)
		_target_extract->set_shader_input(ShaderInput("ShadedScene", _target_firefly->get_color_tex(), 1000));

	// Downsample passes
	for (int i = 0; i < _num_mips; ++i)
	{
		int scale_multiplier = std::pow(2, 1 + i);
		auto target = create_target(std::string("Downsample:Step-") + std::to_string(i));
		target->set_size(-scale_multiplier);
		target->prepare_buffer();
		target->set_shader_input(ShaderInput("sourceMip", LVecBase4i(i, 0, 0, 0)));
        target->set_shader_input(ShaderInput("SourceTex", _scene_target_img->get_texture()));
        target->set_shader_input(ShaderInput("DestTex", _scene_target_img->get_texture(), false, true, -1, i + 1));
		_downsample_targets.push_back(target);
	}

	// Upsample passes
	for (int i = 0; i < _num_mips; ++i)
	{
		int scale_multiplier = std::pow(2, _num_mips - i - 1);
		auto target = create_target(std::string("Upsample:Step-") + std::to_string(i));
		target->set_size(-scale_multiplier);
		target->prepare_buffer();
		target->set_shader_input(ShaderInput("FirstUpsamplePass", LVecBase4i(i == 0, 0, 0, 0)));

		target->set_shader_input(ShaderInput("sourceMip", LVecBase4i(_num_mips - i, 0, 0, 0)));
        target->set_shader_input(ShaderInput("SourceTex", _scene_target_img->get_texture()));
        target->set_shader_input(ShaderInput("DestTex", _scene_target_img->get_texture(), false, true, -1, _num_mips - i - 1));
		_upsample_targets.push_back(target);
	}

	_target_apply = create_target("ApplyBloom");
	_target_apply->add_color_attachment(16);
    if (stereo_mode_)
        _target_apply->set_layers(2);
	_target_apply->prepare_buffer();
    _target_apply->set_shader_input(ShaderInput("BloomTex", _scene_target_img->get_texture()));
}

void BloomStage::set_dimensions(void)
{
	_scene_target_img->set_x_size(rpcore::Globals::resolution.get_x());
	_scene_target_img->set_y_size(rpcore::Globals::resolution.get_y());
}

void BloomStage::reload_shaders(void)
{
	_target_extract->set_shader(load_plugin_shader({"extract_bright_spots.frag.glsl"}));

	if (_remove_fireflies)
		_target_firefly->set_shader(load_plugin_shader({"remove_fireflies.frag.glsl"}, stereo_mode_));
	_target_apply->set_shader(load_plugin_shader({"apply_bloom.frag.glsl"}, stereo_mode_));

	PT(Shader) downsample_shader = load_plugin_shader({"bloom_downsample.frag.glsl"});
	for (auto& target: _downsample_targets)
		target->set_shader(downsample_shader);

	PT(Shader) upsample_shader = load_plugin_shader({"bloom_upsample.frag.glsl"});
	for (auto& target: _upsample_targets)
		target->set_shader(upsample_shader);
}

std::string BloomStage::get_plugin_id(void) const
{
	return RPCPP_PLUGIN_ID_STRING;
}

}	// namespace rpplugins
