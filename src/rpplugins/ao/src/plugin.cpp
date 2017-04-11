#include "plugin.hpp"

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/stages/ambient_stage.h>

RPCPP_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPCPP_PLUGIN_ID_STRING)
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
