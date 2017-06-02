#include "../include/pssm_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <displayRegion.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/native/pssm_camera_rig.h>

#include "pssm_stage.h"
#include "pssm_shadow_stage.h"
#include "pssm_scene_shadow_stage.h"
#include "pssm_dist_shadow_stage.h"

#include "scattering_plugin.hpp"
#include "volumetrics_plugin.hpp"
#include "volumetrics_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::PSSMPlugin)

namespace rpplugins {

struct PSSMPlugin::Impl
{
    static void toggle_update_enabled(const Event* ev, void* user_data);

    Impl(PSSMPlugin& self);

    void on_pre_render_update(void);

public:
    static RequrieType require_plugins_;

    PSSMPlugin& self_;

    bool update_enabled_;
    PTA_LVecBase3f pta_sun_vector_;
    int last_cache_reset_;

    std::shared_ptr<PSSMShadowStage> shadow_stage_;
    std::shared_ptr<PSSMStage> pssm_stage_;
    std::shared_ptr<PSSMSceneShadowStage> scene_shadow_stage_;
    std::shared_ptr<PSSMDistShadowStage> dist_shadow_stage_;

    NodePath node_;
    std::shared_ptr<rpcore::PSSMCameraRig> camera_rig_;
};

PSSMPlugin::RequrieType PSSMPlugin::Impl::require_plugins_ = { "scattering" };

PSSMPlugin::Impl::Impl(PSSMPlugin& self): self_(self)
{
}

void PSSMPlugin::Impl::toggle_update_enabled(const Event* ev, void* user_data)
{
    PSSMPlugin* plugin = reinterpret_cast<PSSMPlugin*>(user_data);
    plugin->impl_->update_enabled_ = !plugin->impl_->update_enabled_;
    plugin->debug(std::string("Update enabled: ") + (plugin->impl_->update_enabled_ ? "True" : "False"));
}

void PSSMPlugin::Impl::on_pre_render_update(void)
{
    const LVecBase3f& sun_vector = std::dynamic_pointer_cast<rpplugins::ScatteringPlugin>(self_.get_plugin_instance("scattering"))->get_sun_vector();

    if (sun_vector.get_z() < 0.0f)
    {
        shadow_stage_->set_active(false);
        scene_shadow_stage_->set_active(false);
        pssm_stage_->set_render_shadows(false);

        if (boost::any_cast<bool>(self_.get_setting("use_distant_shadows")))
            dist_shadow_stage_->set_active(false);

        // Return, no need to update the pssm splits
        return;
    }
    else
    {
        shadow_stage_->set_active(true);
        scene_shadow_stage_->set_active(true);
        pssm_stage_->set_render_shadows(true);

        if (boost::any_cast<bool>(self_.get_setting("use_distant_shadows")))
            dist_shadow_stage_->set_active(true);
    }

    if (update_enabled_)
    {
        camera_rig_->update(rpcore::Globals::base->get_cam(), sun_vector);

        // Eventually reset cache
        double cache_diff = rpcore::Globals::clock->get_frame_time() - last_cache_reset_;
        if (cache_diff > 5.0)
        {
            last_cache_reset_ = rpcore::Globals::clock->get_frame_time();
            camera_rig_->reset_film_size_cache();
        }

        scene_shadow_stage_->set_sun_vector(sun_vector);

        if (boost::any_cast<bool>(self_.get_setting("use_distant_shadows")))
            dist_shadow_stage_->set_sun_vector(sun_vector);
    }
}

// ************************************************************************************************

PSSMPlugin::PSSMPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>(*this))
{
}

PSSMPlugin::RequrieType& PSSMPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void PSSMPlugin::on_stage_setup(void)
{
    impl_->update_enabled_ = true;
    impl_->pta_sun_vector_ = PTA_LVecBase3f::empty_array(1);
    impl_->last_cache_reset_ = 0;

    impl_->shadow_stage_ = std::make_shared<PSSMShadowStage>(pipeline_);
    add_stage(impl_->shadow_stage_);

    impl_->pssm_stage_ = std::make_shared<PSSMStage>(pipeline_);
    add_stage(impl_->pssm_stage_);

    impl_->shadow_stage_->set_num_splits(boost::any_cast<int>(get_setting("split_count")));
    impl_->shadow_stage_->set_split_resolution(boost::any_cast<int>(get_setting("resolution")));

    impl_->scene_shadow_stage_ = std::make_shared<PSSMSceneShadowStage>(pipeline_);
    add_stage(impl_->scene_shadow_stage_);
    impl_->scene_shadow_stage_->set_resolution(boost::any_cast<int>(get_setting("scene_shadow_resolution")));
    impl_->scene_shadow_stage_->set_sun_distance(boost::any_cast<float>(get_setting("scene_shadow_sundist")));

    if (boost::any_cast<bool>(get_setting("use_distant_shadows")))
    {
        impl_->dist_shadow_stage_ = std::make_shared<PSSMDistShadowStage>(pipeline_);
        add_stage(impl_->dist_shadow_stage_);

        impl_->dist_shadow_stage_->set_resolution(boost::any_cast<int>(get_setting("dist_shadow_resolution")));
        impl_->dist_shadow_stage_->set_clip_size(boost::any_cast<float>(get_setting("dist_shadow_clipsize")));
        impl_->dist_shadow_stage_->set_sun_distance(boost::any_cast<float>(get_setting("dist_shadow_sundist")));

        impl_->pssm_stage_->get_global_required_pipes().push_back("PSSMDistSunShadowMap");
        impl_->pssm_stage_->get_global_required_inputs().push_back("PSSMDistSunShadowMapMVP");
    }
}

void PSSMPlugin::on_pipeline_created(void)
{
    debug("Initializing pssm ..");

    // Construct a dummy node to parent the rig to
    impl_->node_ = rpcore::Globals::base->get_render().attach_new_node("PSSMCameraRig");
    impl_->node_.hide();

    const int split_count = boost::any_cast<int>(get_setting("split_count"));

    // Construct the actual PSSM rig
    impl_->camera_rig_ = std::make_shared<rpcore::PSSMCameraRig>(split_count);
    impl_->camera_rig_->set_sun_distance(boost::any_cast<float>(get_setting("sun_distance")));
    impl_->camera_rig_->set_pssm_distance(boost::any_cast<float>(get_setting("max_distance")));
    impl_->camera_rig_->set_logarithmic_factor(boost::any_cast<float>(get_setting("logarithmic_factor")));
    impl_->camera_rig_->set_border_bias(boost::any_cast<float>(get_setting("border_bias")));
    impl_->camera_rig_->set_use_stable_csm(true);
    impl_->camera_rig_->set_use_fixed_film_size(true);
    impl_->camera_rig_->set_resolution(boost::any_cast<int>(get_setting("resolution")));
    impl_->camera_rig_->reparent_to(impl_->node_);

    // Attach the cameras to the shadow stage
    for (int i = 0; i < split_count; ++i)
    {
        NodePath camera_np = impl_->camera_rig_->get_camera(i);

        Camera* cam = DCAST(Camera, camera_np.node());

        cam->set_scene(rpcore::Globals::base->get_render());
        PT(DisplayRegion) region = impl_->shadow_stage_->get_split_regions()[i];
        region->set_camera(camera_np);
        // cam->show_frustum();

        // Make sure the pipeline knows about our camera, so it can apply
        // the correct bitmasks
        pipeline_.get_tag_mgr()->register_camera("shadow", cam);
    }

    // Accept a shortcut to enable / disable the update of PSSM
    rpcore::Globals::base->accept("u", Impl::toggle_update_enabled, this);

    // Set inputs
    impl_->pssm_stage_->set_shader_input(ShaderInput("pssm_mvps", impl_->camera_rig_->get_mvp_array()));
    impl_->pssm_stage_->set_shader_input(ShaderInput("pssm_nearfar", impl_->camera_rig_->get_nearfar_array()));

    if (is_plugin_enabled("volumetrics"))
    {
        const auto& stage = std::dynamic_pointer_cast<VolumentricsPlugin>(get_plugin_instance("volumetrics"))->get_stage();
        stage->set_shader_input(ShaderInput("pssm_mvps", impl_->camera_rig_->get_mvp_array()));
        stage->set_shader_input(ShaderInput("pssm_nearfar", impl_->camera_rig_->get_nearfar_array()));
    }
}

void PSSMPlugin::on_pre_render_update(void)
{
    impl_->on_pre_render_update();
}

void PSSMPlugin::request_focus(const LVecBase3f& focus_point, float focus_size)
{
    impl_->scene_shadow_stage_->request_focus(focus_point, focus_size);
}

bool PSSMPlugin::get_last_focus(LVecBase3f& focus_point, float& focus_size) const
{
    const auto& last_focus = impl_->scene_shadow_stage_->get_last_focus();
    if (last_focus)
    {
        focus_point = last_focus.get().first;
        focus_size = last_focus.get().second;
        return true;
    }
    else
    {
        return false;
    }
};

}
