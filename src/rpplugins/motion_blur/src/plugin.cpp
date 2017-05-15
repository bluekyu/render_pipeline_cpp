#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGIN_ID_STRING)
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
