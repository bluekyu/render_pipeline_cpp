#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage applies the volumetric lighting. */
class VolumetricsStage: public rpcore::RenderStage
{
public:
	VolumetricsStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "VolumetricsStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	void set_enable_volumetric_shadows(bool enable_volumetric_shadows);

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	bool _enable_volumetric_shadows = false;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_upscale = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_combine = nullptr;
};

inline void VolumetricsStage::set_enable_volumetric_shadows(bool enable_volumetric_shadows)
{
	_enable_volumetric_shadows = enable_volumetric_shadows;
}

}	// namespace rpplugins
