#include "ssr_plugin.hpp"

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/stages/ambient_stage.hpp>

#include "ssr_stage.h"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SSRPlugin)

namespace rpplugins {

struct SSRPlugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<SSRStage> ssr_stage_;
};

SSRPlugin::RequrieType SSRPlugin::Impl::require_plugins_;

// ************************************************************************************************

SSRPlugin::SSRPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
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
