/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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

SkyAOPlugin::RequrieType& SkyAOPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void SkyAOPlugin::on_stage_setup()
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

void SkyAOPlugin::on_post_stage_setup()
{
    if (is_plugin_enabled("env_probes"))
    {
        const auto& capture_stage = dynamic_cast<EnvProbesPlugin*>(get_plugin_instance("env_probes"))->get_capture_stage();

        capture_stage->get_required_inputs().push_back("SkyAOCapturePosition");
        capture_stage->get_required_pipes().push_back("SkyAOHeight");
    }
}

}
