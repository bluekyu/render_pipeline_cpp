#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

class ColorCorrectionStage: public rpcore::RenderStage
{
public:
	ColorCorrectionStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ColorCorrectionStage") {}

	RequireType& get_required_inputs(void) const final { return required_inputs; }
	RequireType& get_required_pipes(void) const final { return required_pipes; }
	ProduceType get_produced_pipes(void) const final;

	void create(void) final;
	void reload_shaders(void) final;

private:
	std::string get_plugin_id(void) const final;

	static RequireType required_inputs;
	static RequireType required_pipes;

    bool stereo_mode_ = false;

	std::shared_ptr<rpcore::RenderTarget> target_ = nullptr;
};

}	// namespace rpplugins
