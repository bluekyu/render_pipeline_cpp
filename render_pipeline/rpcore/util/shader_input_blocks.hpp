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

#include <shaderInput.h>

#include <unordered_map>

#include <boost/variant.hpp>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpcore {

class RenderStage;

/**
 * Simplest possible uniform buffer which just stores a set of values.
 * This does not use any fancy uniform buffer objects under the hood, and
 * instead just sets every value as a shader input.
 */
class RENDER_PIPELINE_DECL SimpleInputBlock : public RPObject
{
public:
    using InputsType = std::unordered_map<std::string, ShaderInput>;

    /** Creates the ubo with the given name. */
    SimpleInputBlock(const std::string& name): RPObject("SimpleInputBlock"), name_(name) {}

    /** Adds a new input to the UBO. */
    void add_input(const std::string& id, Texture *tex);
    void add_input(const std::string& id, Texture *tex, const SamplerState &sampler);
    void add_input(const std::string& id, Texture *tex, bool read, bool write, int z=-1, int n=0);
    void add_input(const std::string& id, int n1, int n2=0, int n3=0, int n4=0);
    void add_input(const std::string& id, PN_stdfloat n1, PN_stdfloat n2=0, PN_stdfloat n3=0, PN_stdfloat n4=0);

    template <class T>
    void add_input(const std::string& id, const T& np);

    void bind_to(RenderStage* target) const;

    const std::string& get_name() const;

private:
    InputsType inputs_;
    const std::string name_;
};

// ************************************************************************************************
inline void SimpleInputBlock::add_input(const std::string& id, Texture *tex)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    inputs_.insert_or_assign(id, ShaderInput(name_ + "." + id, tex));
#else
    inputs_[id] = ShaderInput(name_ + "." + id, tex);
#endif
}

inline void SimpleInputBlock::add_input(const std::string& id, Texture *tex, const SamplerState &sampler)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    inputs_.insert_or_assign(id, ShaderInput(name_ + "." + id, tex, sampler));
#else
    inputs_[id] = ShaderInput(name_ + "." + id, tex, sampler);
#endif
}

inline void SimpleInputBlock::add_input(const std::string& id, Texture *tex, bool read, bool write, int z, int n)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    inputs_.insert_or_assign(id, ShaderInput(name_ + "." + id, tex, read, write, z, n));
#else
    inputs_[id] = ShaderInput(name_ + "." + id, tex, read, write, z, n);
#endif
}

inline void SimpleInputBlock::add_input(const std::string& id, int n1, int n2, int n3, int n4)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    inputs_.insert_or_assign(id, ShaderInput(name_ + "." + id, LVecBase4i(n1, n2, n3, n4)));
#else
    inputs_[id] = ShaderInput(name_ + "." + id, LVecBase4i(n1, n2, n3, n4));
#endif
}

inline void SimpleInputBlock::add_input(const std::string& id, PN_stdfloat n1, PN_stdfloat n2, PN_stdfloat n3, PN_stdfloat n4)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    inputs_.insert_or_assign(id, ShaderInput(name_ + "." + id, LVecBase4f(n1, n2, n3, n4)));
#else
    inputs_[id] = ShaderInput(name_ + "." + id, LVecBase4f(n1, n2, n3, n4));
#endif
}

template <class T>
inline void SimpleInputBlock::add_input(const std::string& id, const T& np)
{
    // XXX: force to set priority.
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    inputs_.insert_or_assign(id, ShaderInput(name_ + "." + id, np, 0));
#else
    inputs_[id] = ShaderInput(name_ + "." + id, np, 0);
#endif
}

inline const std::string& SimpleInputBlock::get_name() const
{
    return name_;
}



/**
 * Grouped uniform buffer which either uses PointerToArray's to efficiently
 * store and update the shader inputs, or in case of uniform buffer object (UBO)
 * support, uses these to pass the inputs to the shaders.
 */
class RENDER_PIPELINE_DECL GroupedInputBlock : public RPObject
{
public:
    using PTA_Types = boost::variant<
        PTA_int,
        PTA_float,
        PTA_LVecBase2f,
        PTA_LVecBase2i,
        PTA_LVecBase3f,
        PTA_LVecBase4f,
        PTA_LMatrix3f,
        PTA_LMatrix4f>;

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
    void bind_to(RenderStage* target) const;

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
    const PTA_Types& get_input(const std::string& name) const;

    /** Generates the GLSL shader code to use the UBO. */
    std::string generate_shader_code() const;

    const std::string& get_name() const;

private:
    const std::string name_;

    std::unordered_map<std::string, PTA_Types> ptas_;
    bool use_ubo_;
    int bind_id_;
};

// ************************************************************************************************
inline void GroupedInputBlock::update_input(const std::string& name, int value, size_t index)
{
    boost::get<PTA_int&>(ptas_.at(name))[index] = value;
}

inline void GroupedInputBlock::update_input(const std::string& name, float value, size_t index)
{
    boost::get<PTA_float&>(ptas_.at(name))[index] = value;
}

inline void GroupedInputBlock::update_input(const std::string& name, const LVecBase2f& value, size_t index)
{
    boost::get<PTA_LVecBase2f&>(ptas_.at(name))[index] = value;
}

inline void GroupedInputBlock::update_input(const std::string& name, const LVecBase2i& value, size_t index)
{
    boost::get<PTA_LVecBase2i&>(ptas_.at(name))[index] = value;
}

inline void GroupedInputBlock::update_input(const std::string& name, const LVecBase3f& value, size_t index)
{
    boost::get<PTA_LVecBase3f&>(ptas_.at(name))[index] = value;
}

inline void GroupedInputBlock::update_input(const std::string& name, const LVecBase4f& value, size_t index)
{
    boost::get<PTA_LVecBase4f&>(ptas_.at(name))[index] = value;
}

inline void GroupedInputBlock::update_input(const std::string& name, const LMatrix3f& value, size_t index)
{
    boost::get<PTA_LMatrix3f&>(ptas_.at(name))[index] = value;
}

inline void GroupedInputBlock::update_input(const std::string& name, const LMatrix4f& value, size_t index)
{
    boost::get<PTA_LMatrix4f&>(ptas_.at(name))[index] = value;
}

inline const std::string& GroupedInputBlock::get_name() const
{
    return name_;
}

inline const GroupedInputBlock::PTA_Types& GroupedInputBlock::get_input(const std::string& name) const
{
    return ptas_.at(name);
}

}
