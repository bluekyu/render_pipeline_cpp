#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpcore {

class RenderTarget;

/** This stage downscales the depth buffer. */
class DownscaleZStage: public RenderStage
{
public:
    DownscaleZStage(RenderPipeline& pipeline): RenderStage(pipeline, "DownscaleZStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;
    std::shared_ptr<RenderTarget> target = nullptr;
};

}
