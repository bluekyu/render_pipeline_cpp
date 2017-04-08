#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class EnvProbesPlugin: public rpcore::BasePlugin
{
public:
    EnvProbesPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const override;

    void on_stage_setup(void) override;
    void on_prepare_scene(NodePath scene) override;
    void on_pre_render_update(void) override;

    virtual std::shared_ptr<rpcore::RenderStage> get_capture_stage(void);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}