#include "plugin.hpp"

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/stages/ambient_stage.h>

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Ambient Occlusion",
    "tobspr <tobias.springer1@gmail.com>",
    "1.1",

    "A plugin to render ambient occlusion supporting multiple"
    "techniques such as SSAO, HBAO and much more ",
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

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info)
{
}

Plugin::~Plugin(void) = default;

void Plugin::on_stage_setup(void)
{
    stage_ = std::make_shared<AOStage>(pipeline_);
    add_stage(stage_);

    stage_->set_quality(boost::any_cast<std::string>(get_setting("blur_quality")));

    // Make the stages use our output
    rpcore::AmbientStage::get_global_required_pipes().push_back("AmbientOcclusion");
}

}
