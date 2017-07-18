#include "color_correction_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

ColorCorrectionStage::RequireType ColorCorrectionStage::required_inputs;
ColorCorrectionStage::RequireType ColorCorrectionStage::required_pipes = { "ShadedScene" };

ColorCorrectionStage::ProduceType ColorCorrectionStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", target_->get_color_tex()),
    };
}

void ColorCorrectionStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("PostFX");
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();
}

void ColorCorrectionStage::reload_shaders(void)
{
    target_->set_shader(load_plugin_shader({"post_fx.frag.glsl"}, stereo_mode_));
}

std::string ColorCorrectionStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
