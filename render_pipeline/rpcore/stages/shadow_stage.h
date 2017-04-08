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

	virtual RequireType& get_required_inputs(void) const override { return required_inputs_; }
	virtual RequireType& get_required_pipes(void) const override { return required_pipes_; }
	virtual ProduceType get_produced_pipes(void) const override;

	SamplerState make_pcf_state(void) const;
	GraphicsOutput* get_atlas_buffer(void) const;

	void create(void) override;
	void set_shader_input(const ShaderInput& inp) override;

	size_t get_size(void) const;
	void set_size(size_t size);

private:
	virtual std::string get_plugin_id(void) const;

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
