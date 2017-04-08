#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class SMAAPlugin: public rpcore::BasePlugin
{
public:
    SMAAPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final;

    void on_stage_setup(void) final;
    void on_pre_render_update(void) final;
    void on_window_resized(void) final;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
