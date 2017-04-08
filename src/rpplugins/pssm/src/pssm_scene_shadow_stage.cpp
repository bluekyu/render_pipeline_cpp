#include "pssm_scene_shadow_stage.h"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/util/task_scheduler.h>
#include <render_pipeline/rpcore/util/generic.h>
#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rppanda/showbase/showbase.h>

namespace rpplugins {

PSSMSceneShadowStage::RequireType PSSMSceneShadowStage::required_inputs;
PSSMSceneShadowStage::RequireType PSSMSceneShadowStage::required_pipes;

PSSMSceneShadowStage::ProduceType PSSMSceneShadowStage::get_produced_inputs(void) const
{
    return {
        ShaderInput("PSSMSceneSunShadowMVP", _pta_mvp),
    };
}

PSSMSceneShadowStage::ProduceType PSSMSceneShadowStage::get_produced_pipes(void) const
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

SamplerState PSSMSceneShadowStage::make_pcf_state(void) const
{
    SamplerState state;
    state.set_minfilter(SamplerState::FT_shadow);
    state.set_magfilter(SamplerState::FT_shadow);
    return state;
}

void PSSMSceneShadowStage::request_focus(const LVecBase3f& focus_point, float focus_size)
{
    _focus = std::make_pair(focus_point, focus_size);
    _last_focus = _focus;
}

LMatrix4f PSSMSceneShadowStage::get_mvp(void) const
{
    return rpcore::Globals::base->get_render().get_transform(_cam_node)->get_mat() * _cam_lens->get_projection_mat();
}

void PSSMSceneShadowStage::create(void)
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

void PSSMSceneShadowStage::update(void)
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
            const LVecBase3f& focus_point = _focus.get().first;
            const float focus_size = _focus.get().second;

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

std::string PSSMSceneShadowStage::get_plugin_id(void) const
{
	return PLUGIN_ID_STRING;
}

}	// namespace rpplugins
