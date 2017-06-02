#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>
#include <render_pipeline/rpcore/image.hpp>

namespace rpcore {

class RenderTarget;
class Image;

/** This stage flags all used cells based on the depth buffer. */
class FlagUsedCellsStage: public RenderStage
{
public:
    FlagUsedCellsStage(RenderPipeline& pipeline): RenderStage(pipeline, "FlagUsedCellsStage") {}

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

    std::shared_ptr<Image> _cell_grid_flags;

    std::shared_ptr<RenderTarget> _target = nullptr;
};

}
