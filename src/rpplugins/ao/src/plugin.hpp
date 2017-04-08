#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "ao_stage.hpp"

namespace rpplugins {

class Plugin: public rpcore::BasePlugin
{
public:
    Plugin(rpcore::RenderPipeline& pipeline);
    ~Plugin(void) override;

    virtual RequrieType& get_required_plugins(void) const override { return require_plugins; }

    virtual void on_stage_setup(void) override;

private:
    static RequrieType require_plugins;

    std::shared_ptr<AOStage> stage_;
};

}
