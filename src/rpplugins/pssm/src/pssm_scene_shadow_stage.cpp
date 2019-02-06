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

#include "pssm_scene_shadow_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/util/task_scheduler.hpp>
#include <render_pipeline/rpcore/util/generic.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rpplugins {

PSSMSceneShadowStage::RequireType PSSMSceneShadowStage::required_inputs;
PSSMSceneShadowStage::RequireType PSSMSceneShadowStage::required_pipes;

PSSMSceneShadowStage::ProduceType PSSMSceneShadowStage::get_produced_inputs() const
{
    return {
        ShaderInput("PSSMSceneSunShadowMVP", _pta_mvp),
    };
}

PSSMSceneShadowStage::ProduceType PSSMSceneShadowStage::get_produced_pipes() const
{
    return {
        ShaderInput("PSSMSceneSunShadowMapPCF", _target->get_depth_tex(), make_pcf_state()),
    };
}

PSSMSceneShadowStage::PSSMSceneShadowStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "PSSMSceneShadowStage")
{
    _pta_mvp = PTA_LMatrix4f::empty_array(1);
    _focus.reset();

    // Store last focus entirely for the purpose of being able to see
    // it in the debugger
    _last_focus.reset();
}

SamplerState PSSMSceneShadowStage::make_pcf_state() const
{
    SamplerState state;
    state.set_minfilter(SamplerState::FT_shadow);
    state.set_magfilter(SamplerState::FT_shadow);
    return state;
}

void PSSMSceneShadowStage::request_focus(const LVecBase3& focus_point, PN_stdfloat focus_size)
{
    _focus = std::make_pair(focus_point, focus_size);
    _last_focus = _focus;
}

LMatrix4 PSSMSceneShadowStage::get_mvp() const
{
    return rpcore::Globals::base->get_render().get_transform(_cam_node)->get_mat() * _cam_lens->get_projection_mat();
}

void PSSMSceneShadowStage::create()
{
    _camera = new Camera("PSSMSceneSunShadowCam");
    _cam_lens = new OrthographicLens;
    _cam_lens->set_film_size(400, 400);
    _cam_lens->set_near_far(100.0, 1800.0);
    _camera->set_lens(_cam_lens);
    _cam_node = rpcore::Globals::base->get_render().attach_new_node(_camera);

    _target = create_target("ShadowMap");
    _target->set_size(_resolution);
    _target->add_depth_attachment(32);
    _target->prepare_render(_cam_node);

    // Register shadow camera
    pipeline_.get_tag_mgr()->register_camera("shadow", _camera);
}

void PSSMSceneShadowStage::update()
{
    if (pipeline_.get_task_scheduler()->is_scheduled("pssm_scene_shadows"))
    {
        if (!_focus)
        {
            // When no focus is set, there is no point in rendering the shadow map
            _target->set_active(false);
        }
        else
        {
            const LVecBase3& focus_point = _focus.value().first;
            const auto focus_size = _focus.value().second;

            _cam_lens->set_near_far(0.0f, 2.0f * (focus_size + _sun_distance));
            _cam_lens->set_film_size(2 * focus_size, 2 * focus_size);
            _cam_node.set_pos(
                focus_point + _sun_vector * (_sun_distance + focus_size));
            _cam_node.look_at(focus_point);

            const auto& mvp = get_mvp();
            rpcore::snap_shadow_map(mvp, _cam_node, _resolution);
            _target->set_active(true);
            _pta_mvp[0] = mvp;

            _focus.reset();
        }
    }
    else
    {
        _target->set_active(false);
    }
}

void PSSMSceneShadowStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

std::string PSSMSceneShadowStage::get_plugin_id() const
{
    return RPPLUGINS_ID_STRING;
}

}    // namespace rpplugins
