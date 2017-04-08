#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class VXGIPlugin: public rpcore::BasePlugin
{
public:
    VXGIPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const override;

    void on_stage_setup(void) override;
    void on_pipeline_created(void) override;
    void on_pre_render_update(void) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
