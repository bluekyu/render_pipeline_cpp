#include "sky_ao_plugin.hpp"

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/stages/ambient_stage.hpp>

#include "ao_stage.hpp"
#include "capture_stage.hpp"
#include "env_probes_plugin.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SkyAOPlugin)

namespace rpplugins {

class SkyAOPlugin::Impl
{
public:
    static RequrieType require_plugins_;

    std::shared_ptr<SkyAOCaptureStage> capture_stage_;
    std::shared_ptr<SkyAOStage> ao_stage_;
};

SkyAOPlugin::RequrieType SkyAOPlugin::Impl::require_plugins_;

// ************************************************************************************************

SkyAOPlugin::SkyAOPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
{
}

SkyAOPlugin::RequrieType& SkyAOPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void SkyAOPlugin::on_stage_setup(void)
{
    impl_->capture_stage_ = std::make_shared<SkyAOCaptureStage>(pipeline_);
    add_stage(impl_->capture_stage_);

    impl_->ao_stage_ = std::make_shared<SkyAOStage>(pipeline_);
    add_stage(impl_->ao_stage_);

    impl_->capture_stage_->set_resolution(boost::any_cast<int>(get_setting("resolution")));
    impl_->capture_stage_->set_max_radius(boost::any_cast<float>(get_setting("max_radius")));
    impl_->capture_stage_->set_capture_height(boost::any_cast<float>(get_setting("capture_height")));

    // Make the stages use our output
    rpcore::AmbientStage::get_global_required_pipes().push_back("SkyAO");
}

void SkyAOPlugin::on_post_stage_setup(void)
{
    if (is_plugin_enabled("env_probes"))
    {
        const auto& capture_stage = std::dynamic_pointer_cast<EnvProbesPlugin>(get_plugin_instance("env_probes"))->get_capture_stage();

        capture_stage->get_required_inputs().push_back("SkyAOCapturePosition");
        capture_stage->get_required_pipes().push_back("SkyAOHeight");
    }
}

}
