#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class VolumentricsPlugin: public rpcore::BasePlugin
{
public:
    VolumentricsPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const override;

    void on_stage_setup(void) override;

    virtual const std::shared_ptr<rpcore::RenderStage>& get_stage(void) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
