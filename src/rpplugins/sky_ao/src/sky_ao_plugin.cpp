#include "sky_ao_plugin.hpp"

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/stages/ambient_stage.h>

#include "ao_stage.h"
#include "capture_stage.h"
#include "../../env_probes/include/env_probes_plugin.hpp"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Sky Occlusion",
    "tobspr <tobias.springer1@gmail.com>",
    "0.2 beta",

    "This plugin adds support for sky occlusion, computing "
    "occlusion based on the sky visibility."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::SkyAOPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

struct SkyAOPlugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<SkyAOCaptureStage> capture_stage_;
    std::shared_ptr<SkyAOStage> ao_stage_;
};

SkyAOPlugin::RequrieType SkyAOPlugin::Impl::require_plugins_;

// ************************************************************************************************

SkyAOPlugin::SkyAOPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info), impl_(std::make_unique<Impl>())
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
