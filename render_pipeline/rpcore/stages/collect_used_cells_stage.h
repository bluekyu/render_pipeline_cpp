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

	virtual RequireType& get_required_inputs(void) const override { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const override { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const override;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	virtual void update(void) override;
	virtual void set_dimensions(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<Image> _cell_list_buffer;
	std::shared_ptr<Image> _cell_index_buffer;

	std::shared_ptr<RenderTarget> _target = nullptr;
};

}	// namespace rpcore
