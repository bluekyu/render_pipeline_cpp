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

#include "motion_blur_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

MotionBlurStage::RequireType MotionBlurStage::required_inputs;
MotionBlurStage::RequireType MotionBlurStage::required_pipes = { "ShadedScene", "GBuffer", "DownscaledDepth", "CombinedVelocity" };

MotionBlurStage::ProduceType MotionBlurStage::get_produced_pipes() const
{
    return {
        ShaderInput("ShadedScene", _target_cam_blur->get_color_tex()),
    };
}

void MotionBlurStage::create()
{
    if (per_object_blur_)
    {
        _tile_target = create_target("FetchVertDominantVelocity");
        _tile_target->set_size(-1, -_tile_size);
        _tile_target->add_color_attachment(LVecBase3i(16, 16, 0));
        _tile_target->prepare_buffer();

        _tile_target_horiz = create_target("FetchHorizDominantVelocity");
        _tile_target_horiz->set_size(-_tile_size);
        _tile_target_horiz->add_color_attachment(LVecBase3i(16, 16, 0));
        _tile_target_horiz->prepare_buffer();
        _tile_target_horiz->set_shader_input(ShaderInput("SourceTex", _tile_target->get_color_tex()));

        _minmax_target = create_target("NeighborMinMax");
        _minmax_target->set_size(-_tile_size);
        _minmax_target->add_color_attachment(LVecBase3i(16, 16, 0));
        _minmax_target->prepare_buffer();
        _minmax_target->set_shader_input(ShaderInput("TileMinMax", _tile_target_horiz->get_color_tex()));

        _pack_target = create_target("PackBlurData");
        _pack_target->add_color_attachment(LVecBase3i(16, 16, 0));
        _pack_target->prepare_buffer();

        _target = create_target("ObjectMotionBlur");
        _target->add_color_attachment(16);
        _target->prepare_buffer();
        _target->set_shader_input(ShaderInput("NeighborMinMax", _minmax_target->get_color_tex()));
        _target->set_shader_input(ShaderInput("PackedSceneData", _pack_target->get_color_tex()));

        _target->get_color_tex()->set_wrap_u(SamplerState::WM_clamp);
        _target->get_color_tex()->set_wrap_v(SamplerState::WM_clamp);
    }

    _target_cam_blur = create_target("CameraMotionBlur");
    _target_cam_blur->add_color_attachment(16);
    _target_cam_blur->prepare_buffer();

    if (per_object_blur_)
        _target_cam_blur->set_shader_input(ShaderInput("SourceTex", _target->get_color_tex()));
}

void MotionBlurStage::reload_shaders()
{
    if (per_object_blur_)
    {
        _tile_target->set_shader(load_plugin_shader({"fetch_dominant_velocity.frag.glsl"}));
        _tile_target_horiz->set_shader(load_plugin_shader({"fetch_dominant_velocity_horiz.frag.glsl"}));
        _minmax_target->set_shader(load_plugin_shader({"neighbor_minmax.frag.glsl"}));
        _pack_target->set_shader(load_plugin_shader({"pack_blur_data.frag.glsl"}));
        _target->set_shader(load_plugin_shader({"apply_motion_blur.frag.glsl"}));
    }
    _target_cam_blur->set_shader(load_plugin_shader({"camera_motion_blur.frag.glsl"}));
}

std::string MotionBlurStage::get_plugin_id() const
{
    return RPPLUGINS_ID_STRING;
}

}    // namespace rpplugins
