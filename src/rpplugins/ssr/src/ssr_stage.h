#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage does the SSR pass. */
class SSRStage: public rpcore::RenderStage
{
public:
    SSRStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "SSRStage") {}

    static RequireType& get_global_required_inputs(void) { return required_inputs; }
    static RequireType& get_global_required_pipes(void) { return required_pipes; }

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_velocity = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_reproject_lighting = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_upscale = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_resolve = nullptr;
};

}
