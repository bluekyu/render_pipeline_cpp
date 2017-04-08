#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage does the DoF pass. */
class DoFStage: public rpcore::RenderStage
{
public:
    DoFStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "DoFStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<rpcore::RenderTarget> target_prefilter_ = nullptr;
	std::shared_ptr<rpcore::RenderTarget> tile_target_ = nullptr;
	std::shared_ptr<rpcore::RenderTarget> tile_target_horiz_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> minmax_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> presort_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_merge_ = nullptr;
};

}	// namespace rpplugins
