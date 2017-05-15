#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/loader.h>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING)
{
}

void Plugin::on_stage_setup(void)
{
    _bloom_stage = std::make_shared<DoFStage>(pipeline_);
    add_stage(_bloom_stage);
}

}
