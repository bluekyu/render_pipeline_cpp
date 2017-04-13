#pragma once

#include <camera.h>
#include <orthographicLens.h>

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpplugins {

/** This stage voxelizes the whole scene */
class VoxelizationStage: public rpcore::RenderStage
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

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_inputs(void) const final;
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void update(void) final;
    void reload_shaders(void) final;

    void set_shader_input(const ShaderInput& inp) final;

    void create_ptas(void);

    void set_voxel_resolution(int voxel_resolution);
    void set_voxel_world_size(float voxel_world_size);

    void set_grid_position(const LPoint3& pos);

    void set_state(StateType state);

private:
    std::string get_plugin_id(void) const final;

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

    std::shared_ptr<rpcore::RenderTarget> _voxel_target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _copy_target = nullptr;
    std::vector<std::shared_ptr<rpcore::RenderTarget>> _mip_targets;
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
