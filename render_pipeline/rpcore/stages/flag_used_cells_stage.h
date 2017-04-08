#pragma once

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpcore {

class RenderTarget;
class Image;

/** This stage flags all used cells based on the depth buffer. */
class FlagUsedCellsStage: public RenderStage
{
public:
	FlagUsedCellsStage(RenderPipeline& pipeline): RenderStage(pipeline, "FlagUsedCellsStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	virtual void update(void) override;
	virtual void set_dimensions(void) override;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	std::shared_ptr<Image> _cell_grid_flags;

	std::shared_ptr<RenderTarget> _target = nullptr;
};

}	// namespace rpcore
