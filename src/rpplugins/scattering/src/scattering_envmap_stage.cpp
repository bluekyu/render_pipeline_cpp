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

#include "scattering_envmap_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/cubemap_filter.hpp>
#include <render_pipeline/rpcore/stages/ambient_stage.hpp>
#include <render_pipeline/rpcore/stages/gbuffer_stage.hpp>


namespace rpplugins {

ScatteringEnvmapStage::RequireType ScatteringEnvmapStage::required_inputs = { "DefaultSkydome", "DefaultEnvmap" };
ScatteringEnvmapStage::RequireType ScatteringEnvmapStage::required_pipes;

ScatteringEnvmapStage::ScatteringEnvmapStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ScatteringEnvmapStage")
{
}

ScatteringEnvmapStage::ProduceType ScatteringEnvmapStage::get_produced_pipes() const
{
    return {
        ShaderInput("ScatteringIBLDiffuse", _cubemap_filter->get_diffuse_cubemap()->get_texture()),
        ShaderInput("ScatteringIBLSpecular", _cubemap_filter->get_specular_cubemap()->get_texture()),
    };
}

void ScatteringEnvmapStage::create()
{
    _cubemap_filter = new rpcore::CubemapFilter(this, "ScatEnvCub");

    _target_cube = create_target("ComputeScattering");
    _target_cube->set_size(_cubemap_filter->get_size() * 6, _cubemap_filter->get_size());
    _target_cube->prepare_buffer();
    _target_cube->set_shader_input(ShaderInput("DestCubemap", _cubemap_filter->get_target_cubemap()->get_texture()));

    _cubemap_filter->create();

    // Make the stages use our cubemap textures
    rpcore::AmbientStage::get_global_required_pipes().push_back("ScatteringIBLDiffuse");
    rpcore::AmbientStage::get_global_required_pipes().push_back("ScatteringIBLSpecular");
    rpcore::GBufferStage::get_global_required_pipes().push_back("ScatteringIBLDiffuse");
    rpcore::GBufferStage::get_global_required_pipes().push_back("ScatteringIBLSpecular");
}

void ScatteringEnvmapStage::reload_shaders()
{
    _target_cube->set_shader(load_plugin_shader({"scattering_envmap.frag.glsl"}));
    _cubemap_filter->reload_shaders();
}

std::string ScatteringEnvmapStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
