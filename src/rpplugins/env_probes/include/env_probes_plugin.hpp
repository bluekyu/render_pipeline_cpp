#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

namespace rpplugins {

class EnvProbesPlugin: public rpcore::BasePlugin
{
public:
    EnvProbesPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final;

    void on_stage_setup(void) final;
    void on_prepare_scene(NodePath scene) final;
    void on_pre_render_update(void) final;

    virtual std::shared_ptr<rpcore::RenderStage> get_capture_stage(void);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}