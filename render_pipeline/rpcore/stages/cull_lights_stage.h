#pragma once

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpcore {

class RenderTarget;

/** This stage takes the list of used cells and creates a list of lights for each cell.*/
class RENDER_PIPELINE_DECL CullLightsStage: public RenderStage
{
public:
    CullLightsStage(RenderPipeline& pipeline);

    static RequireType& get_global_required_inputs(void) { return required_inputs; }
    static RequireType& get_global_required_pipes(void) { return required_pipes; }

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;
    DefinesType get_produced_defines(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

    void update(void) final;
    void set_dimensions(void) final;

private:
    std::string get_plugin_id(void) const final;

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

}
