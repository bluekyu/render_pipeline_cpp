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

#include "plugin.hpp"

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/image.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING)
{
}

void Plugin::on_pipeline_created()
{
    load_lut();
    load_grain();
}

void Plugin::on_stage_setup()
{
    auto stage = std::make_unique<ColorCorrectionStage>(pipeline_);
    stage_ = stage.get();
    add_stage(std::move(stage));

    auto tonemapping_stage = std::make_unique<TonemappingStage>(pipeline_);
    tonemapping_stage_ = tonemapping_stage.get();
    add_stage(std::move(tonemapping_stage));

    if (get_setting<rpcore::BoolType>("use_sharpen"))
    {
        auto sharpen_stage = std::make_unique<SharpenStage>(pipeline_);
        sharpen_stage->set_sharpen_twice(get_setting<rpcore::BoolType>("sharpen_twice"));
        add_stage(std::move(sharpen_stage));
    }

    if (get_setting<rpcore::BoolType>("manual_camera_parameters"))
    {
        auto exposure_stage_manual = std::make_unique<ManualExposureStage>(pipeline_);
        add_stage(std::move(exposure_stage_manual));
    }
    else
    {
        auto exposure_stage_auto = std::make_unique<AutoExposureStage>(pipeline_);
        add_stage(std::move(exposure_stage_auto));
    }
}

void Plugin::load_lut()
{
    std::string lut_path = get_resource(get_setting<rpcore::PathType>("color_lut"));
    PT(Texture) lut = rpcore::RPLoader::load_sliced_3d_texture(lut_path, 64);
    lut->set_wrap_u(SamplerState::WM_clamp);
    lut->set_wrap_v(SamplerState::WM_clamp);
    lut->set_wrap_w(SamplerState::WM_clamp);
    lut->set_minfilter(SamplerState::FT_linear);
    lut->set_magfilter(SamplerState::FT_linear);
    lut->set_anisotropic_degree(0);
    tonemapping_stage_->set_shader_input(ShaderInput("ColorLUT", lut));
}

void Plugin::load_grain()
{
    PT(Texture) grain_tex = rpcore::RPLoader::load_texture("/$$rp/data/film_grain/grain.txo");
    grain_tex->set_minfilter(SamplerState::FT_linear);
    grain_tex->set_magfilter(SamplerState::FT_linear);
    grain_tex->set_wrap_u(SamplerState::WM_repeat);
    grain_tex->set_wrap_v(SamplerState::WM_repeat);
    grain_tex->set_anisotropic_degree(0);
    stage_->set_shader_input(ShaderInput("PrecomputedGrain", grain_tex));
}

void Plugin::update_color_lut()
{
    debug("Updating color lut ..");
    load_lut();
}

}
