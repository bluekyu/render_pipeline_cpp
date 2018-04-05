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

#pragma once

#include <unordered_map>

#include <shader.h>

#include <render_pipeline/rpcore/rpobject.hpp>

#include "../include/scattering_plugin.hpp"

namespace rpcore {
class Image;
}

namespace rpplugins {

/** Base class for all scattering methods. */
class ScatteringMethod : public rpcore::RPObject
{
public:
    ScatteringMethod(ScatteringPlugin& plugin, const std::string& name="ScatteringMethod"): RPObject(name), handle_(plugin) {}

    ScatteringPlugin& get_handle() const { return handle_; }

    virtual void load() = 0;
    virtual void compute() = 0;

protected:
    ScatteringPlugin& handle_;
};

// ************************************************************************************************
/** Precomputed atmospheric scattering by Eric Bruneton. */
class ScatteringMethodEricBruneton : public ScatteringMethod
{
public:
    ScatteringMethodEricBruneton(ScatteringPlugin& plugin): ScatteringMethod(plugin, "ScatteringMethodEricBruneton") {}

    /** Inits parameters, those should match with the ones specified in common.glsl. */
    void load() final;

    /**
     * Executes a compute shader. The shader object should be a shader
     * loaded with Shader.load_compute, the shader inputs should be a dict where
     * the keys are the names of the shader inputs and the values are the
     * inputs. The workgroup_size has to match the size defined in the
     * compute shader.
     */
    void exec_compute_shader(const Shader* shader_obj, const std::vector<ShaderInput>& shader_inputs,
        const LVecBase3i& exec_size, const LVecBase3i& workgroup_size=LVecBase3i(16, 16, 1));

    /** Precomputes the scattering. */
    void compute() final;

    /** Creates all textures required for the scattering. */
    void create_textures();

    /** Creates all the shaders used for precomputing. */
    void create_shaders();

private:
    bool _use_32_bit;

    int _trans_w;
    int _trans_h;

    int _sky_w;
    int _sky_h;

    int _res_r;
    int _res_mu;
    int _res_mu_s;
    int _res_nu;

    int _res_mu_s_nu;

    std::unordered_map<std::string, std::unique_ptr<rpcore::Image>> _textures;
    std::unordered_map<std::string, PT(Shader)> _shaders;
};

}    // namespace rpplugins
