#include "../include/scattering_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/stage_manager.h>
#include <render_pipeline/rpcore/util/task_scheduler.h>	
#include <render_pipeline/rpcore/util/shader_input_blocks.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/stages/cull_lights_stage.h>

#include "scattering_stage.h"
#include "scattering_envmap_stage.h"
#include "godray_stage.h"
#include "scattering_methods.h"

RPCPP_PLUGIN_CREATOR(rpplugins::ScatteringPlugin)

namespace rpplugins {

struct ScatteringPlugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<ScatteringStage> display_stage_;
    std::shared_ptr<ScatteringEnvmapStage> envmap_stage_;
    std::shared_ptr<GodrayStage> godray_stage_;
    std::shared_ptr<ScatteringMethod> scattering_model_;
};

ScatteringPlugin::RequrieType ScatteringPlugin::Impl::require_plugins_;

// ************************************************************************************************

ScatteringPlugin::ScatteringPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPCPP_PLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
{
}

ScatteringPlugin::RequrieType& ScatteringPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void ScatteringPlugin::on_pipeline_created(void)
{
    impl_->scattering_model_->load();
    impl_->scattering_model_->compute();
}

void ScatteringPlugin::on_stage_setup(void)
{
    impl_->display_stage_ = std::make_shared<ScatteringStage>(pipeline_);
    add_stage(impl_->display_stage_);
    impl_->envmap_stage_ = std::make_shared<ScatteringEnvmapStage>(pipeline_);
    add_stage(impl_->envmap_stage_);

    if (boost::any_cast<bool>(get_setting("enable_godrays")))
    {
        impl_->godray_stage_ = std::make_shared<GodrayStage>(pipeline_);
        add_stage(impl_->godray_stage_);
    }

    // Load scattering method
    const std::string method(boost::any_cast<const std::string&>(get_setting("scattering_method")));

    debug(std::string("Loading scattering method for '") + method + "'");

    if (method == "eric_bruneton")
    {
        impl_->scattering_model_ = std::make_shared<ScatteringMethodEricBruneton>(*this);
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

void ScatteringPlugin::on_pre_render_update(void)
{
    impl_->envmap_stage_->set_active(pipeline_.get_task_scheduler()->is_scheduled("scattering_update_envmap"));
}

void ScatteringPlugin::on_shader_reload(void)
{
    impl_->scattering_model_->compute();
}

LVecBase3f ScatteringPlugin::get_sun_vector(void)
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

const std::shared_ptr<ScatteringStage>& ScatteringPlugin::get_display_stage(void) const
{
    return impl_->display_stage_;
}

const std::shared_ptr<ScatteringEnvmapStage>& ScatteringPlugin::get_envmap_stage(void) const
{
    return impl_->envmap_stage_;
}

}
