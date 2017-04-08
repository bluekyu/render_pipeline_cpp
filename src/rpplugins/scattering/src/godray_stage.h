#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage renders the suns godrays. */
class GodrayStage: public rpcore::RenderStage
{
public:
	GodrayStage(rpcore::RenderPipeline& pipeline);

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }

	virtual ProduceType get_produced_pipes(void) const override;

	virtual void create(void) override;

	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
};

}	// namespace rpplugins
