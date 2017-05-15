#include "forward_stage.h"

#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rppanda/showbase/showbase.h>
#include <render_pipeline/rpcore/render_pipeline.h>
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

}	// namespace rpplugins
