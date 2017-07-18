#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

/** This stage raymarchs the cloud voxel grid. */
class ApplyCloudsStage: public rpcore::RenderStage
{
public:
    ApplyCloudsStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ApplyCloudsStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    std::shared_ptr<rpcore::RenderTarget> _render_target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _upscale_target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_apply_clouds = nullptr;
};

}    // namespace rpplugins
