#include "render_pipeline/rpcore/stages/final_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.h"

namespace rpcore {

FinalStage::RequireType FinalStage::required_inputs;
FinalStage::RequireType FinalStage::required_pipes = { "ShadedScene" };

FinalStage::ProduceType FinalStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", _target->get_color_tex()),
    };
}

void FinalStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    _target = create_target("FinalStage");
    _target->add_color_attachment(16);
    if (stereo_mode_)
        _target->set_layers(2);
    _target->prepare_buffer();

    // XXX: We cannot simply assign the final shader to the window display
    // region.This is because of a bug that the last FBO always only gets
    // 8 bits, regardles of what was requested - probably because of the
    // assumption that no tonemapping/srgb correction will follow afterwards.
    //
    // This might be a driver bug, or an optimization in Panda3D.However, it
    // also has the nice side effect that when taking screenshots(or using
    // the pixel inspector), we get the srgb corrected data, so its not too
    // much of a disadvantage.
    _present_target = create_target("FinalPresentStage");
    _present_target->present_on_screen();
    _present_target->set_shader_input(ShaderInput("SourceTex", _target->get_color_tex()));
}

void FinalStage::reload_shaders(void)
{
    _target->set_shader(load_shader({ "final_stage.frag.glsl" }, stereo_mode_));
    _present_target->set_shader(load_shader({ "final_present_stage.frag.glsl" }));
}

std::string FinalStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
