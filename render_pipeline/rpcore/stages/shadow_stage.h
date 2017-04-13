#pragma once

#include <render_pipeline/rpcore/render_stage.h>

#include <samplerState.h>

class GraphicsOutput;

namespace rpcore {

/** This is the stage which renders all shadows. */
class ShadowStage: public RenderStage
{
public:
	ShadowStage(RenderPipeline& pipeline): RenderStage(pipeline, "ShadowStage") {}

	RequireType& get_required_inputs(void) const final { return required_inputs_; }
	RequireType& get_required_pipes(void) const final { return required_pipes_; }
	ProduceType get_produced_pipes(void) const final;

	SamplerState make_pcf_state(void) const;
	GraphicsOutput* get_atlas_buffer(void) const;

	void create(void) final;
	void set_shader_input(const ShaderInput& inp) final;

	size_t get_size(void) const;
	void set_size(size_t size);

private:
	std::string get_plugin_id(void) const final;

	static RequireType required_inputs_;
	static RequireType required_pipes_;

	size_t size_ = 4096;
	std::shared_ptr<RenderTarget> target_ = nullptr;
};

// ************************************************************************************************
inline size_t ShadowStage::get_size(void) const
{
	return size_;
}

inline void ShadowStage::set_size(size_t size)
{
	size_ = size;
}

}	// namespace rpcore
