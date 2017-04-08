#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Motion blur",
    "tobspr <tobias.springer1@gmail.com>",
    "1.0",

    "This plugin adds support for motion blur. This includes "
    "per-object motion blur aswell as camera-motion blur."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::Plugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, plugin_info)
{
}

void Plugin::on_stage_setup(void)
{
    _stage = std::make_shared<MotionBlurStage>(pipeline_);
    add_stage(_stage);
    _stage->set_tile_size(boost::any_cast<int>(get_setting("tile_size")));
    _stage->set_per_object_blur(boost::any_cast<bool>(get_setting("enable_object_blur")));
}

}
