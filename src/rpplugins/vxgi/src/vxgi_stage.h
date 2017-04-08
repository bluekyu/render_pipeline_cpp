#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage applies the volumetric lighting. */
class VXGIStage: public rpcore::RenderStage
{
public:
	VXGIStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "VXGIStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<rpcore::RenderTarget> _target_spec = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_diff = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_blur_v = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_blur_h = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_upscale_diff = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_resolve = nullptr;
};

}	// namespace rpplugins
