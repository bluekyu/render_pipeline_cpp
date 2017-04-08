#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class SSRPlugin: public rpcore::BasePlugin
{
public:
    SSRPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final;

    void on_stage_setup(void) final;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
