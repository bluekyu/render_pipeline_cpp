#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This is the main stage used by the SkinShadingStage plugin. */
class SkinShadingStage: public rpcore::RenderStage
{
public:
	SkinShadingStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "SkinShadingStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	PT(Texture) _final_tex = nullptr;
	std::vector<std::shared_ptr<rpcore::RenderTarget>> _blur_targets;
};

}	// namespace rpplugins
