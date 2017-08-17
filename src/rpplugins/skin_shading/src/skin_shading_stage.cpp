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

#include "skin_shading_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

SkinShadingStage::RequireType SkinShadingStage::required_inputs;
SkinShadingStage::RequireType SkinShadingStage::required_pipes = { "ShadedScene", "GBuffer" };

SkinShadingStage::ProduceType SkinShadingStage::get_produced_pipes() const
{
    return {
        ShaderInput("ShadedScene", _final_tex.p()),
    };
}

void SkinShadingStage::create()
{
    Texture* current_tex = nullptr;
    _blur_targets.clear();

    for (int i = 0; i < 3; ++i)
    {
        auto target_h = create_target(std::string("BlurH-") + std::to_string(i));
        target_h->add_color_attachment(16);
        target_h->prepare_buffer();
        target_h->set_shader_input(ShaderInput("direction", LVecBase2i(1, 0)));
        if (current_tex)
            target_h->set_shader_input(ShaderInput("ShadedScene", current_tex), true);
        current_tex = target_h->get_color_tex();

        auto target_v = create_target(std::string("BlurV-") + std::to_string(i));
        target_v->add_color_attachment(16);
        target_v->prepare_buffer();
        target_v->set_shader_input(ShaderInput("ShadedScene", current_tex), true);
        target_v->set_shader_input(ShaderInput("direction", LVecBase2i(0, 1)));
        current_tex = target_v->get_color_tex();

        _blur_targets.push_back(target_h);
        _blur_targets.push_back(target_v);
    }
    _final_tex = current_tex;
}

void SkinShadingStage::reload_shaders()
{
    PT(Shader) blur_shader = load_plugin_shader({"sssss_blur.frag.glsl"});
    for (auto& target: _blur_targets)
        target->set_shader(blur_shader);
}

std::string SkinShadingStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
