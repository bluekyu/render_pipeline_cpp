#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpcore {

class RenderTarget;

/** This stage computes the ambient term. */
class ApplyLightsStage: public RenderStage
{
public:
	ApplyLightsStage(RenderPipeline& pipeline): RenderStage(pipeline, "ApplyLightsStage") {}

	virtual RequireType& get_required_inputs(void) const override { return required_inputs_; }
	virtual RequireType& get_required_pipes(void) const override { return required_pipes_; }
	virtual ProduceType get_produced_pipes(void) const override;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const override;

	static RequireType required_inputs_;
	static RequireType required_pipes_;

	bool stereo_mode_ = false;
	std::shared_ptr<RenderTarget> target_ = nullptr;
};

}	// namespace rpcore
