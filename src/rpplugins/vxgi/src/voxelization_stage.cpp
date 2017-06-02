#include "voxelization_stage.h"

#include <cullFaceAttrib.h>
#include <depthTestAttrib.h>

#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rpplugins {

VoxelizationStage::RequireType VoxelizationStage::required_inputs = { "DefaultEnvmap", "AllLightsData", "maxLightIndex" };
VoxelizationStage::RequireType VoxelizationStage::required_pipes;

VoxelizationStage::ProduceType VoxelizationStage::get_produced_inputs(void) const
{
    return {
        ShaderInput("voxelGridPosition", _pta_grid_pos),
    };
}

VoxelizationStage::ProduceType VoxelizationStage::get_produced_pipes(void) const
{
	return {
        ShaderInput("SceneVoxels", _voxel_grid->get_texture()),
	};
}

void VoxelizationStage::create_ptas(void)
{
    _pta_next_grid_pos = PTA_LVecBase3::empty_array(1);
    _pta_grid_pos = PTA_LVecBase3::empty_array(1);
}

void VoxelizationStage::create(void)
{
    // Create the voxel grid used to generate the voxels
    _voxel_temp_grid = rpcore::Image::create_3d("VoxelsTemp", _voxel_resolution, _voxel_resolution, _voxel_resolution, "RGBA8");
    _voxel_temp_grid->set_clear_color(LColor(0));
    _voxel_temp_nrm_grid = rpcore::Image::create_3d("VoxelsTemp", _voxel_resolution, _voxel_resolution, _voxel_resolution, "R11G11B10");
    _voxel_temp_nrm_grid->set_clear_color(LColor(0));

    // Create the voxel grid which is a copy of the temporary grid, but stable
    _voxel_grid = rpcore::Image::create_3d("Voxels", _voxel_resolution, _voxel_resolution, _voxel_resolution, "RGBA8");
    _voxel_grid->set_clear_color(LColor(0));
    _voxel_grid->set_minfilter(SamplerState::FT_linear_mipmap_linear);

    // Create the camera for voxelization
    _voxel_cam = new Camera("VoxelizeCam");
    _voxel_cam->set_camera_mask(pipeline_.get_tag_mgr()->get_mask("voxelize"));
    _voxel_cam_lens = new OrthographicLens;
    _voxel_cam_lens->set_film_size(-2.0 * _voxel_world_size, 2.0 * _voxel_world_size);
    _voxel_cam_lens->set_near_far(0.0, 2.0 * _voxel_world_size);
    _voxel_cam->set_lens(_voxel_cam_lens);
    _voxel_cam_np = rpcore::Globals::base->get_render().attach_new_node(_voxel_cam);
    pipeline_.get_tag_mgr()->register_camera("voxelize", _voxel_cam);

    // Create the voxelization target
    _voxel_target = create_target("VoxelizeScene");
    _voxel_target->set_size(_voxel_resolution);
    _voxel_target->prepare_render(_voxel_cam_np);

    // Create the target which copies the voxel grid
    _copy_target = create_target("CopyVoxels");
    _copy_target->set_size(_voxel_resolution);
    _copy_target->prepare_buffer();

    // TODO! Does not work with the new render target yet - maybe add option
    // to post process region for instances ?
    _copy_target->set_instance_count(_voxel_resolution);
    _copy_target->set_shader_input(ShaderInput("SourceTex", _voxel_temp_grid->get_texture()));
    _copy_target->set_shader_input(ShaderInput("DestTex", _voxel_grid->get_texture()));

    // Create the target which generates the mipmaps
    _mip_targets.clear();
    int mip_size = _voxel_resolution;
    int mip = 0;
    while (mip_size > 1)
    {
        mip_size = mip_size / 2;
        mip = mip + 1;

        auto mip_target = create_target(std::string("GenMipmaps:") + std::to_string(mip));
        mip_target->set_size(mip_size);
        mip_target->prepare_buffer();
        mip_target->set_instance_count(mip_size);
        mip_target->set_shader_input(ShaderInput("SourceTex", _voxel_grid->get_texture()));
        mip_target->set_shader_input(ShaderInput("sourceMip", LVecBase4i(mip - 1, 0, 0, 0)));
        mip_target->set_shader_input(ShaderInput("DestTex", _voxel_grid->get_texture(), false, true, -1, mip, 0));
        _mip_targets.push_back(mip_target);
    }

    // Create the initial state used for rendering voxels
    NodePath initial_state("VXGIInitialState");
    initial_state.set_attrib(CullFaceAttrib::make(CullFaceAttrib::M_cull_none), 100000);
    initial_state.set_attrib(DepthTestAttrib::make(DepthTestAttrib::M_none), 100000);
    initial_state.set_attrib(ColorWriteAttrib::make(ColorWriteAttrib::C_off), 100000);
    _voxel_cam->set_initial_state(initial_state.get_state());

    rpcore::Globals::base->get_render().set_shader_input("voxelGridPosition", _pta_next_grid_pos);
    rpcore::Globals::base->get_render().set_shader_input("VoxelGridDest", _voxel_temp_grid->get_texture());
}

void VoxelizationStage::update(void)
{
    _voxel_cam_np.show();
    _voxel_target->set_active(true);
    _copy_target->set_active(false);

    for (auto& target: _mip_targets)
        target->set_active(false);

    switch (_state)
    {
        // Voxelization disable
        case StateType::S_disabled:
        {
            _voxel_cam_np.hide();
            _voxel_target->set_active(false);
            break;
        }

        // Voxelization from X-Axis
        case StateType::S_voxelize_x:
        {
            // Clear voxel grid
            _voxel_temp_grid->clear_image();
            _voxel_cam_np.set_pos(_pta_next_grid_pos[0] + LVecBase3(_voxel_world_size, 0, 0));
            _voxel_cam_np.look_at(_pta_next_grid_pos[0]);
            break;
        }

        // Voxelization from Y-Axis
        case StateType::S_voxelize_y:
        {
            _voxel_cam_np.set_pos(_pta_next_grid_pos[0] + LVecBase3(0, _voxel_world_size, 0));
            _voxel_cam_np.look_at(_pta_next_grid_pos[0]);
            break;
        }

        // Voxelization from Z-Axis
        case StateType::S_voxelize_z:
        {
            _voxel_cam_np.set_pos(_pta_next_grid_pos[0] + LVecBase3(0, 0, _voxel_world_size));
            _voxel_cam_np.look_at(_pta_next_grid_pos[0]);
            break;
        }

        // Generate mipmaps
        case StateType::S_gen_mipmaps:
        {
            _voxel_target->set_active(false);
            _copy_target->set_active(true);
            _voxel_cam_np.hide();

            for (auto& target: _mip_targets)
                target->set_active(true);

            // As soon as we generate the mipmaps, we need to update the grid position as well
            _pta_grid_pos[0] = _pta_next_grid_pos[0];
            break;
        }
    }
}

void VoxelizationStage::reload_shaders(void)
{
	_copy_target->set_shader(load_plugin_shader({ "/$$rp/shader/default_post_process_instanced.vert.glsl", "copy_voxels.frag.glsl" }));

    PT(Shader) mip_shader = load_plugin_shader({"/$$rp/shader/default_post_process_instanced.vert.glsl", "generate_mipmaps.frag.glsl"});
    for (auto& target: _mip_targets)
        target->set_shader(mip_shader);
}

void VoxelizationStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

std::string VoxelizationStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
