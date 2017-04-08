#include "../include/volumetrics_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include "../include/volumetrics_stage.hpp"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Volumetric Lighting",
    "tobspr <tobias.springer1@gmail.com>",
    "0.1 alpha (!)",

    "This plugins adds support for volumetric lighting"
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::VolumentricsPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

struct VolumentricsPlugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<VolumetricsStage> stage_;
};

VolumentricsPlugin::RequrieType VolumentricsPlugin::Impl::require_plugins_ = { "pssm" };

VolumentricsPlugin::VolumentricsPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, plugin_info), impl_(std::make_unique<Impl>())
{
}

VolumentricsPlugin::RequrieType& VolumentricsPlugin::get_required_plugins(void) const 
{
    return impl_->require_plugins_;
}

void VolumentricsPlugin::on_stage_setup(void)
{
    impl_->stage_ = std::make_shared<VolumetricsStage>(pipeline_);
    add_stage(impl_->stage_);

    impl_->stage_->set_enable_volumetric_shadows(boost::any_cast<bool>(get_setting("enable_volumetric_shadows")));

    if (boost::any_cast<bool>(get_plugin_instance("pssm")->get_setting("use_pcf")))
        impl_->stage_->get_required_pipes().push_back("PSSMShadowAtlasPCF");
    else
        impl_->stage_->get_required_pipes().push_back("PSSMShadowAtlas");
}

const std::shared_ptr<VolumetricsStage>& VolumentricsPlugin::get_stage(void) const
{
    return impl_->stage_;
}

}
