#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <render_pipeline/rpcore/render_stage.h>

namespace rpcore {

class RenderTarget;

/** This stage computes the ambient term. */
class RPCPP_DECL AmbientStage: public RenderStage
{
public:
	AmbientStage(RenderPipeline& pipeline): RenderStage(pipeline, "AmbientStage") {}

	static RequireType& get_global_required_inputs(void) { return required_inputs_; }
	static RequireType& get_global_required_pipes(void) { return required_pipes_; }

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
