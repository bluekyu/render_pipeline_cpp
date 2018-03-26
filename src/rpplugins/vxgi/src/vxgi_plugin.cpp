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

#include "vxgi_plugin.hpp"

#include <functional>

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rplibs/py_to_cpp.hpp>

#include "vxgi_stage.hpp"
#include "voxelization_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::VXGIPlugin)

namespace rpplugins {

class VXGIPlugin::Impl
{
public:
    Impl(VXGIPlugin& self);

    void on_pipeline_created();

    /** Finds the new voxel grid position. */
    void set_grid_pos();

    /** Voxelizes the scene from the x axis. */
    void voxelize_x();

    /** Voxelizes the scene from the y axis. */
    void voxelize_y();

    /** Voxelizes the scene from the z axis. */
    void voxelize_z();

    /** Generates the mipmaps for the voxel grid. */
    void generate_mipmaps();

public:
    static RequrieType require_plugins_;

    VXGIPlugin& self_;

    VoxelizationStage* voxel_stage_;

    std::deque<std::function<void()>> queue_;
};

VXGIPlugin::RequrieType VXGIPlugin::Impl::require_plugins_ = { "pssm" };

VXGIPlugin::Impl::Impl(VXGIPlugin& self): self_(self)
{
}

void VXGIPlugin::Impl::on_pipeline_created()
{
    queue_.push_back(std::bind(&Impl::voxelize_x, this));
    queue_.push_back(std::bind(&Impl::voxelize_y, this));
    queue_.push_back(std::bind(&Impl::voxelize_z, this));
    queue_.push_back(std::bind(&Impl::generate_mipmaps, this));
}

void VXGIPlugin::Impl::set_grid_pos()
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

void VXGIPlugin::Impl::voxelize_x()
{
    set_grid_pos();
    voxel_stage_->set_state(VoxelizationStage::StateType::S_voxelize_x);
}

void VXGIPlugin::Impl::voxelize_y()
{
    voxel_stage_->set_state(VoxelizationStage::StateType::S_voxelize_y);
}

void VXGIPlugin::Impl::voxelize_z()
{
    voxel_stage_->set_state(VoxelizationStage::StateType::S_voxelize_z);
}

void VXGIPlugin::Impl::generate_mipmaps()
{
    voxel_stage_->set_state(VoxelizationStage::StateType::S_gen_mipmaps);
}

// ************************************************************************************************

VXGIPlugin::VXGIPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>(*this))
{
}

VXGIPlugin::RequrieType& VXGIPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void VXGIPlugin::on_stage_setup()
{
    auto voxel_stage = std::make_unique<VoxelizationStage>(pipeline_);
    impl_->voxel_stage_ = voxel_stage.get();
    add_stage(std::move(voxel_stage));

    add_stage(std::make_unique<VXGIStage>(pipeline_));

    impl_->voxel_stage_->set_voxel_resolution(boost::any_cast<int>(get_setting("grid_resolution")));
    impl_->voxel_stage_->set_voxel_world_size(boost::any_cast<float>(get_setting("grid_ws_size")));

    if (is_plugin_enabled("pssm"))
    {
        // Add shadow map as requirement
        impl_->voxel_stage_->get_required_pipes().push_back("PSSMSceneSunShadowMapPCF");
        impl_->voxel_stage_->get_required_inputs().push_back("PSSMSceneSunShadowMVP");
    }
}

void VXGIPlugin::on_pre_render_update()
{
    auto task = impl_->queue_.front();
    impl_->queue_.pop_front();
    impl_->queue_.push_back(task);
    task();
}

void VXGIPlugin::on_pipeline_created()
{
    impl_->on_pipeline_created();
}

}
