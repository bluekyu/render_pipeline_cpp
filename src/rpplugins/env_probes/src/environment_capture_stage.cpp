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

#include "environment_capture_stage.hpp"

#include <graphicsBuffer.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/util/task_scheduler.hpp>

#include "environment_probe.hpp"

namespace rpplugins {

EnvironmentCaptureStage::RequireType EnvironmentCaptureStage::required_inputs = { "DefaultEnvmap", "AllLightsData", "maxLightIndex" };
EnvironmentCaptureStage::RequireType EnvironmentCaptureStage::required_pipes;

EnvironmentCaptureStage::EnvironmentCaptureStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "EnvironmentCaptureStage")
{
    rig_node = rpcore::Globals::render.attach_new_node("EnvmapCamRig");
    _pta_index = PTA_int::empty_array(1);
}

void EnvironmentCaptureStage::create()
{
    _target = create_target("CaptureScene");
    _target->set_size(_resolution * 6, _resolution);
    _target->add_depth_attachment(16);
    _target->add_color_attachment(16, true);
    _target->prepare_render(NodePath());

    // Remove all unused display regions
    GraphicsBuffer* internal_buffer = _target->get_internal_buffer();
    internal_buffer->remove_all_display_regions();
    internal_buffer->disable_clears();
    internal_buffer->get_overlay_display_region()->disable_clears();

    setup_camera_rig();
    create_store_targets();
    create_filter_targets();
}

void EnvironmentCaptureStage::setup_camera_rig()
{
    const LVecBase3f directions[6] = {
        LVecBase3f(1, 0, 0), LVecBase3f(-1, 0, 0), LVecBase3f(0, 1, 0),
        LVecBase3f(0, -1, 0), LVecBase3f(0, 0, 1), LVecBase3f(0, 0, -1),};

    // Prepare the display regions
    for (size_t i = 0; i < std::extent<decltype(directions)>::value; ++i)
    {
        DisplayRegion* region = _target->get_internal_buffer()->make_display_region(
            i / 6.0f, i / 6.0f + 1 / 6.0f, 0.0f, 1.0f);
        region->set_sort(25 + i);
        region->set_active(true);
        region->disable_clears();

        // Set the correct clears
        region->set_clear_depth_active(true);
        region->set_clear_depth(1.0f);
        region->set_clear_color_active(true);
        region->set_clear_color(LColorf(0));

        PT(PerspectiveLens) lens = new PerspectiveLens;
        lens->set_fov(90.0f);
        lens->set_near_far(0.001f, 1.0f);
        PT(Camera) camera = new Camera(std::string("EnvmapCam-") + std::to_string(i), lens);
        NodePath camera_np = rig_node.attach_new_node(camera);
        camera_np.look_at(camera_np, directions[i]);
        region->set_camera(camera_np);
        regions.push_back(region);
        cameras.push_back(camera_np);
    }

    cameras[0].set_r(90);
    cameras[1].set_r(-90);
    cameras[3].set_r(180);
    cameras[5].set_r(180);

    // Register cameras
    for (NodePath& camera_np :cameras)
        pipeline_.get_tag_mgr()->register_camera("envmap", DCAST(Camera, camera_np.node()));
}

void EnvironmentCaptureStage::create_store_targets()
{
    _target_store = create_target("StoreCubemap");
    _target_store->set_size(_resolution * 6, _resolution);
    _target_store->prepare_buffer();
    _target_store->set_shader_input(ShaderInput("SourceTex", _target->get_color_tex()));
    _target_store->set_shader_input(ShaderInput("DestTex", _storage_tex));
    _target_store->set_shader_input(ShaderInput("currentIndex", _pta_index));

    _temporary_diffuse_map = rpcore::Image::create_cube("DiffuseTemp", _resolution, "RGBA16");
    _target_store_diff = create_target("StoreCubemapDiffuse");
    _target_store_diff->set_size(_resolution * 6, _resolution);
    _target_store_diff->prepare_buffer();
    _target_store_diff->set_shader_input(ShaderInput("SourceTex", _target->get_color_tex()));
    _target_store_diff->set_shader_input(ShaderInput("DestTex", _temporary_diffuse_map->get_texture()));
    _target_store_diff->set_shader_input(ShaderInput("currentIndex", _pta_index));
}

void EnvironmentCaptureStage::create_filter_targets()
{
    int mip = 0;
    int size = _resolution;
    while (size > 1)
    {
        size = size / 2;
        mip += 1;
        const std::string string_size(std::to_string(size));
        auto target = create_target(std::string("FilterCubemap:") + std::to_string(mip) + "-" + string_size + "x" + string_size);
        target->set_size(size * 6, size);
        target->prepare_buffer();
        target->set_shader_input(ShaderInput("currentIndex", _pta_index));
        target->set_shader_input(ShaderInput("currentMip", LVecBase4i(mip, 0, 0, 0)));
        target->set_shader_input(ShaderInput("SourceTex", _storage_tex));
        target->set_shader_input(ShaderInput("DestTex", _storage_tex, false, true, -1, mip, 0));
        _filter_targets.push_back(target);
    }

    // Target to filter the diffuse cubemap
    _filter_diffuse_target = create_target("FilterCubemapDiffuse");
    _filter_diffuse_target->set_size(_diffuse_resolution * 6, _diffuse_resolution);
    _filter_diffuse_target->prepare_buffer();
    _filter_diffuse_target->set_shader_input(ShaderInput("SourceTex", _temporary_diffuse_map->get_texture()));
    _filter_diffuse_target->set_shader_input(ShaderInput("DestTex", _storage_tex_diffuse));
    _filter_diffuse_target->set_shader_input(ShaderInput("currentIndex", _pta_index));
}

void EnvironmentCaptureStage::update()
{
    // First, disable all targets
    for (auto&& id_target: targets_)
        id_target.second->set_active(false);

    // Check for updated faces
    for (size_t i = 0, i_end=regions.size(); i < i_end; ++i)
        if (pipeline_.get_task_scheduler()->is_scheduled(std::string("envprobes_capture_envmap_face") + std::to_string(i)))
            regions[i]->set_active(true);

    // Check for filtering
    if (pipeline_.get_task_scheduler()->is_scheduled("envprobes_filter_and_store_envmap"))
    {
        _target_store->set_active(true);
        _target_store_diff->set_active(true);
        _filter_diffuse_target->set_active(true);

        for (auto&& target: _filter_targets)
            target->set_active(true);
    }
}

void EnvironmentCaptureStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

void EnvironmentCaptureStage::set_probe(EnvironmentProbe* probe)
{
    rig_node.set_mat(probe->get_matrix());
    _pta_index[0] = probe->get_index();
}

void EnvironmentCaptureStage::reload_shaders()
{
    _target_store->set_shader(load_plugin_shader({"store_cubemap.frag.glsl"}));
    _target_store_diff->set_shader(load_plugin_shader({"store_cubemap_diffuse.frag.glsl"}));
    _filter_diffuse_target->set_shader(load_plugin_shader({"filter_cubemap_diffuse.frag.glsl"}));

    for (size_t i=0, i_end=_filter_targets.size(); i < i_end; ++i)
        _filter_targets[i]->set_shader(load_plugin_shader({std::string("mips/") + std::to_string(i) + ".autogen.glsl"}));
}

std::string EnvironmentCaptureStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
