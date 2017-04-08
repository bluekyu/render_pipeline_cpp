#pragma once

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpplugins {

/**
 * This stage takes the list of used cells and creates a list of environment
 * probes for each cell.
 */
class CullProbesStage: public rpcore::RenderStage
{
public:
	CullProbesStage(rpcore::RenderPipeline& pipeline);

	RequireType& get_required_inputs(void) const final { return required_inputs; }
	RequireType& get_required_pipes(void) const final { return required_pipes; }

	ProduceType get_produced_pipes(void) const final;

	DefinesType get_produced_defines(void) const final;

	void create(void) final;
	void set_dimensions(void) final;

	void reload_shaders(void) final;

private:
	std::string get_plugin_id(void) const final;

	static RequireType required_inputs;
	static RequireType required_pipes;

	int _max_probes_per_cell = 4;
	int _slice_width;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::Image> _per_cell_probes;
};

}	// namespace rpplugins
