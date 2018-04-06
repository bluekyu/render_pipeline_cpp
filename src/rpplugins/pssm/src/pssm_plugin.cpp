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

#include "pssm_stage.hpp"
#include "pssm_shadow_stage.hpp"
#include "pssm_scene_shadow_stage.hpp"
#include "pssm_dist_shadow_stage.hpp"

#include "scattering_plugin.hpp"
#include "volumetrics_plugin.hpp"
#include "volumetrics_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::PSSMPlugin)

namespace rpplugins {

class PSSMPlugin::Impl
{
public:
    void toggle_update_enabled();

    Impl(PSSMPlugin& self);

    void on_pre_render_update();

public:
    static RequrieType require_plugins_;

    PSSMPlugin& self_;

    bool update_enabled_;
    PTA_LVecBase3f pta_sun_vector_;
    int last_cache_reset_;

    PSSMShadowStage* shadow_stage_;
    PSSMStage* pssm_stage_;
    PSSMSceneShadowStage* scene_shadow_stage_;
    PSSMDistShadowStage* dist_shadow_stage_;

    NodePath node_;
    std::unique_ptr<rpcore::PSSMCameraRig> camera_rig_;
};

PSSMPlugin::RequrieType PSSMPlugin::Impl::require_plugins_ = { "scattering" };

PSSMPlugin::Impl::Impl(PSSMPlugin& self): self_(self)
{
}

void PSSMPlugin::Impl::toggle_update_enabled()
{
    update_enabled_ = !update_enabled_;
    self_.debug(std::string("Update enabled: ") + (update_enabled_ ? "True" : "False"));
}

void PSSMPlugin::Impl::on_pre_render_update()
{
    const LVecBase3f& sun_vector = dynamic_cast<rpplugins::ScatteringPlugin*>(self_.get_plugin_instance("scattering"))->get_sun_vector();

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

PSSMPlugin::RequrieType& PSSMPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void PSSMPlugin::on_stage_setup()
{
    impl_->update_enabled_ = true;
    impl_->pta_sun_vector_ = PTA_LVecBase3f::empty_array(1);
    impl_->last_cache_reset_ = 0;

    auto shadow_stage = std::make_unique<PSSMShadowStage>(pipeline_);
    impl_->shadow_stage_ = shadow_stage.get();
    add_stage(std::move(shadow_stage));

    auto pssm_stage = std::make_unique<PSSMStage>(pipeline_);
    impl_->pssm_stage_ = pssm_stage.get();
    add_stage(std::move(pssm_stage));

    impl_->shadow_stage_->set_num_splits(boost::any_cast<int>(get_setting("split_count")));
    impl_->shadow_stage_->set_split_resolution(boost::any_cast<int>(get_setting("resolution")));

    auto scene_shadow_stage = std::make_unique<PSSMSceneShadowStage>(pipeline_);
    impl_->scene_shadow_stage_ = scene_shadow_stage.get();
    add_stage(std::move(scene_shadow_stage));

    impl_->scene_shadow_stage_->set_resolution(boost::any_cast<int>(get_setting("scene_shadow_resolution")));
    impl_->scene_shadow_stage_->set_sun_distance(boost::any_cast<float>(get_setting("scene_shadow_sundist")));

    if (boost::any_cast<bool>(get_setting("use_distant_shadows")))
    {
        auto dist_shadow_stage = std::make_unique<PSSMDistShadowStage>(pipeline_);
        dist_shadow_stage->set_resolution(boost::any_cast<int>(get_setting("dist_shadow_resolution")));
        dist_shadow_stage->set_clip_size(boost::any_cast<float>(get_setting("dist_shadow_clipsize")));
        dist_shadow_stage->set_sun_distance(boost::any_cast<float>(get_setting("dist_shadow_sundist")));

        impl_->dist_shadow_stage_ = dist_shadow_stage.get();
        add_stage(std::move(dist_shadow_stage));

        impl_->pssm_stage_->get_global_required_pipes().push_back("PSSMDistSunShadowMap");
        impl_->pssm_stage_->get_global_required_inputs().push_back("PSSMDistSunShadowMapMVP");
    }
}

void PSSMPlugin::on_pipeline_created()
{
    debug("Initializing pssm ..");

    // Construct a dummy node to parent the rig to
    impl_->node_ = rpcore::Globals::base->get_render().attach_new_node("PSSMCameraRig");
    impl_->node_.hide();

    const int split_count = boost::any_cast<int>(get_setting("split_count"));

    // Construct the actual PSSM rig
    impl_->camera_rig_ = std::make_unique<rpcore::PSSMCameraRig>(split_count);
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
    rpcore::Globals::base->accept("u", [this](const Event*) { impl_->toggle_update_enabled(); });

    // Set inputs
    impl_->pssm_stage_->set_shader_input(ShaderInput("pssm_mvps", impl_->camera_rig_->get_mvp_array()));
    impl_->pssm_stage_->set_shader_input(ShaderInput("pssm_nearfar", impl_->camera_rig_->get_nearfar_array()));

    if (is_plugin_enabled("volumetrics"))
    {
        auto stage = dynamic_cast<VolumentricsPlugin*>(get_plugin_instance("volumetrics"))->get_stage();
        stage->set_shader_input(ShaderInput("pssm_mvps", impl_->camera_rig_->get_mvp_array()));
        stage->set_shader_input(ShaderInput("pssm_nearfar", impl_->camera_rig_->get_nearfar_array()));
    }
}

void PSSMPlugin::on_pre_render_update()
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
        focus_point = last_focus.value().first;
        focus_size = last_focus.value().second;
        return true;
    }
    else
    {
        return false;
    }
};

}
