/**
 * Render Pipeline C++
 *
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

#include <shaderInput.h>

#include <unordered_map>
#include <typeindex>

#include <boost/variant.hpp>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpcore {

class RenderStage;

/**
 * Simplest possible uniform buffer which just stores a set of values.
 * This does not use any fancy uniform buffer objects under the hood, and
 * instead just sets every value as a shader input.
 */
class SimpleInputBlock: public RPObject
{
public:
    using InputsType = std::unordered_map<std::string, ShaderInput>;

    /** Creates the ubo with the given name. */
    SimpleInputBlock(const std::string& name): RPObject("SimpleInputBlock"), name(name) {}

    /** Adds a new input to the UBO. */
    void add_input(const std::string& id, Texture *tex);
    void add_input(const std::string& id, Texture *tex, const SamplerState &sampler);
    void add_input(const std::string& id, Texture *tex, bool read, bool write, int z=-1, int n=0);
    void add_input(const std::string& id, int n1, int n2=0, int n3=0, int n4=0);
    void add_input(const std::string& id, PN_stdfloat n1, PN_stdfloat n2=0, PN_stdfloat n3=0, PN_stdfloat n4=0);

    template <class T>
    void add_input(const std::string& id, const T& np);

    void bind_to(const std::shared_ptr<RenderStage>& target);

    const std::string& get_name(void) const;

private:
    InputsType inputs;
    const std::string name;
};

// ************************************************************************************************
inline void SimpleInputBlock::add_input(const std::string& id, Texture *tex)
{
    inputs[id] = ShaderInput(name + "." + id, tex);
}

inline void SimpleInputBlock::add_input(const std::string& id, Texture *tex, const SamplerState &sampler)
{
    inputs[id] = ShaderInput(name + "." + id, tex, sampler);
}

inline void SimpleInputBlock::add_input(const std::string& id, Texture *tex, bool read, bool write, int z, int n)
{
    inputs[id] = ShaderInput(name + "." + id, tex, read, write, z, n);
}

inline void SimpleInputBlock::add_input(const std::string& id, int n1, int n2, int n3, int n4)
{
    inputs[id] = ShaderInput(name + "." + id, LVecBase4i(n1, n2, n3, n4));
}

inline void SimpleInputBlock::add_input(const std::string& id, PN_stdfloat n1, PN_stdfloat n2, PN_stdfloat n3, PN_stdfloat n4)
{
    inputs[id] = ShaderInput(name + "." + id, LVecBase4(n1, n2, n3, n4));
}

template <class T>
inline void SimpleInputBlock::add_input(const std::string& id, const T& np)
{
    // XXX: force to set priority.
    inputs[id] = ShaderInput(name + "." + id, np, 0);
}

inline const std::string& SimpleInputBlock::get_name(void) const
{
    return name;
}



/**
 * Grouped uniform buffer which either uses PointerToArray's to efficiently
 * store and update the shader inputs, or in case of uniform buffer object (UBO)
 * support, uses these to pass the inputs to the shaders.
 */
class GroupedInputBlock: public RPObject
{
public:
    enum class PTA_ID: int
    {
        INVALID = -1,
        PTA_int = 0,
        PTA_float,
        PTA_LVecBase2f,
        PTA_LVecBase2i,
        PTA_LVecBase3f,
        PTA_LVecBase4f,
        PTA_LMatrix3f,
        PTA_LMatrix4f
    };

private:
    std::unordered_map<std::string, boost::variant<
        PTA_int,
        PTA_float,
        PTA_LVecBase2f,
        PTA_LVecBase2i,
        PTA_LVecBase3f,
        PTA_LVecBase4f,
        PTA_LMatrix3f,
        PTA_LMatrix4f>> _ptas;

public:
    // Keeps track of the global allocated input blocks to be able to assign
    // a unique binding to all of them
    static int UBO_BINDING_INDEX;

    static const std::vector<std::string> PTA_MAPPINGS;

    /** Constructs the input block with a given name. */
    GroupedInputBlock(const std::string& name);

    /** Registers a new input, type should be a glsl type. */
    void register_pta(const std::string& name, const std::string& input_type);

    /** Converts a PtaXXX to a glsl type. */
    const std::string& pta_to_glsl_type(PTA_ID which) const;

    /** Converts a glsl type to a PtaXXX type. */
    PTA_ID glsl_type_to_pta(const std::string& glsl_type) const;

    /**
     * Binds all inputs of this UBO to the given target, which may be
     * either a RenderTarget or a NodePath.
     */
    void bind_to(const std::shared_ptr<RenderStage>& target);

    /** Updates an existing input. */
    void update_input(const std::string& name, int value, size_t index=0);
    void update_input(const std::string& name, float value, size_t index=0);
    void update_input(const std::string& name, const LVecBase2f& value, size_t index=0);
    void update_input(const std::string& name, const LVecBase2i& value, size_t index=0);
    void update_input(const std::string& name, const LVecBase3f& value, size_t index=0);
    void update_input(const std::string& name, const LVecBase4f& value, size_t index=0);
    void update_input(const std::string& name, const LMatrix3f& value, size_t index=0);
    void update_input(const std::string& name, const LMatrix4f& value, size_t index=0);

    /** Returns the value of an existing input. */
    const decltype(_ptas)::mapped_type& get_input(const std::string& name) const;

    /** Generates the GLSL shader code to use the UBO. */
    std::string generate_shader_code(void) const;

    const std::string& get_name(void) const;

private:
    const std::string _name;
    bool _use_ubo;
    int _bind_id;
};

// ************************************************************************************************
inline const std::string& GroupedInputBlock::get_name(void) const
{
    return _name;
}

inline const decltype(GroupedInputBlock::_ptas)::mapped_type& GroupedInputBlock::get_input(const std::string& name) const
{
    return _ptas.at(name);
}

}
