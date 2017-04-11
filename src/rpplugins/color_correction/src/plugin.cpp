#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/loader.h>

RPCPP_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPCPP_PLUGIN_ID_STRING)
{
}

void Plugin::on_pipeline_created(void)
{
	load_lut();
	load_grain();
}

void Plugin::on_stage_setup(void)
{
	_stage = std::make_shared<ColorCorrectionStage>(pipeline_);
	add_stage(_stage);

	_tonemapping_stage = std::make_shared<TonemappingStage>(pipeline_);
	add_stage(_tonemapping_stage);

	if (boost::any_cast<bool>(get_setting("use_sharpen")))
	{
		_sharpen_stage = std::make_shared<SharpenStage>(pipeline_);
		add_stage(_sharpen_stage);
		_sharpen_stage->set_sharpen_twice(boost::any_cast<bool>(get_setting("sharpen_twice")));
	}

	if (boost::any_cast<bool>(get_setting("manual_camera_parameters")))
	{
		_exposure_stage_manual = std::make_shared<ManualExposureStage>(pipeline_);
		add_stage(_exposure_stage_manual);
	}
	else
	{
		_exposure_stage_auto = std::make_shared<AutoExposureStage>(pipeline_);
		add_stage(_exposure_stage_auto);
	}
}

void Plugin::load_lut(void)
{
	std::string lut_path = get_resource(boost::any_cast<const std::string&>(get_setting("color_lut")));
	PT(Texture) lut = rpcore::RPLoader::load_sliced_3d_texture(lut_path, 64);
	lut->set_wrap_u(SamplerState::WM_clamp);
	lut->set_wrap_v(SamplerState::WM_clamp);
	lut->set_wrap_w(SamplerState::WM_clamp);
	lut->set_minfilter(SamplerState::FT_linear);
	lut->set_magfilter(SamplerState::FT_linear);
	lut->set_anisotropic_degree(0);
	_tonemapping_stage->set_shader_input(ShaderInput("ColorLUT", lut));
}

void Plugin::load_grain(void)
{
	PT(Texture) grain_tex = rpcore::RPLoader::load_texture("/$$rp/data/film_grain/grain.txo");
	grain_tex->set_minfilter(SamplerState::FT_linear);
	grain_tex->set_magfilter(SamplerState::FT_linear);
	grain_tex->set_wrap_u(SamplerState::WM_repeat);
	grain_tex->set_wrap_v(SamplerState::WM_repeat);
	grain_tex->set_anisotropic_degree(0);
	_stage->set_shader_input(ShaderInput("PrecomputedGrain", grain_tex));
}

void Plugin::update_color_lut(void)
{
	debug("Updating color lut ..");
	load_lut();
}

}
