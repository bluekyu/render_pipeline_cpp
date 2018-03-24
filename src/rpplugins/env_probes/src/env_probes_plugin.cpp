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

#include "../include/env_probes_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <displayRegion.h>
#include <nodePathCollection.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/stage_manager.hpp>
#include <render_pipeline/rpcore/util/task_scheduler.hpp>
#include <render_pipeline/rpcore/util/shader_input_blocks.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/stages/cull_lights_stage.hpp>

#include "probe_manager.hpp"
#include "environment_capture_stage.hpp"
#include "cull_probes_stage.hpp"
#include "apply_envprobes_stage.hpp"
#include "environment_probe.hpp"
#include "pssm_plugin.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::EnvProbesPlugin)

namespace rpplugins {

class EnvProbesPlugin::Impl
{
public:
    Impl(EnvProbesPlugin& self);

    void on_stage_setup();

    /** Setups all stages. */
    void setup_stages();

    /** Sets all required inputs. */
    void setup_inputs();

public:
    static RequrieType require_plugins_;

    EnvProbesPlugin& self_;
    std::unique_ptr<ProbeManager> probe_mgr_;
    PTA_int pta_probes_;
    std::shared_ptr<rpcore::SimpleInputBlock> data_ubo_;

    std::shared_ptr<EnvironmentCaptureStage> capture_stage_;
    std::shared_ptr<CullProbesStage> cull_stage_;
    std::shared_ptr<ApplyEnvprobesStage> apply_stage_;
};

EnvProbesPlugin::RequrieType EnvProbesPlugin::Impl::require_plugins_;

EnvProbesPlugin::Impl::Impl(EnvProbesPlugin& self): self_(self)
{
}

EnvProbesPlugin::EnvProbesPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>(*this))
{
}

EnvProbesPlugin::RequrieType& EnvProbesPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void EnvProbesPlugin::Impl::on_stage_setup()
{
    probe_mgr_ = std::make_unique<ProbeManager>();
    probe_mgr_->set_resolution(boost::any_cast<int>(self_.get_setting("probe_resolution")));
    probe_mgr_->set_diffuse_resolution(boost::any_cast<int>(self_.get_setting("diffuse_probe_resolution")));
    probe_mgr_->set_max_probes(boost::any_cast<int>(self_.get_setting("max_probes")));
    probe_mgr_->init();

    setup_stages();
}

void EnvProbesPlugin::Impl::setup_stages()
{
    // Create the stage to generate and update the cubemaps
    capture_stage_ = std::make_shared<EnvironmentCaptureStage>(self_.pipeline_);
    self_.add_stage(capture_stage_);
    capture_stage_->set_resolution(probe_mgr_->get_resolution());
    capture_stage_->set_diffuse_resolution(probe_mgr_->get_diffuse_resolution());
    capture_stage_->set_storage_tex(probe_mgr_->get_cubemap_storage()->get_texture());
    capture_stage_->set_storage_tex_diffuse(probe_mgr_->get_diffuse_storage()->get_texture());

    // Create the stage to cull the cubemaps
    cull_stage_ = std::make_shared<CullProbesStage>(self_.pipeline_);
    self_.add_stage(cull_stage_);

    // Create the stage to apply the cubemaps
    apply_stage_ = std::make_shared<ApplyEnvprobesStage>(self_.pipeline_);
    self_.add_stage(apply_stage_);

    if (self_.is_plugin_enabled("scattering"))
    {
        capture_stage_->get_required_pipes().push_back("ScatteringIBLSpecular");
        capture_stage_->get_required_pipes().push_back("ScatteringIBLDiffuse");
    }

    if (self_.is_plugin_enabled("pssm"))
    {
        capture_stage_->get_required_pipes().push_back("PSSMSceneSunShadowMapPCF");
        capture_stage_->get_required_inputs().push_back("PSSMSceneSunShadowMVP");
    }

    setup_inputs();
}

void EnvProbesPlugin::Impl::setup_inputs()
{
    pta_probes_ = PTA_int::empty_array(1);

    // Construct the UBO which stores all environment probe data
    data_ubo_ = std::make_shared<rpcore::SimpleInputBlock>("EnvProbes");
    data_ubo_->add_input("num_probes", pta_probes_);
    data_ubo_->add_input("cubemaps", probe_mgr_->get_cubemap_storage()->get_texture());
    data_ubo_->add_input("diffuse_cubemaps", probe_mgr_->get_diffuse_storage()->get_texture());
    data_ubo_->add_input("dataset", probe_mgr_->get_dataset_storage()->get_texture());
    self_.pipeline_.get_stage_mgr()->add_input_blocks(data_ubo_);

    // Use the UBO in light culling
    rpcore::CullLightsStage::get_global_required_inputs().push_back("EnvProbes");
}

// ************************************************************************************************

void EnvProbesPlugin::on_stage_setup()
{
    impl_->on_stage_setup();
}

void EnvProbesPlugin::on_prepare_scene(NodePath scene)
{
    const NodePathCollection& ep_npc = scene.find_all_matches("**/ENVPROBE*");
    for (int k=0, k_end=ep_npc.get_num_paths(); k < k_end; ++k)
    {
        auto probe = std::make_shared<EnvironmentProbe>();
        impl_->probe_mgr_->add_probe(probe);

        if (probe)
        {
            probe->set_mat(ep_npc.get_path(k).get_mat());
            probe->set_border_smoothness(0.0001f);
            probe->set_parallax_correction(true);
            ep_npc.get_path(k).remove_node();
        }
    }
}

void EnvProbesPlugin::on_pre_render_update()
{
    if (pipeline_.get_task_scheduler()->is_scheduled("envprobes_select_and_cull"))
    {
        impl_->probe_mgr_->update();
        impl_->pta_probes_[0] = impl_->probe_mgr_->get_num_probes();
        const auto& probe = impl_->probe_mgr_->find_probe_to_update();

        if (probe)
        {
            probe->set_last_update(rpcore::Globals::clock->get_frame_count());
            impl_->capture_stage_->set_active(true);
            impl_->capture_stage_->set_probe(probe);

            if (is_plugin_enabled("pssm"))
            {
                dynamic_cast<PSSMPlugin*>(get_plugin_instance("pssm"))->request_focus(probe->get_bounds()->get_center(), probe->get_bounds()->get_radius());
            }
        }
        else
        {
            impl_->capture_stage_->set_active(false);
        }
    }
}

std::shared_ptr<rpcore::RenderStage> EnvProbesPlugin::get_capture_stage()
{
    return impl_->capture_stage_;
}

}
