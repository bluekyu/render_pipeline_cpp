#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

class AutoExposureStage: public rpcore::RenderStage
{
public:
    AutoExposureStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "AutoExposureStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;
    void set_dimensions(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;

    std::shared_ptr<rpcore::RenderTarget> _target_lum = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_analyze = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_apply = nullptr;
    std::shared_ptr<rpcore::Image> _tex_exposure;
    PT(Shader) _mip_shader;

    std::vector<std::shared_ptr<rpcore::RenderTarget>> _mip_targets;
};

}    // namespace rpplugins
