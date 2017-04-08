#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage does the SSR pass. */
class SSRStage: public rpcore::RenderStage
{
public:
    SSRStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "SSRStage") {}

    static RequireType& get_global_required_inputs(void) { return required_inputs; }
    static RequireType& get_global_required_pipes(void) { return required_pipes; }

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_velocity = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_reproject_lighting = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_upscale = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_resolve = nullptr;
};

}	// namespace rpplugins
