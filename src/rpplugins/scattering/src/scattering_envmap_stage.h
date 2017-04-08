#pragma once

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpcore {
class CubemapFilter;
}

namespace rpplugins {

/**
 * This stage uses the precomputed data to make a cubemap containing the
 * scattering.
 */
class ScatteringEnvmapStage: public rpcore::RenderStage
{
public:
	ScatteringEnvmapStage(rpcore::RenderPipeline& pipeline);

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }

	virtual ProduceType get_produced_pipes(void) const override;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<rpcore::RenderTarget> _target_cube = nullptr;
	rpcore::CubemapFilter* _cubemap_filter = nullptr;
};

}	// namespace rpplugins
