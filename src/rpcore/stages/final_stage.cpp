/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "render_pipeline/rpcore/stages/final_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rpcore {

FinalStage::RequireType FinalStage::required_inputs_;
FinalStage::RequireType FinalStage::required_pipes_ = { "ShadedScene" };

FinalStage::ProduceType FinalStage::get_produced_pipes() const
{
    return {
        ShaderInput("ShadedScene", target_->get_color_tex()),
    };
}

void FinalStage::create()
{
    stereo_mode_ = pipeline_.is_stereo_mode();

    target_ = create_target("FinalStage").get();
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();

    // XXX: We cannot simply assign the final shader to the window display
    // region.This is because of a bug that the last FBO always only gets
    // 8 bits, regardles of what was requested - probably because of the
    // assumption that no tonemapping/srgb correction will follow afterwards.
    //
    // This might be a driver bug, or an optimization in Panda3D.However, it
    // also has the nice side effect that when taking screenshots(or using
    // the pixel inspector), we get the srgb corrected data, so its not too
    // much of a disadvantage.
    present_target_ = create_target("FinalPresentStage").get();
    present_target_->present_on_screen();
    present_target_->set_shader_input(ShaderInput("SourceTex", target_->get_color_tex()));
}

void FinalStage::reload_shaders()
{
    target_->set_shader(load_shader({ "final_stage.frag.glsl" }, stereo_mode_));
    present_target_->set_shader(load_shader({ "final_present_stage.frag.glsl" }));
}

std::string FinalStage::get_plugin_id() const
{
    return std::string("render_pipeline_internal");
}

}
