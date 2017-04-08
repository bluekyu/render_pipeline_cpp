#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

class AOStage: public rpcore::RenderStage
{
public:
	AOStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "AOStage") {}

	RequireType& get_required_inputs(void) const final { return required_inputs; }
	RequireType& get_required_pipes(void) const final { return required_pipes; }
	ProduceType get_produced_pipes(void) const final;

	void create(void) final;
	void reload_shaders(void) final;

	void set_quality(const std::string& quality);

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	bool stereo_mode_ = false;
	bool enable_small_scale_ao_;

    std::string quality_;

	std::shared_ptr<rpcore::RenderTarget> target_ = nullptr;
	std::shared_ptr<rpcore::RenderTarget> target_upscale_ = nullptr;
	std::shared_ptr<rpcore::RenderTarget> target_detail_ao_ = nullptr;
	std::vector<std::shared_ptr<rpcore::RenderTarget>> blur_targets_;
	std::shared_ptr<rpcore::RenderTarget> target_resolve_ = nullptr;
};

inline void AOStage::set_quality(const std::string& quality)
{
	quality_ = quality;
}

}	// namespace rpplugins
