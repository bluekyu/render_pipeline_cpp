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

#include "pssm_dist_shadow_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/util/task_scheduler.hpp>
#include <render_pipeline/rpcore/util/generic.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rpplugins {

PSSMDistShadowStage::RequireType PSSMDistShadowStage::required_inputs;
PSSMDistShadowStage::RequireType PSSMDistShadowStage::required_pipes;

PSSMDistShadowStage::ProduceType PSSMDistShadowStage::get_produced_inputs() const
{
    return {
        ShaderInput("PSSMDistSunShadowMapMVP", _pta_mvp),
    };
}

PSSMDistShadowStage::ProduceType PSSMDistShadowStage::get_produced_pipes() const
{
    return {
        ShaderInput("PSSMDistSunShadowMap", _target_blur_h->get_color_tex()),
    };
}

PSSMDistShadowStage::PSSMDistShadowStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "PSSMDistShadowStage")
{
    _pta_mvp = PTA_LMatrix4f::empty_array(1);
}

LMatrix4f PSSMDistShadowStage::get_mvp() const
{
    return rpcore::Globals::base->get_render().get_transform(_cam_node)->get_mat() * _cam_lens->get_projection_mat();
}

void PSSMDistShadowStage::create()
{
    _camera = new Camera("PSSMDistShadowsESM");
    _cam_lens = new OrthographicLens;
    _cam_lens->set_film_size(12000, 12000);
    _cam_lens->set_near_far(10.0, _sun_distance * 2);
    _camera->set_lens(_cam_lens);
    _cam_node = rpcore::Globals::base->get_render().attach_new_node(_camera);

    _target = create_target("ShadowMap");
    _target->set_size(_resolution);
    _target->add_depth_attachment(32);
    _target->prepare_render(_cam_node);

    _target_convert = create_target("ConvertToESM");
    _target_convert->set_size(_resolution);
    _target_convert->add_color_attachment(LVecBase4i(32, 0, 0, 0));
    _target_convert->prepare_buffer();
    _target_convert->set_shader_input(ShaderInput("SourceTex", _target->get_depth_tex()));

    _target_blur_v = create_target("BlurVert");
    _target_blur_v->set_size(_resolution);
    _target_blur_v->add_color_attachment(LVecBase4i(32, 0, 0, 0));
    _target_blur_v->prepare_buffer();
    _target_blur_v->set_shader_input(ShaderInput("SourceTex", _target_convert->get_color_tex()));
    _target_blur_v->set_shader_input(ShaderInput("direction", LVecBase2i(1, 0)));

    _target_blur_h = create_target("BlurHoriz");
    _target_blur_h->set_size(_resolution);
    _target_blur_h->add_color_attachment(LVecBase4i(32, 0, 0, 0));
    _target_blur_h->prepare_buffer();
    _target_blur_h->set_shader_input(ShaderInput("SourceTex", _target_blur_v->get_color_tex()));
    _target_blur_h->set_shader_input(ShaderInput("direction", LVecBase2i(0, 1)));

    // Register shadow camera
    pipeline_.get_tag_mgr()->register_camera("shadow", _camera);
}

void PSSMDistShadowStage::update()
{
    _target->set_active(false);
    _target_convert->set_active(false);
    _target_blur_v->set_active(false);
    _target_blur_h->set_active(false);

    const auto& mvp = get_mvp();

    // Query scheduled tasks
    if (pipeline_.get_task_scheduler()->is_scheduled("pssm_distant_shadows"))
    {
        _target->set_active(true);

        // Reposition camera before we capture the scene
        const LPoint3& cam_pos = rpcore::Globals::base->get_cam().get_pos(rpcore::Globals::base->get_render());
        _cam_node.set_pos(cam_pos + _sun_vector * _sun_distance);
        _cam_node.look_at(cam_pos);
        _cam_lens->set_film_size(_clip_size, _clip_size);

        rpcore::snap_shadow_map(mvp, _cam_node, _resolution);
    }

    if (pipeline_.get_task_scheduler()->is_scheduled("pssm_convert_distant_to_esm"))
        _target_convert->set_active(true);

    if (pipeline_.get_task_scheduler()->is_scheduled("pssm_blur_distant_vert"))
        _target_blur_v->set_active(true);

    if (pipeline_.get_task_scheduler()->is_scheduled("pssm_blur_distant_horiz"))
    {
        _target_blur_h->set_active(true);

        // Only update the MVP as soon as the shadow map is available
        _pta_mvp[0] = mvp;
    }
}

void PSSMDistShadowStage::reload_shaders()
{
    _target_convert->set_shader(load_plugin_shader({"convert_to_esm.frag.glsl"}));
    _target_blur_v->set_shader(load_plugin_shader({"blur_esm.frag.glsl"}));
    _target_blur_h->set_shader(load_plugin_shader({"blur_esm.frag.glsl"}));
}

void PSSMDistShadowStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

std::string PSSMDistShadowStage::get_plugin_id() const
{
    return RPPLUGINS_ID_STRING;
}

}    // namespace rpplugins
