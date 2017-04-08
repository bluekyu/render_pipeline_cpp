#include "ssr_plugin.hpp"

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/stages/ambient_stage.h>

#include "ssr_stage.h"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Screen Space Reflections",
    "tobspr <tobias.springer1@gmail.com>",
    "beta (!)",

    "This plugin adds support for stochastik screen space reflections "
    "and is still very experimental!"
};

}

// ************************************************************************************************

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::SSRPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

namespace rpplugins {

struct SSRPlugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<SSRStage> ssr_stage_;
};

SSRPlugin::RequrieType SSRPlugin::Impl::require_plugins_;

// ************************************************************************************************

SSRPlugin::SSRPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, plugin_info), impl_(std::make_unique<Impl>())
{
}

SSRPlugin::RequrieType& SSRPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void SSRPlugin::on_stage_setup(void)
{
    impl_->ssr_stage_ = std::make_shared<SSRStage>(pipeline_);
    add_stage(impl_->ssr_stage_);

    if (is_plugin_enabled("color_correction"))
        impl_->ssr_stage_->get_global_required_pipes().push_back("FuturePipe::Exposure");
}

}
