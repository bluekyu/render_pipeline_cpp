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

#include "sharpen_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

SharpenStage::RequireType SharpenStage::required_inputs;
SharpenStage::RequireType SharpenStage::required_pipes = { "ShadedScene" };

SharpenStage::ProduceType SharpenStage::get_produced_pipes(void) const
{
    if (_sharpen_twice)
    {
        return {
            ShaderInput("ShadedScene", _target2->get_color_tex()),
        };
    }
    else
    {
        return {
            ShaderInput("ShadedScene", _target->get_color_tex()),
        };
    }
}

void SharpenStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    _target = create_target("Sharpen");
    _target->add_color_attachment(16);
    if (stereo_mode_)
        _target->set_layers(2);
    _target->prepare_buffer();

    if (_sharpen_twice)
    {
        _target2 = create_target("Sharpen2");
        _target2->add_color_attachment(16);
        if (stereo_mode_)
                _target2->set_layers(2);
        _target2->prepare_buffer();
        _target2->set_shader_input(ShaderInput("ShadedScene", _target->get_color_tex()));
    }
}

void SharpenStage::reload_shaders(void)
{
    auto sharpen_shader = load_plugin_shader({"sharpen.frag.glsl"}, stereo_mode_);
    for (auto& target: targets_)
        target.second->set_shader(sharpen_shader);
}

std::string SharpenStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
