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

    VolumetricsStage* stage_;
};

VolumentricsPlugin::RequrieType VolumentricsPlugin::Impl::require_plugins_ = { "pssm" };

VolumentricsPlugin::VolumentricsPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGINS_ID_STRING), impl_(std::make_unique<Impl>())
{
}

VolumentricsPlugin::RequrieType& VolumentricsPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void VolumentricsPlugin::on_stage_setup()
{
    auto stage = std::make_unique<VolumetricsStage>(pipeline_);
    impl_->stage_ = stage.get();
    add_stage(std::move(stage));

    impl_->stage_->set_enable_volumetric_shadows(boost::any_cast<bool>(get_setting("enable_volumetric_shadows")));

    if (boost::any_cast<bool>(get_plugin_instance("pssm")->get_setting("use_pcf")))
        impl_->stage_->get_required_pipes().push_back("PSSMShadowAtlasPCF");
    else
        impl_->stage_->get_required_pipes().push_back("PSSMShadowAtlas");
}

VolumetricsStage* VolumentricsPlugin::get_stage() const
{
    return impl_->stage_;
}

}
