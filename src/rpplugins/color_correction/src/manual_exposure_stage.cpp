#include "manual_exposure_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

ManualExposureStage::RequireType ManualExposureStage::required_inputs;
ManualExposureStage::RequireType ManualExposureStage::required_pipes = { "ShadedScene" };

ManualExposureStage::ProduceType ManualExposureStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", _target->get_color_tex()),
    };
}

void ManualExposureStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    _target = create_target("ManualExposure");
    _target->add_color_attachment(16);
    if (stereo_mode_)
        _target->set_layers(2);
    _target->prepare_buffer();
}

void ManualExposureStage::reload_shaders(void)
{
    _target->set_shader(load_plugin_shader({"manual_exposure.frag.glsl"}, stereo_mode_));
}

std::string ManualExposureStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
