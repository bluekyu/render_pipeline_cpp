#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "bloom_stage.hpp"

namespace rpplugins {

class Plugin: public rpcore::BasePlugin
{
public:
    Plugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const override { return require_plugins; }

    void on_stage_setup(void) override;
    void on_pipeline_created(void) override;

private:
    static RequrieType require_plugins;

    std::shared_ptr<BloomStage> _bloom_stage;
};

}
