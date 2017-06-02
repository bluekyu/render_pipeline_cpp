#include "render_pipeline/rpcore/stages/upscale_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/globals.hpp"

namespace rpcore {

UpscaleStage::RequireType UpscaleStage::required_inputs;
UpscaleStage::RequireType UpscaleStage::required_pipes = { "ShadedScene" };

UpscaleStage::ProduceType UpscaleStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", target_->get_color_tex()),
    };
}

void UpscaleStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("Upscale");
    target_->set_size(Globals::native_resolution);
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();

    set_cropping(pipeline_.get_setting<bool>("pipeline.screen_cropping", false));
}

void UpscaleStage::set_dimensions(void)
{
    target_->set_size(Globals::native_resolution);
}

void UpscaleStage::reload_shaders(void)
{
    target_->set_shader(load_shader({"upscale_stage.frag.glsl"}, stereo_mode_));
}

void UpscaleStage::set_cropping(bool enable)
{
    target_->set_shader_input(ShaderInput("use_cropping", LVecBase4i(enable ? 1 : 0)));
}

std::string UpscaleStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
