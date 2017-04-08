#pragma once

#include <camera.h>

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/**
 * Forward shading stage, which first renders all forward objects,
 * and then merges them with the scene.
 */
class ForwardStage: public rpcore::RenderStage
{
public:
	ForwardStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ForwardStage") {}

	RequireType& get_required_inputs(void) const final { return required_inputs; }
	RequireType& get_required_pipes(void) const final { return required_pipes; }
	ProduceType get_produced_pipes(void) const final;

	void create(void) final;
	void reload_shaders(void) final;

	void set_shader_input(const ShaderInput& inp) final;

private:
	std::string get_plugin_id(void) const final;

	static RequireType required_inputs;
	static RequireType required_pipes;

    bool stereo_mode_ = false;

	PT(Camera) _forward_cam;
	NodePath _forward_cam_np;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_merge = nullptr;
};

}	// namespace rpplugins
