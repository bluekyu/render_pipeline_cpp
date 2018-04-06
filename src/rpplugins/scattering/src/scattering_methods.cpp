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

#include "scattering_methods.hpp"

#include <virtualFileSystem.h>
#include <graphicsEngine.h>

#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/image.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/stdpy/file.hpp>

#include "scattering_stage.hpp"
#include "scattering_envmap_stage.hpp"

namespace rpplugins {

void ScatteringMethodEricBruneton::load()
{
    _use_32_bit = false;

    _trans_w = 256 * 4;
    _trans_h = 64 * 4;

    _sky_w = 64 * 4;
    _sky_h = 16 * 4;

    _res_r = 32;
    _res_mu = 128;
    _res_mu_s = 32;
    _res_nu = 8;

    _res_mu_s_nu = _res_mu_s * _res_nu;

    create_shaders();
    create_textures();
}

void ScatteringMethodEricBruneton::exec_compute_shader(const Shader* shader_obj, const std::vector<ShaderInput>& shader_inputs,
    const LVecBase3i& exec_size, const LVecBase3i& workgroup_size)
{
    const int ntx = int(std::ceil(float(exec_size[0]) / workgroup_size[0]));
    const int nty = int(std::ceil(float(exec_size[1]) / workgroup_size[1]));
    const int ntz = int(std::ceil(float(exec_size[2]) / workgroup_size[2]));

    NodePath nodepath("shader");
    nodepath.set_shader(shader_obj);
    for (auto&& input: shader_inputs)
        nodepath.set_shader_input(input);

    const ShaderAttrib* attr = DCAST(ShaderAttrib, nodepath.get_attrib(ShaderAttrib::get_class_type()));
    rpcore::Globals::base->get_graphics_engine()->dispatch_compute(LVecBase3i(ntx, nty, ntz), attr, rpcore::Globals::base->get_win()->get_gsg());
}

void ScatteringMethodEricBruneton::compute()
{
    debug("Precomputing ...");

    // Transmittance
    exec_compute_shader(_shaders.at("transmittance"), {
            ShaderInput("dest", _textures.at("transmittance")->get_texture()),
        }, LVecBase3i(_trans_w, _trans_h, 1));

    // Delta E
    exec_compute_shader(_shaders.at("delta_e"), {
            ShaderInput("transmittanceSampler", _textures.at("transmittance")->get_texture()),
            ShaderInput("dest", _textures.at("delta_e")->get_texture()),
        }, LVecBase3i(_sky_w, _sky_h, 1));

    // Delta S
    exec_compute_shader(_shaders.at("delta_sm_sr"), {
            ShaderInput("transmittanceSampler", _textures.at("transmittance")->get_texture()),
            ShaderInput("destDeltaSR", _textures.at("delta_sr")->get_texture()),
            ShaderInput("destDeltaSM", _textures.at("delta_sm")->get_texture()),
        }, LVecBase3i(_res_mu_s_nu, _res_mu, _res_r), LVecBase3i(8, 8, 8));

    // Copy deltaE to irradiance texture
    exec_compute_shader(_shaders.at("copy_irradiance"), {
            ShaderInput("k", LVecBase4f(0.0f, 0.0f, 0.0f, 0.0f)),
            ShaderInput("deltaESampler", _textures.at("delta_e")->get_texture()),
            ShaderInput("dest", _textures.at("irradiance")->get_texture()),
        }, LVecBase3i(_sky_w, _sky_h, 1));


    // Copy delta s into inscatter texture
    exec_compute_shader(_shaders.at("copy_inscatter"), {
            ShaderInput("deltaSRSampler", _textures.at("delta_sr")->get_texture()),
            ShaderInput("deltaSMSampler", _textures.at("delta_sm")->get_texture()),
            ShaderInput("dest", _textures.at("inscatter")->get_texture()),
        }, LVecBase3i(_res_mu_s_nu, _res_mu, _res_r), LVecBase3i(8, 8, 8));

    for (int order = 2; order < 5; ++order)
    {
        const int first = order == 2 ? 1 : 0;

        // Delta J
        exec_compute_shader(_shaders.at("delta_j"), {
                ShaderInput("transmittanceSampler", _textures.at("transmittance")->get_texture()),
                ShaderInput("deltaSRSampler", _textures.at("delta_sr")->get_texture()),
                ShaderInput("deltaSMSampler", _textures.at("delta_sm")->get_texture()),
                ShaderInput("deltaESampler", _textures.at("delta_e")->get_texture()),
                ShaderInput("dest", _textures.at("delta_j")->get_texture()),
                ShaderInput("first", LVecBase4i(first, 0, 0, 0))
            }, LVecBase3i(_res_mu_s_nu, _res_mu, _res_r), LVecBase3i(8, 8, 8));

        // Delta E
        exec_compute_shader(_shaders.at("irradiance_n"), {
                ShaderInput("transmittanceSampler", _textures.at("transmittance")->get_texture()),
                ShaderInput("deltaSRSampler", _textures.at("delta_sr")->get_texture()),
                ShaderInput("deltaSMSampler", _textures.at("delta_sm")->get_texture()),
                ShaderInput("dest", _textures.at("delta_e")->get_texture()),
                ShaderInput("first", LVecBase4i(first, 0, 0, 0)),
            }, LVecBase3i(_sky_w, _sky_h, 1));

        // Delta Sr
        exec_compute_shader(_shaders.at("delta_sr"), {
                ShaderInput("transmittanceSampler", _textures.at("transmittance")->get_texture()),
                ShaderInput("deltaJSampler", _textures.at("delta_j")->get_texture()),
                ShaderInput("dest", _textures.at("delta_sr")->get_texture()),
                ShaderInput("first", LVecBase4i(first, 0, 0, 0)),
            }, LVecBase3i(_res_mu_s_nu, _res_mu, _res_r), LVecBase3i(8, 8, 8));

        // Add delta E to irradiance
        exec_compute_shader(_shaders.at("add_delta_e"), {
                ShaderInput("deltaESampler", _textures.at("delta_e")->get_texture()),
                ShaderInput("dest", _textures.at("irradiance")->get_texture()),
            }, LVecBase3i(_sky_w, _sky_h, 1));

        // Add deltaSr to inscatter texture
        exec_compute_shader(_shaders.at("add_delta_sr"), {
                ShaderInput("deltaSSampler", _textures.at("delta_sr")->get_texture()),
                ShaderInput("dest", _textures.at("inscatter")->get_texture()),
            }, LVecBase3i(_res_mu_s_nu, _res_mu, _res_r), LVecBase3i(8, 8, 8));
    }

    // Make stages available
    for (auto&& stage: std::vector<rpcore::RenderStage*>({handle_.get_display_stage(), handle_.get_envmap_stage()}))
    {
        stage->set_shader_input(ShaderInput("InscatterSampler", _textures.at("inscatter")->get_texture()));
        stage->set_shader_input(ShaderInput("transmittanceSampler", _textures.at("transmittance")->get_texture()));
        stage->set_shader_input(ShaderInput("IrradianceSampler", _textures.at("irradiance")->get_texture()));
    }
}

void ScatteringMethodEricBruneton::create_textures()
{
    const std::string tex_format = _use_32_bit ? "RGBA32" : "RGBA16";

    _textures.clear();
    _textures.emplace("transmittance", rpcore::Image::create_2d("scat-trans", _trans_w, _trans_h, tex_format));
    _textures.emplace("irradiance", rpcore::Image::create_2d("scat-irrad", _sky_w, _sky_h, tex_format));
    _textures.emplace("inscatter", rpcore::Image::create_3d("scat-inscat", _res_mu_s_nu, _res_mu, _res_r, tex_format));
    _textures.emplace("delta_e", rpcore::Image::create_2d("scat-dx-e", _sky_w, _sky_h, tex_format));
    _textures.emplace("delta_sr", rpcore::Image::create_3d("scat-dx-sr", _res_mu_s_nu, _res_mu, _res_r, tex_format));
    _textures.emplace("delta_sm", rpcore::Image::create_3d("scat-dx-sm", _res_mu_s_nu, _res_mu, _res_r, tex_format));
    _textures.emplace("delta_j", rpcore::Image::create_3d("scat-dx-j", _res_mu_s_nu, _res_mu, _res_r, tex_format));

    for (const auto& key_img: _textures)
    {
        key_img.second->set_minfilter(SamplerState::FT_linear);
        key_img.second->set_magfilter(SamplerState::FT_linear);
        key_img.second->set_wrap_u(SamplerState::WM_clamp);
        key_img.second->set_wrap_v(SamplerState::WM_clamp);
        key_img.second->set_wrap_w(SamplerState::WM_clamp);
    }
}

void ScatteringMethodEricBruneton::create_shaders()
{
    _shaders.clear();

    const Filename& resource_path = handle_.get_shader_resource("eric_bruneton");
    for (const auto& fname: rppanda::listdir(resource_path))
    {
        const Filename& fpath = rppanda::join(resource_path, fname);

        size_t pos;
        if (rppanda::isfile(fpath) && ((pos=fname.rfind(".compute.glsl")) != std::string::npos &&
            fname.substr(pos).length() == std::string(".compute.glsl").length()))
        {
            const std::string& shader_name = fname.substr(0, fname.find("."));
            _shaders[shader_name] = rpcore::RPLoader::load_shader({fpath});
        }
    }
}

}    // namespace rpplugins
