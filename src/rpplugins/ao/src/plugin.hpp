#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "ao_stage.hpp"

namespace rpplugins {

class Plugin: public rpcore::BasePlugin
{
public:
    Plugin(rpcore::RenderPipeline& pipeline);
    ~Plugin(void) final;

    RequrieType& get_required_plugins(void) const final { return require_plugins; }

    void on_stage_setup(void) final;

private:
    static RequrieType require_plugins;

    std::shared_ptr<AOStage> stage_;
};

}
