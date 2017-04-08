#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class SMAAPlugin: public rpcore::BasePlugin
{
public:
    SMAAPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const override;

    void on_stage_setup(void) override;
    void on_pre_render_update(void) override;
    void on_window_resized(void) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
