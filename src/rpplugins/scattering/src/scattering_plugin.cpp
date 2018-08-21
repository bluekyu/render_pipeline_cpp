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

#include "../include/scattering_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/stage_manager.hpp>
#include <render_pipeline/rpcore/util/task_scheduler.hpp>
#include <render_pipeline/rpcore/util/shader_input_blocks.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/stages/cull_lights_stage.hpp>

#include "scattering_stage.hpp"
#include "scattering_envmap_stage.hpp"
#include "godray_stage.hpp"
#include "scattering_methods.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::ScatteringPlugin)

namespace rpplugins {

class ScatteringPlugin::Impl
{
public:
    static RequrieType require_plugins_;

    ScatteringStage* display_stage_;
    ScatteringEnvmapStage* envmap_stage_;
    std::unique_ptr<ScatteringMethod> scattering_model_;
};

ScatteringPlugin::RequrieType ScatteringPlugin::Impl::require_plugins_;

// ************************************************************************************************

ScatteringPlugin::ScatteringPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING), impl_(std::make_unique<Impl>())
{
}

ScatteringPlugin::~ScatteringPlugin() = default;

ScatteringPlugin::RequrieType& ScatteringPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void ScatteringPlugin::on_pipeline_created()
{
    impl_->scattering_model_->load();
    impl_->scattering_model_->compute();
}

void ScatteringPlugin::on_stage_setup()
{
    auto display_stage = std::make_unique<ScatteringStage>(pipeline_);
    impl_->display_stage_ = display_stage.get();
    add_stage(std::move(display_stage));

    auto envmap_stage = std::make_unique<ScatteringEnvmapStage>(pipeline_);
    impl_->envmap_stage_ = envmap_stage.get();
    add_stage(std::move(envmap_stage));

    if (boost::any_cast<bool>(get_setting("enable_godrays")))
    {
        add_stage(std::make_unique<GodrayStage>(pipeline_));
    }

    // Load scattering method
    const std::string method(boost::any_cast<const std::string&>(get_setting("scattering_method")));

    debug(std::string("Loading scattering method for '") + method + "'");

    if (method == "eric_bruneton")
    {
        impl_->scattering_model_ = std::make_unique<ScatteringMethodEricBruneton>(*this);
    }
//  TODO: implement.
//  else if (method == "hosek_wilkie")
//  {
//  }
    else
    {
        error("Unrecognized scattering method!");
    }
}

void ScatteringPlugin::on_pre_render_update()
{
    impl_->envmap_stage_->set_active(pipeline_.get_task_scheduler()->is_scheduled("scattering_update_envmap"));
}

void ScatteringPlugin::on_shader_reload()
{
    impl_->scattering_model_->compute();
}

LVecBase3f ScatteringPlugin::get_sun_vector() const
{
    float sun_altitude = get_daytime_setting("sun_altitude").first[0];
    float sun_azimuth = get_daytime_setting("sun_azimuth").first[0];

    static const float pi = std::acos(-1);
    const float theta = (90.0f - sun_altitude) / 180.0f * pi;
    const float phi = sun_azimuth / 180.0f * pi;
    const float sin_theta = std::sin(theta);
    return LVecBase3f(
        sin_theta * std::cos(phi),
        sin_theta * std::sin(phi),
        std::cos(theta));
}

ScatteringStage* ScatteringPlugin::get_display_stage() const
{
    return impl_->display_stage_;
}

ScatteringEnvmapStage* ScatteringPlugin::get_envmap_stage() const
{
    return impl_->envmap_stage_;
}

}
