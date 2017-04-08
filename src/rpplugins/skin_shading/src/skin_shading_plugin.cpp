#include "skin_shading_plugin.hpp"

#include <boost/dll/alias.hpp>

#include "skin_shading_stage.h"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Skin Shading",
    "tobspr <tobias.springer1@gmail.com>",
    "1.0",

    "This plugin adds support for Seperable Screen Space Subsurface "
    "Scattering (SSSSS). This improves skin rendering."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::SkinShadingPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

struct SkinShadingPlugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<SkinShadingStage> stage_;
};

SkinShadingPlugin::RequrieType SkinShadingPlugin::Impl::require_plugins_;

// ************************************************************************************************

SkinShadingPlugin::SkinShadingPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info), impl_(std::make_unique<Impl>())
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
