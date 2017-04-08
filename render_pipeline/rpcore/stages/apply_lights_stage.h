#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpcore {

class RenderTarget;

/** This stage computes the ambient term. */
class ApplyLightsStage: public RenderStage
{
public:
	ApplyLightsStage(RenderPipeline& pipeline): RenderStage(pipeline, "ApplyLightsStage") {}

	RequireType& get_required_inputs(void) const final { return required_inputs_; }
	RequireType& get_required_pipes(void) const final { return required_pipes_; }
	ProduceType get_produced_pipes(void) const final;

	void create(void) final;
	void reload_shaders(void) final;

private:
	std::string get_plugin_id(void) const final;

	static RequireType required_inputs_;
	static RequireType required_pipes_;

	bool stereo_mode_ = false;
	std::shared_ptr<RenderTarget> target_ = nullptr;
};

}	// namespace rpcore
