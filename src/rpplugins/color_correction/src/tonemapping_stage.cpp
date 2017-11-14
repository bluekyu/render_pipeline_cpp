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

#include "tonemapping_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

TonemappingStage::RequireType TonemappingStage::required_inputs;
TonemappingStage::RequireType TonemappingStage::required_pipes = { "ShadedScene" };

TonemappingStage::ProduceType TonemappingStage::get_produced_pipes() const
{
    return {
        ShaderInput("ShadedScene", target_->get_color_tex()),
    };
}

void TonemappingStage::create()
{
    stereo_mode_ = pipeline_.is_stereo_mode();

    target_ = create_target("Tonemap");
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();
}

void TonemappingStage::reload_shaders()
{
    target_->set_shader(load_plugin_shader({"apply_tonemap.frag.glsl"}, stereo_mode_));
}

std::string TonemappingStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
