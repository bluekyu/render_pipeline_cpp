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

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	virtual void set_shader_input(const ShaderInput& inp) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

    bool stereo_mode_ = false;

	PT(Camera) _forward_cam;
	NodePath _forward_cam_np;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_merge = nullptr;
};

}	// namespace rpplugins
