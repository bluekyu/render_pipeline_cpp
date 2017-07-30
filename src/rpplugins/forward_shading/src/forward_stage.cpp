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

#include "forward_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/native/tag_state_manager.h>

namespace rpplugins {

ForwardStage::RequireType ForwardStage::required_inputs = { "DefaultEnvmap", "PrefilteredBRDF", "PrefilteredCoatBRDF" };
ForwardStage::RequireType ForwardStage::required_pipes = { "SceneDepth", "ShadedScene", "CellIndices" };

ForwardStage::ProduceType ForwardStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", _target_merge->get_color_tex()),
    };
}

void ForwardStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    _forward_cam = new Camera("ForwardShadingCam");
    _forward_cam->set_lens(rpcore::Globals::base->get_cam_lens());
    _forward_cam_np = rpcore::Globals::base->get_cam().attach_new_node(_forward_cam);

    _target = create_target("ForwardShading");
    _target->add_color_attachment(16, true);
    _target->add_depth_attachment(32);
    if (stereo_mode_)
        _target->set_layers(2);
    _target->prepare_render(_forward_cam_np);
    _target->set_clear_color(LColor(0, 0, 0, 0));

    pipeline_.get_tag_mgr()->register_camera("forward", _forward_cam);

    _target_merge = create_target("MergeWithDeferred");
    _target_merge->add_color_attachment(16);
    if (stereo_mode_)
        _target_merge->set_layers(2);
    _target_merge->prepare_buffer();
    _target_merge->set_shader_input(ShaderInput("ForwardDepth", _target->get_depth_tex()));
    _target_merge->set_shader_input(ShaderInput("ForwardColor", _target->get_color_tex()));
}

void ForwardStage::reload_shaders(void)
{
    _target_merge->set_shader(load_plugin_shader({"merge_with_deferred.frag.glsl"}, stereo_mode_));
}

void ForwardStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
    RenderStage::set_shader_input(inp);
}

std::string ForwardStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
