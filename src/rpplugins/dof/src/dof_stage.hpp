#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

/** This stage does the DoF pass. */
class DoFStage: public rpcore::RenderStage
{
public:
    DoFStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "DoFStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    std::shared_ptr<rpcore::RenderTarget> target_prefilter_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> tile_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> tile_target_horiz_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> minmax_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> presort_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_merge_ = nullptr;
};

}    // namespace rpplugins
