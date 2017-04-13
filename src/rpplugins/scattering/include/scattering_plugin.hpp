#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class ScatteringStage;
class ScatteringEnvmapStage;
class GodrayStage;
class ScatteringMethod;

class ScatteringPlugin: public rpcore::BasePlugin
{
public:
    ScatteringPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final;

    void on_pipeline_created(void) final;
    void on_stage_setup(void) final;
    void on_pre_render_update(void) final;
    void on_shader_reload(void) final;

    virtual LVecBase3f get_sun_vector(void);

    virtual const std::shared_ptr<ScatteringStage>& get_display_stage(void) const;
    virtual const std::shared_ptr<ScatteringEnvmapStage>& get_envmap_stage(void) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
