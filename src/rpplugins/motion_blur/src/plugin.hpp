#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "motion_blur_stage.h"

namespace rpplugins {

class Plugin: public rpcore::BasePlugin
{
public:
    Plugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const override { return require_plugins; }

    void on_stage_setup(void) override;

private:
    static RequrieType require_plugins;

    std::shared_ptr<MotionBlurStage> _stage;
};

}
