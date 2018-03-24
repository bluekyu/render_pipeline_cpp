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

#pragma once

#include <camera.h>
#include <orthographicLens.h>

#include <render_pipeline/rpcore/render_stage.hpp>
#include <render_pipeline/rpcore/image.hpp>

namespace rpplugins {

/** This stage voxelizes the whole scene */
class VoxelizationStage : public rpcore::RenderStage
{
public:
    // The different states of voxelization
    enum class StateType
    {
        S_disabled = 0,
        S_voxelize_x = 1,
        S_voxelize_y = 2,
        S_voxelize_z = 3,
        S_gen_mipmaps = 4,
    };

    VoxelizationStage(rpcore::RenderPipeline& pipeline);

    RequireType& get_required_inputs() const final { return required_inputs; }
    RequireType& get_required_pipes() const final { return required_pipes; }
    ProduceType get_produced_inputs() const final;
    ProduceType get_produced_pipes() const final;

    void create() final;
    void update() final;
    void reload_shaders() final;

    void set_shader_input(const ShaderInput& inp) final;

    void create_ptas();

    void set_voxel_resolution(int voxel_resolution);
    void set_voxel_world_size(float voxel_world_size);

    void set_grid_position(const LPoint3& pos);

    void set_state(StateType state);

private:
    std::string get_plugin_id() const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    int _voxel_resolution = 256;
    float _voxel_world_size = -1;
    StateType _state = StateType::S_disabled;

    PTA_LVecBase3 _pta_next_grid_pos;
    PTA_LVecBase3 _pta_grid_pos;

    std::shared_ptr<rpcore::Image> _voxel_temp_grid;
    std::shared_ptr<rpcore::Image> _voxel_temp_nrm_grid;
    std::shared_ptr<rpcore::Image> _voxel_grid;

    PT(Camera) _voxel_cam;
    PT(OrthographicLens) _voxel_cam_lens;
    NodePath _voxel_cam_np;

    rpcore::RenderTarget* _voxel_target;
    rpcore::RenderTarget* _copy_target;
    std::vector<rpcore::RenderTarget*> _mip_targets;
};

inline VoxelizationStage::VoxelizationStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "VoxelizationStage")
{
    create_ptas();
}

inline void VoxelizationStage::set_voxel_resolution(int voxel_resolution)
{
    _voxel_resolution = voxel_resolution;
}

inline void VoxelizationStage::set_voxel_world_size(float voxel_world_size)
{
    _voxel_world_size = voxel_world_size;
}

inline void VoxelizationStage::set_grid_position(const LPoint3& pos)
{
    _pta_next_grid_pos[0] = pos;
}

inline void VoxelizationStage::set_state(StateType state)
{
    _state = state;
}

}
