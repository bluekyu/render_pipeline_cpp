#include "../include/volumetrics_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include "../include/volumetrics_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::VolumentricsPlugin)

namespace rpplugins {

class VolumentricsPlugin::Impl
{
public:
    static RequrieType require_plugins_;

    std::shared_ptr<VolumetricsStage> stage_;
};

VolumentricsPlugin::RequrieType VolumentricsPlugin::Impl::require_plugins_ = { "pssm" };

VolumentricsPlugin::VolumentricsPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
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
