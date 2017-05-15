#include "skin_shading_plugin.hpp"

#include <boost/dll/alias.hpp>

#include "skin_shading_stage.h"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SkinShadingPlugin)

namespace rpplugins {

struct SkinShadingPlugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<SkinShadingStage> stage_;
};

SkinShadingPlugin::RequrieType SkinShadingPlugin::Impl::require_plugins_;

// ************************************************************************************************

SkinShadingPlugin::SkinShadingPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
{
}

SkinShadingPlugin::RequrieType& SkinShadingPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void SkinShadingPlugin::on_stage_setup(void)
{
	impl_->stage_ = std::make_shared<SkinShadingStage>(pipeline_);
	add_stage(impl_->stage_);
}

}
