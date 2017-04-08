#include "sharpen_stage.h"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/globals.h>

namespace rpplugins {

SharpenStage::RequireType SharpenStage::required_inputs;
SharpenStage::RequireType SharpenStage::required_pipes = { "ShadedScene" };

SharpenStage::ProduceType SharpenStage::get_produced_pipes(void) const
{
	if (_sharpen_twice)
	{
		return {
			ShaderInput("ShadedScene", _target2->get_color_tex()),
		};
	}
	else
	{
		return {
			ShaderInput("ShadedScene", _target->get_color_tex()),
		};
	}
}

void SharpenStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

	_target = create_target("Sharpen");
	_target->add_color_attachment(16);
    if (stereo_mode_)
        _target->set_layers(2);
	_target->prepare_buffer();

	if (_sharpen_twice)
	{
		_target2 = create_target("Sharpen2");
		_target2->add_color_attachment(16);
        if (stereo_mode_)
                _target2->set_layers(2);
		_target2->prepare_buffer();
		_target2->set_shader_input(ShaderInput("ShadedScene", _target->get_color_tex()));
	}
}

void SharpenStage::reload_shaders(void)
{
	auto sharpen_shader = load_plugin_shader({"sharpen.frag.glsl"}, stereo_mode_);
	for (auto& target: targets_)
		target.second->set_shader(sharpen_shader);
}

std::string SharpenStage::get_plugin_id(void) const
{
	return PLUGIN_ID_STRING;
}

}	// namespace rpplugins
