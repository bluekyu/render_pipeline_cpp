#include "plugin.hpp"

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/loader.h>

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Volumetric Clouds",
    "tobspr <tobias.springer1@gmail.com>",
    "alpha (!)",

    "This Plugin adds support or volumetric, raytraced clouds. "
    "Right now this is pretty unoptimized and may consum a lot "
    "of performance."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::Plugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins ={"scattering"};

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info)
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
