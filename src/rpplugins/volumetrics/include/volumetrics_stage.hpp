#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

/** This stage applies the volumetric lighting. */
class VolumetricsStage: public rpcore::RenderStage
{
public:
    VolumetricsStage(rpcore::RenderPipeline& pipeline);
    ~VolumetricsStage(void);

    RequireType& get_required_inputs(void) const final;
    RequireType& get_required_pipes(void) const final;
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

    virtual void set_enable_volumetric_shadows(bool enable_volumetric_shadows);

private:
    std::string get_plugin_id(void) const final;

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
