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

ScatteringEnvmapStage::ProduceType ScatteringEnvmapStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ScatteringIBLDiffuse", _cubemap_filter->get_diffuse_cubemap()->get_texture()),
        ShaderInput("ScatteringIBLSpecular", _cubemap_filter->get_specular_cubemap()->get_texture()),
    };
}

void ScatteringEnvmapStage::create(void)
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

void ScatteringEnvmapStage::reload_shaders(void)
{
    _target_cube->set_shader(load_plugin_shader({"scattering_envmap.frag.glsl"}));
    _cubemap_filter->reload_shaders();
}

std::string ScatteringEnvmapStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
