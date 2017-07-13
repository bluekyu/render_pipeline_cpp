#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

namespace rpplugins {

class SkinShadingPlugin: public rpcore::BasePlugin
{
public:
    SkinShadingPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final;

    void on_stage_setup(void) final;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
