#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpcore {

class SimpleInputBlock;

/**
 * This is the main pass stage, rendering the objects and creating the
 * GBuffer which is used in later stages.
 */
class RPCPP_DECL GBufferStage: public RenderStage
{
public:
	GBufferStage(RenderPipeline& pipeline): RenderStage(pipeline, "GBufferStage") {}

	static RequireType& get_global_required_inputs(void) { return required_inputs; }
	static RequireType& get_global_required_pipes(void) { return required_pipes; }

	RequireType& get_required_inputs(void) const final { return required_inputs; }
	RequireType& get_required_pipes(void) const final { return required_pipes; }
	ProduceType get_produced_pipes(void) const final;

	std::shared_ptr<SimpleInputBlock> make_gbuffer_ubo(void) const;
	void create(void) final;

	void set_shader_input(const ShaderInput& inp) final;

private:
	std::string get_plugin_id(void) const final;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<RenderTarget> target = nullptr;
};

}	// namespace rpcore
