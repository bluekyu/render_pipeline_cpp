#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "color_correction_stage.h"
#include "tonemapping_stage.h"
#include "sharpen_stage.h"
#include "auto_exposure_stage.h"
#include "manual_exposure_stage.h"

namespace rpplugins {

class Plugin: public rpcore::BasePlugin
{
public:
    Plugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final { return require_plugins; }

    void on_pipeline_created(void) final;
    void on_stage_setup(void) final;

private:
    static RequrieType require_plugins;

    /** Loads the color correction lookup table (LUT). */
    void load_lut(void);

    /** Loads the precomputed film grain. */
    void load_grain(void);

    void update_color_lut(void);

    std::shared_ptr<ColorCorrectionStage> _stage;
    std::shared_ptr<TonemappingStage> _tonemapping_stage;
    std::shared_ptr<SharpenStage> _sharpen_stage;
    std::shared_ptr<AutoExposureStage> _exposure_stage_auto;
    std::shared_ptr<ManualExposureStage> _exposure_stage_manual;
};

}
