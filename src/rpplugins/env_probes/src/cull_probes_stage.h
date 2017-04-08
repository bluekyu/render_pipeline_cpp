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

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }

	virtual ProduceType get_produced_pipes(void) const override;

	virtual DefinesType get_produced_defines(void) const override;

	virtual void create(void) override;
	virtual void set_dimensions(void) override;

	virtual void reload_shaders(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	int _max_probes_per_cell = 4;
	int _slice_width;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::Image> _per_cell_probes;
};

}	// namespace rpplugins
