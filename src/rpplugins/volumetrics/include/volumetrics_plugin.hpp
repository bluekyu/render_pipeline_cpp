#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

namespace rpplugins {

class VolumetricsStage;

class VolumentricsPlugin: public rpcore::BasePlugin
{
public:
    VolumentricsPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final;

    void on_stage_setup(void) final;

    virtual const std::shared_ptr<VolumetricsStage>& get_stage(void) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
