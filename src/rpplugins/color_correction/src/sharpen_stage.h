#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

class SharpenStage: public rpcore::RenderStage
{
public:
	SharpenStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "SharpenStage") {}

	virtual RequireType& get_required_inputs(void) const override { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const override { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const override;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	bool get_sharpen_twice(void) const { return _sharpen_twice; }
	void set_sharpen_twice(bool sharpen_twice) { _sharpen_twice = sharpen_twice; }

private:
	virtual std::string get_plugin_id(void) const override;

	static RequireType required_inputs;
	static RequireType required_pipes;

    bool stereo_mode_ = false;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target2 = nullptr;

	bool _sharpen_twice = true;
};

}	// namespace rpplugins
