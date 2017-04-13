#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage applies the volumetric lighting. */
class VXGIStage: public rpcore::RenderStage
{
public:
    VXGIStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "VXGIStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    std::shared_ptr<rpcore::RenderTarget> _target_spec = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_diff = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_blur_v = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_blur_h = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_upscale_diff = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_resolve = nullptr;
};

}
