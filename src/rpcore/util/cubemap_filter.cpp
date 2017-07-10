#include "render_pipeline/rpcore/util/cubemap_filter.hpp"

#include "render_pipeline/rpcore/render_stage.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/image.hpp"

namespace rpcore {

CubemapFilter::CubemapFilter(RenderStage* stage, const std::string& name, int size): RPObject("CubemapFilter"), _stage(stage), _name(name), _size(size)
{
    make_maps();
}

void CubemapFilter::create(void)
{
    make_specular_targets();
    make_diffuse_target();
}

void CubemapFilter::reload_shaders(void)
{
    // Set diffuse filter shaders
    _diffuse_target->set_shader(_stage->load_shader({"ibl/cubemap_diffuse.frag.glsl"}));
    _diff_filter_target->set_shader(_stage->load_shader({"ibl/cubemap_diffuse_filter.frag.glsl"}));

    // Set specular prefilter shaders
    PT(Shader) mip_shader = _stage->load_shader({"ibl/cubemap_specular_prefilter.frag.glsl"});
    for (auto& target: _targets_spec)
        target->set_shader(mip_shader);

    // Special shader for the first prefilter target
    _targets_spec[0]->set_shader(_stage->load_shader({"ibl/cubemap_specular_prefilter_first.frag.glsl"}));

    // Set specular filter sampling shaders
    PT(Shader) mip_filter_shader = _stage->load_shader({"ibl/cubemap_specular_filter.frag.glsl"});
    for (auto& target: _targets_spec_filter)
        target->set_shader(mip_filter_shader);

    // Special shader for the first filter target
    _targets_spec_filter[0]->set_shader(_stage->load_shader({"ibl/cubemap_specular_filter_first.frag.glsl"}));
}

void CubemapFilter::make_maps(void)
{
    // Create the cubemaps for the diffuse and specular components
    _prefilter_map = Image::create_cube(_name + "IBLPrefDiff", CubemapFilter::PREFILTER_CUBEMAP_SIZE, "R11G11B10");
    _diffuse_map = Image::create_cube(_name + "IBLDiff", CubemapFilter::DIFFUSE_CUBEMAP_SIZE, "R11G11B10");
    _spec_pref_map = Image::create_cube(_name + "IBLPrefSpec", _size, "R11G11B10");
    _specular_map = Image::create_cube(_name + "IBLSpec", _size, "R11G11B10");

    // Set the correct filtering modes
    for (auto& tex: {_diffuse_map, _specular_map, _prefilter_map, _spec_pref_map})
    {
        tex->set_minfilter(SamplerState::FT_linear);
        tex->set_magfilter(SamplerState::FT_linear);
        tex->set_clear_color(LColor(0));
        tex->clear_image();
    }

    // Use mipmaps for the specular cubemap
    _spec_pref_map->set_minfilter(SamplerState::FT_linear_mipmap_linear);
    _specular_map->set_minfilter(SamplerState::FT_linear_mipmap_linear);
}

void CubemapFilter::make_specular_targets(void)
{
    _targets_spec.clear();
    _targets_spec_filter.clear();
    int mip = 0;
    int mipsize = _size;

    while (mipsize >= 2)
    {
        // Assuming an initial size of a power of 2, its safe to do this
        mipsize = mipsize / 2;

        // Create the target which downsamples the mipmap
        auto target = _stage->create_target(std::string("CF:SpecIBL-PreFilter-") + std::to_string(mipsize));
        target->set_size(mipsize * 6, mipsize);
        target->prepare_buffer();

        if (mip == 0)
            target->set_shader_input(ShaderInput("SourceTex", _specular_map->get_texture()));
        else
            target->set_shader_input(ShaderInput("SourceTex", _spec_pref_map->get_texture()));

        target->set_shader_input(ShaderInput("DestMipmap", _spec_pref_map->get_texture(), false, true, -1, mip + 1, 0));
        target->set_shader_input(ShaderInput("currentMip", LVecBase4i(mip, 0, 0, 0)));

        mip += 1;

        // Create the target which filters the mipmap and removes the noise
        auto target_filter = _stage->create_target(std::string("CF:SpecIBL-PostFilter-") + std::to_string(mipsize));
        target_filter->set_size(mipsize * 6, mipsize);
        target_filter->prepare_buffer();
        target_filter->set_shader_input(ShaderInput("currentMip", LVecBase4i(mip, 0, 0, 0)));
        target_filter->set_shader_input(ShaderInput("SourceTex", _spec_pref_map->get_texture()));
        target_filter->set_shader_input(ShaderInput("DestMipmap", _specular_map->get_texture(), false, true, -1, mip, 0));

        _targets_spec.push_back(target);
        _targets_spec_filter.push_back(target_filter);
    }
}

void CubemapFilter::make_diffuse_target(void)
{
    // Create the target which integrates the lambert brdf
    _diffuse_target = _stage->create_target("CF:DiffuseIBL");
    _diffuse_target->set_size(CubemapFilter::PREFILTER_CUBEMAP_SIZE * 6, CubemapFilter::PREFILTER_CUBEMAP_SIZE);
    _diffuse_target->prepare_buffer();

    _diffuse_target->set_shader_input(ShaderInput("SourceCubemap", _specular_map->get_texture()));
    _diffuse_target->set_shader_input(ShaderInput("DestCubemap", _prefilter_map->get_texture()));
    _diffuse_target->set_shader_input(ShaderInput("cubeSize", LVecBase4i(CubemapFilter::PREFILTER_CUBEMAP_SIZE, 0, 0, 0)));

    // Create the target which removes the noise from the previous target,
    // which is introduced with importance sampling
    _diff_filter_target = _stage->create_target("CF:DiffPrefIBL");
    _diff_filter_target->set_size(CubemapFilter::DIFFUSE_CUBEMAP_SIZE * 6, CubemapFilter::DIFFUSE_CUBEMAP_SIZE);
    _diff_filter_target->prepare_buffer();

    _diff_filter_target->set_shader_input(ShaderInput("SourceCubemap", _prefilter_map->get_texture()));
    _diff_filter_target->set_shader_input(ShaderInput("DestCubemap", _diffuse_map->get_texture()));
    _diff_filter_target->set_shader_input(ShaderInput("cubeSize", LVecBase4i(CubemapFilter::DIFFUSE_CUBEMAP_SIZE, 0, 0, 0)));
}

}

