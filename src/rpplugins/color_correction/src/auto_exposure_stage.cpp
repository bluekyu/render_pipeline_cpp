#include "auto_exposure_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/image.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

AutoExposureStage::RequireType AutoExposureStage::required_inputs;
AutoExposureStage::RequireType AutoExposureStage::required_pipes = { "ShadedScene" };

AutoExposureStage::ProduceType AutoExposureStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", _target_apply->get_color_tex()),
        ShaderInput("Exposure", _tex_exposure->get_texture()),
    };
}

void AutoExposureStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    // Create the target which converts the scene color to a luminance
    _target_lum = create_target("GetLuminance");
    _target_lum->set_size("25%");
    _target_lum->add_color_attachment(LVecBase4i(16, 0, 0, 0));
    if (stereo_mode_)
        _target_lum->set_layers(2);
    _target_lum->prepare_buffer();

    _mip_targets.clear();

    // Create the storage for the exposure, this stores the current and last
    // frames exposure
    // XXX: We have to use F_r16 instead of F_r32 because of a weird nvidia
    // driver bug!However, 16 bits should be enough for sure.
    if (stereo_mode_)
        _tex_exposure = rpcore::Image::create_buffer("ExposureStorage", 2, "R16");
    else
        _tex_exposure = rpcore::Image::create_buffer("ExposureStorage", 1, "R16");
    _tex_exposure->set_clear_color(LVecBase4f(0.5f));
    _tex_exposure->clear_image();

    // Create the target which extracts the exposure from the average brightness
    _target_analyze = create_target("AnalyzeBrightness");
    _target_analyze->set_size(1, 1);
    _target_analyze->prepare_buffer();

    _target_analyze->set_shader_input(ShaderInput("ExposureStorage", _tex_exposure->get_texture()));

    // Create the target which applies the generated exposure to the scene
    _target_apply = create_target("ApplyExposure");
    _target_apply->add_color_attachment(16);
    if (stereo_mode_)
        _target_apply->set_layers(2);
    _target_apply->prepare_buffer();
    _target_apply->set_shader_input(ShaderInput("Exposure", _tex_exposure->get_texture()));
}

void AutoExposureStage::set_dimensions(void)
{
    for (auto& old_target: _mip_targets)
        remove_target(old_target);

    int wsize_x = (rpcore::Globals::resolution.get_x() + 3) / 4;
    int wsize_y = (rpcore::Globals::resolution.get_y() + 3) / 4;

    // Create the targets which downscale the luminance mipmaps
    _mip_targets.clear();
    Texture* last_tex = _target_lum->get_color_tex();
    while (wsize_x >= 4 || wsize_y >= 4)
    {
        wsize_x = (wsize_x + 3) / 4;
        wsize_y = (wsize_y + 3) / 4;

        auto mip_target = create_target(std::string("DScaleLum:S") + std::to_string(wsize_x));
        mip_target->add_color_attachment(LVecBase4i(16, 0, 0, 0));
        mip_target->set_size(wsize_x, wsize_y);
        mip_target->set_sort(_target_lum->get_sort().get() + _mip_targets.size());
        if (stereo_mode_)
            mip_target->set_layers(2);
        mip_target->prepare_buffer();
        mip_target->set_shader_input(ShaderInput("SourceTex", last_tex));
        _mip_targets.push_back(mip_target);
        last_tex = mip_target->get_color_tex();
    }

    _target_analyze->set_shader_input(ShaderInput("DownscaledTex", _mip_targets.back()->get_color_tex()));

    // Shaders might not have been loaded at this point
    if (!_mip_shader.is_null())
    {
        for (auto& target: _mip_targets)
            target->set_shader(_mip_shader);
    }
}

void AutoExposureStage::reload_shaders(void)
{
    _target_lum->set_shader(load_plugin_shader({"generate_luminance.frag.glsl"}, stereo_mode_));
    _target_analyze->set_shader(load_plugin_shader({"analyze_brightness.frag.glsl"}));
    _target_apply->set_shader(load_plugin_shader({"apply_exposure.frag.glsl"}, stereo_mode_));

    // Keep shader as reference, required when resizing
    _mip_shader = load_plugin_shader({"downscale_luminance.frag.glsl"}, stereo_mode_);
    for (auto& target: _mip_targets)
        target->set_shader(_mip_shader);
}

std::string AutoExposureStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
