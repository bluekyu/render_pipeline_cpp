#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage raymarchs the cloud voxel grid. */
class ApplyCloudsStage: public rpcore::RenderStage
{
public:
    ApplyCloudsStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ApplyCloudsStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<rpcore::RenderTarget> _render_target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _upscale_target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_apply_clouds = nullptr;
};

}	// namespace rpplugins
