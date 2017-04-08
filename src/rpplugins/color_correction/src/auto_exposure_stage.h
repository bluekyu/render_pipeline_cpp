#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

class AutoExposureStage: public rpcore::RenderStage
{
public:
	AutoExposureStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "AutoExposureStage") {}

	virtual RequireType& get_required_inputs(void) const override { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const override { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const override;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;
	virtual void set_dimensions(void) override;

private:
	virtual std::string get_plugin_id(void) const override;

	static RequireType required_inputs;
	static RequireType required_pipes;

    bool stereo_mode_ = false;

	std::shared_ptr<rpcore::RenderTarget> _target_lum = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_analyze = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_apply = nullptr;
	std::shared_ptr<rpcore::Image> _tex_exposure;
	PT(Shader) _mip_shader;

	std::vector<std::shared_ptr<rpcore::RenderTarget>> _mip_targets;
};

}	// namespace rpplugins
