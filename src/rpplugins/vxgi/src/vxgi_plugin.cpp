#include "vxgi_plugin.hpp"

#include <functional>

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rppanda/showbase/showbase.h>
#include <render_pipeline/rplibs/py_to_cpp.hpp>

#include "vxgi_stage.h"
#include "voxelization_stage.h"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Voxel Global Illumination",
    "tobspr <tobias.springer1@gmail.com>",
    "alpha (!)",

    "Provides Global Illumination using Voxel Cone Tracing. This "
    "technique is still very unoptimized and experimental!"
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::VXGIPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

struct VXGIPlugin::Impl
{
    Impl(VXGIPlugin& self);

    void on_pipeline_created(void);

    /** Finds the new voxel grid position. */
    void set_grid_pos(void);

    /** Voxelizes the scene from the x axis. */
    void voxelize_x(void);

    /** Voxelizes the scene from the y axis. */
    void voxelize_y(void);

    /** Voxelizes the scene from the z axis. */
    void voxelize_z(void);

    /** Generates the mipmaps for the voxel grid. */
    void generate_mipmaps(void);

public:
    static RequrieType require_plugins_;

    VXGIPlugin& self_;

    std::shared_ptr<VoxelizationStage> voxel_stage_;
    std::shared_ptr<VXGIStage> vxgi_stage_;

    std::deque<std::function<void(void)>> queue_;
};

VXGIPlugin::RequrieType VXGIPlugin::Impl::require_plugins_ = { "pssm" };

VXGIPlugin::Impl::Impl(VXGIPlugin& self): self_(self)
{
}

void VXGIPlugin::Impl::on_pipeline_created(void)
{
    queue_.push_back(std::bind(&Impl::voxelize_x, this));
    queue_.push_back(std::bind(&Impl::voxelize_y, this));
    queue_.push_back(std::bind(&Impl::voxelize_z, this));
    queue_.push_back(std::bind(&Impl::generate_mipmaps, this));
}

void VXGIPlugin::Impl::set_grid_pos(void)
{
    LPoint3 grid_pos = rpcore::Globals::base->get_cam().get_pos(rpcore::Globals::base->get_render());

    // Snap the voxel grid
    const float voxel_size = 2.0 * boost::any_cast<float>(self_.get_setting("grid_ws_size")) / boost::any_cast<int>(self_.get_setting("grid_resolution"));
    const float snap_size = voxel_size * (2 * 2 * 2 * 2);

    for (int dimension = 0; dimension < 3; ++dimension)
    {
        const float cell_val = grid_pos.get_cell(dimension);
        grid_pos.set_cell(dimension, cell_val - rplibs::py_fmod(cell_val, snap_size));
    }

    voxel_stage_->set_grid_position(grid_pos);
}

void VXGIPlugin::Impl::voxelize_x(void)
{
    set_grid_pos();
    voxel_stage_->set_state(VoxelizationStage::StateType::S_voxelize_x);
}

void VXGIPlugin::Impl::voxelize_y(void)
{
    voxel_stage_->set_state(VoxelizationStage::StateType::S_voxelize_y);
}

void VXGIPlugin::Impl::voxelize_z(void)
{
    voxel_stage_->set_state(VoxelizationStage::StateType::S_voxelize_z);
}

void VXGIPlugin::Impl::generate_mipmaps(void)
{
    voxel_stage_->set_state(VoxelizationStage::StateType::S_gen_mipmaps);
}

// ************************************************************************************************

VXGIPlugin::VXGIPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, plugin_info), impl_(std::make_unique<Impl>(*this))
{
}

VXGIPlugin::RequrieType& VXGIPlugin::get_required_plugins(void) const 
{
    return impl_->require_plugins_;
}

void VXGIPlugin::on_stage_setup(void)
{
    impl_->voxel_stage_ = std::make_shared<VoxelizationStage>(pipeline_);
	add_stage(impl_->voxel_stage_);

    impl_->vxgi_stage_ = std::make_shared<VXGIStage>(pipeline_);
    add_stage(impl_->vxgi_stage_);

    impl_->voxel_stage_->set_voxel_resolution(boost::any_cast<int>(get_setting("grid_resolution")));
    impl_->voxel_stage_->set_voxel_world_size(boost::any_cast<float>(get_setting("grid_ws_size")));

    if (is_plugin_enabled("pssm"))
    {
        // Add shadow map as requirement
        impl_->voxel_stage_->get_required_pipes().push_back("PSSMSceneSunShadowMapPCF");
        impl_->voxel_stage_->get_required_inputs().push_back("PSSMSceneSunShadowMVP");
    }
}

void VXGIPlugin::on_pre_render_update(void)
{
    auto task = impl_->queue_.front();
    impl_->queue_.pop_front();
    impl_->queue_.push_back(task);
    task();
}

void VXGIPlugin::on_pipeline_created(void)
{
    impl_->on_pipeline_created();
}

}
