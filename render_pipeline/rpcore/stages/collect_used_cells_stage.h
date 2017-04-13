#pragma once

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpcore {

class RenderTarget;

/**
 * This stage collects the flagged cells from the FlagUsedCellsStage and
 * makes a list of them.
 */
class CollectUsedCellsStage: public RenderStage
{
public:
	CollectUsedCellsStage(RenderPipeline& pipeline): RenderStage(pipeline, "CollectUsedCellsStage") {}

	RequireType& get_required_inputs(void) const final { return required_inputs; }
	RequireType& get_required_pipes(void) const final { return required_pipes; }
	ProduceType get_produced_pipes(void) const final;

	void create(void) final;
	void reload_shaders(void) final;

	void update(void) final;
	void set_dimensions(void) final;

private:
	std::string get_plugin_id(void) const final;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<Image> _cell_list_buffer;
	std::shared_ptr<Image> _cell_index_buffer;

	std::shared_ptr<RenderTarget> _target = nullptr;
};

}	// namespace rpcore
