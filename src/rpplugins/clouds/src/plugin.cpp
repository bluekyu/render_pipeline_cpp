#include "plugin.hpp"

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/loader.h>

RPCPP_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins = { "scattering" };

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPCPP_PLUGIN_ID_STRING)
{
}

void Plugin::on_stage_setup(void)
{
    _apply_stage = std::make_shared<ApplyCloudsStage>(pipeline_);
    add_stage(_apply_stage);
}

void Plugin::on_pipeline_created(void)
{
    // High-res noise
    PT(Texture) noise1 = rpcore::RPLoader::load_texture(get_resource("noise1-data.txo"));
    noise1->set_wrap_u(SamplerState::WM_repeat);
    noise1->set_wrap_v(SamplerState::WM_repeat);
    noise1->set_wrap_w(SamplerState::WM_repeat);
    noise1->set_minfilter(SamplerState::FT_linear_mipmap_linear);
    _apply_stage->set_shader_input(ShaderInput("Noise1", noise1));

    // Low-res noise
    PT(Texture) noise2 = rpcore::RPLoader::load_texture(get_resource("noise2-data.txo"));
    noise2->set_wrap_u(SamplerState::WM_repeat);
    noise2->set_wrap_v(SamplerState::WM_repeat);
    noise2->set_wrap_w(SamplerState::WM_repeat);
    noise2->set_minfilter(SamplerState::FT_linear_mipmap_linear);
    _apply_stage->set_shader_input(ShaderInput("Noise2", noise2));

    // Weather tex
    PT(Texture) weather = rpcore::RPLoader::load_texture(get_resource("weather_tex.png"));
    weather->set_wrap_u(SamplerState::WM_repeat);
    weather->set_wrap_v(SamplerState::WM_repeat);
    _apply_stage->set_shader_input(ShaderInput("WeatherTex", weather));
}

}
