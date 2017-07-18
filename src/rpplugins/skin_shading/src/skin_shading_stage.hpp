#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

/** This is the main stage used by the SkinShadingStage plugin. */
class SkinShadingStage: public rpcore::RenderStage
{
public:
    SkinShadingStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "SkinShadingStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    PT(Texture) _final_tex = nullptr;
    std::vector<std::shared_ptr<rpcore::RenderTarget>> _blur_targets;
};

}    // namespace rpplugins
