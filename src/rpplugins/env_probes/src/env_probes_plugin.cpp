#include "../include/env_probes_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <nodePathCollection.h>

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/stage_manager.h>
#include <render_pipeline/rpcore/util/task_scheduler.h>
#include <render_pipeline/rpcore/util/shader_input_blocks.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/stages/cull_lights_stage.h>

#include "probe_manager.h"
#include "environment_capture_stage.h"
#include "cull_probes_stage.h"
#include "apply_envprobes_stage.h"
#include "environment_probe.h"
#include "../../pssm/include/pssm_plugin.hpp"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Environment Probes",
    "tobspr <tobias.springer1@gmail.com>",
    "beta (!)",

    "This plugin adds support for environment probes, containing "
    "diffuse and specular information. This enables accurate "
    "reflections, and can also be used to simulate GI."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::EnvProbesPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

struct EnvProbesPlugin::Impl
{
    Impl(EnvProbesPlugin& self);
    ~Impl(void);

    void on_stage_setup(void);

    /** Setups all stages. */
    void setup_stages(void);

    /** Sets all required inputs. */
    void setup_inputs(void);

public:
    static RequrieType require_plugins_;

    EnvProbesPlugin& self_;
    ProbeManager* probe_mgr_ = nullptr;
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

EnvProbesPlugin::Impl::~Impl(void)
{
    delete probe_mgr_;
}

EnvProbesPlugin::EnvProbesPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info), impl_(std::make_unique<Impl>(*this))
{
}

EnvProbesPlugin::RequrieType& EnvProbesPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void EnvProbesPlugin::Impl::on_stage_setup(void)
{
    probe_mgr_ = new ProbeManager;
    probe_mgr_->set_resolution(boost::any_cast<int>(self_.get_setting("probe_resolution")));
    probe_mgr_->set_diffuse_resolution(boost::any_cast<int>(self_.get_setting("diffuse_probe_resolution")));
    probe_mgr_->set_max_probes(boost::any_cast<int>(self_.get_setting("max_probes")));
    probe_mgr_->init();

    setup_stages();
}

void EnvProbesPlugin::Impl::setup_stages(void)
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

void EnvProbesPlugin::Impl::setup_inputs(void)
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

void EnvProbesPlugin::on_stage_setup(void)
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

void EnvProbesPlugin::on_pre_render_update(void)
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
                std::dynamic_pointer_cast<PSSMPlugin>(get_plugin_instance("pssm"))->request_focus(probe->get_bounds()->get_center(), probe->get_bounds()->get_radius());
            }
        }
        else
        {
            impl_->capture_stage_->set_active(false);
        }
    }
}

std::shared_ptr<rpcore::RenderStage> EnvProbesPlugin::get_capture_stage(void)
{
    return impl_->capture_stage_;
}

}
