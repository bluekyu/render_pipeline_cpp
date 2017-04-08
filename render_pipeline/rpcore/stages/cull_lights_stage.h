#pragma once

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpcore {

class RenderTarget;

/** This stage takes the list of used cells and creates a list of lights for each cell.*/
class RPCPP_DECL CullLightsStage: public RenderStage
{
public:
	CullLightsStage(RenderPipeline& pipeline);

	static RequireType& get_global_required_inputs(void) { return required_inputs; }
	static RequireType& get_global_required_pipes(void) { return required_pipes; }

	virtual RequireType& get_required_inputs(void) const override { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const override { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const override;
	virtual DefinesType get_produced_defines(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	virtual void update(void) override;
	virtual void set_dimensions(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	int _max_lights_per_cell;
	int _slice_width;
	int _cull_threads = 32;
	int _num_light_classes;

	std::shared_ptr<Image> _frustum_lights_ctr;
	std::shared_ptr<Image> _frustum_lights;
	std::shared_ptr<Image> _per_cell_lights;
	std::shared_ptr<Image> _per_cell_light_counts;
	std::shared_ptr<Image> _grouped_cell_lights;
	std::shared_ptr<Image> _grouped_cell_lights_counts;

	std::shared_ptr<RenderTarget> _target_visible = nullptr;
	std::shared_ptr<RenderTarget> _target_cull = nullptr;
	std::shared_ptr<RenderTarget> _target_group = nullptr;
};
}	// namespace rpcore
