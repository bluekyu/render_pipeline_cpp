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

#include "render_pipeline/rpcore/util/shader_input_blocks.hpp"

#include <regex>

#include <boost/format.hpp>

#include "render_pipeline/rpcore/render_stage.hpp"
#include "render_pipeline/rpcore/util/post_process_region.hpp"

namespace rpcore {

void SimpleInputBlock::bind_to(const std::shared_ptr<RenderStage>& target) const
{
    for (const auto& key_val: inputs_)
        target->set_shader_input(key_val.second);
}

int GroupedInputBlock::UBO_BINDING_INDEX = 0;

const std::vector<std::string> GroupedInputBlock::PTA_MAPPINGS = { "int", "float", "vec2", "ivec2", "vec3", "vec4", "mat3", "mat4", };

GroupedInputBlock::GroupedInputBlock(const std::string& name): RPObject("GroupedInputBlock"), name_(name)
{
    use_ubo_ = bool(TypeRegistry::ptr()->find_type("GLUniformBufferContext"));

    // Acquire a unique index for each UBO to store its binding
    bind_id_ = GroupedInputBlock::UBO_BINDING_INDEX;
    GroupedInputBlock::UBO_BINDING_INDEX += 1;

    if (bind_id_ == 0)
    {
        // Only output the bind support debug output once(for the first ubo)
        debug(std::string("Native UBO support = ") + (use_ubo_ ? "true" : "false"));
    }
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#define MAKE_PTA(TYPE_NAME) \
    case PTA_ID::TYPE_NAME: \
        ptas_.insert_or_assign(uniform_name, TYPE_NAME::empty_array(array_size)); \
        break;
#else
#define MAKE_PTA(TYPE_NAME) \
    case PTA_ID::TYPE_NAME: \
        ptas_.insert({uniform_name, TYPE_NAME::empty_array(array_size)}); \
        break;
#endif

void GroupedInputBlock::register_pta(const std::string& name, const std::string& input_type)
{
    int array_size = 1;
    std::string uniform_name = name;

    static const std::regex array_size_regex("\\[([0-9]+)\\]");
    std::smatch match;
    std::regex_search(name, match, array_size_regex);
    if (match.size() > 2)
    {
        warn(std::string("Invalid uniform array syntax: ") + name);
        return;
    }
    else if (match.size() == 2)
    {
        array_size = std::stoi(match[1].str());
        if (array_size <= 0)
        {
            warn(std::string("Invalid uniform array size: ") + name);
            return;
        }
        uniform_name = match.prefix().str();
    }

    try
    {
        switch (glsl_type_to_pta(input_type))
        {
        MAKE_PTA(PTA_int)
        MAKE_PTA(PTA_float)
        MAKE_PTA(PTA_LVecBase2f)
        MAKE_PTA(PTA_LVecBase2i)
        MAKE_PTA(PTA_LVecBase3f)
        MAKE_PTA(PTA_LVecBase4f)
        MAKE_PTA(PTA_LMatrix3f)
        MAKE_PTA(PTA_LMatrix4f)

        default:
            throw std::out_of_range("Invalid type");
        }
    }
    catch (const std::out_of_range&)
    {
        nout << "Invalid Panda3D type for shader input: [" << input_type << "]" << std::endl;
    }
}

const std::string& GroupedInputBlock::pta_to_glsl_type(PTA_ID which) const
{
    try
    {
        return GroupedInputBlock::PTA_MAPPINGS.at(static_cast<int>(which));
    }
    catch (const std::out_of_range&)
    {
        error(std::string("Unrecognized PTA type."));

        static std::string empty_string("");
        return empty_string;
    }
}

GroupedInputBlock::PTA_ID GroupedInputBlock::glsl_type_to_pta(const std::string& glsl_type) const
{
    const int i_end = static_cast<int>(PTA_MAPPINGS.size());
    for (int i = 0; i < i_end; ++i)
    {
        if (PTA_MAPPINGS.at(i) == glsl_type)
            return static_cast<PTA_ID>(i);
    }
    error(std::string("Could not resolve GLSL type: ") + glsl_type);
    return PTA_ID::INVALID;
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#else
struct BindToVisitor : public boost::static_visitor<>
{
public:
    BindToVisitor(const std::shared_ptr<RenderStage>& target): target(target)
    {
    }

    template <class T>
    void operator()(const T& pta_handle)
    {
        target->set_shader_input(ShaderInput(input_name, pta_handle));
    }

    std::string input_name;

private:
    const std::shared_ptr<RenderStage>& target;
};
#endif

void GroupedInputBlock::bind_to(const std::shared_ptr<RenderStage>& target) const
{
    std::string name_prefix = name_;
    if (use_ubo_)
        name_prefix += "_UBO.";
    else
        name_prefix += ".";

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#else
    BindToVisitor btv(target);
#endif

    for (const auto& pta_name_handle: ptas_)
    {
        const std::string& input_name = name_prefix + pta_name_handle.first;

#if !defined(_MSC_VER) || _MSC_VER >= 1900
        boost::apply_visitor([&](const auto& pta_handle) {
            target->set_shader_input(ShaderInput(input_name, pta_handle));
        }, pta_name_handle.second);
#else
        btv.input_name = input_name;
        boost::apply_visitor(btv, pta_name_handle.second);
#endif
    }
}

std::string GroupedInputBlock::generate_shader_code() const
{
    std::string content("#pragma once\n\n");
    content += "// Autogenerated by the render pipeline\n";
    content += "// Do not edit! Your changes will be lost.\n\n";

    std::unordered_map<std::string, std::vector<std::string>> structs;
    std::vector<std::string> inputs;

    for (const auto& key_val: ptas_)
    {
        const std::string& input_name = key_val.first;
        const auto& handle = key_val.second;
        int array_size = 1;

        switch (static_cast<PTA_ID>(handle.which()))
        {
        case PTA_ID::PTA_int:
            array_size = boost::get<const PTA_int&>(handle).size();
            break;
        case PTA_ID::PTA_float:
            array_size = boost::get<const PTA_float&>(handle).size();
            break;
        case PTA_ID::PTA_LVecBase2f:
            array_size = boost::get<const PTA_LVecBase2f&>(handle).size();
            break;
        case PTA_ID::PTA_LVecBase2i:
            array_size = boost::get<const PTA_LVecBase2i&>(handle).size();
            break;
        case PTA_ID::PTA_LVecBase3f:
            array_size = boost::get<const PTA_LVecBase3f&>(handle).size();
            break;
        case PTA_ID::PTA_LVecBase4f:
            array_size = boost::get<const PTA_LVecBase4f&>(handle).size();
            break;
        case PTA_ID::PTA_LMatrix3f:
            array_size = boost::get<const PTA_LMatrix3f&>(handle).size();
            break;
        case PTA_ID::PTA_LMatrix4f:
            array_size = boost::get<const PTA_LMatrix4f&>(handle).size();
            break;
        default:
            throw std::out_of_range("Invalid type");
        }

        const std::regex dot_re("[.]");
        std::vector<std::string> parts(std::sregex_token_iterator(input_name.begin(), input_name.end(), dot_re, -1), std::sregex_token_iterator());

        // Single input, simply add it to the input list
        if (parts.size() == 1)
        {
            if (array_size == 1)
                inputs.push_back(pta_to_glsl_type(static_cast<PTA_ID>(handle.which())) + " " + input_name + ";");
            else
                inputs.push_back(pta_to_glsl_type(static_cast<PTA_ID>(handle.which())) + " " + input_name + "[" + std::to_string(array_size) + "];");
        }
        // Nested input, like scattering.sun_color
        else if (parts.size() == 2)
        {
            const std::string& struct_name = parts[0];
            const std::string& actual_input_name = parts[1];

            if (structs.find(struct_name) == structs.end())
            {
                // Construct a new struct and add it to the list of inputs
                inputs.push_back(struct_name + "_UBOSTRUCT " + struct_name + ";");
            }
            //else    Struct is already defined, add member definition
            
            if (array_size == 1)
                structs[struct_name].push_back(pta_to_glsl_type(static_cast<PTA_ID>(handle.which())) + " " + actual_input_name + ";");
            else
                structs[struct_name].push_back(pta_to_glsl_type(static_cast<PTA_ID>(handle.which())) + " " + actual_input_name + "[" + std::to_string(array_size) + "];");
        }
        // Nested input, like scattering.some_setting.sun_color, not supported yet
        else
        {
            warn(std::string("Structure definition too nested, not supported (yet): ") + input_name);
        }
    }

    // Add structures
    for (const auto& key_val: structs)
    {
        content += std::string("struct ") + key_val.first + "_UBOSTRUCT {\n";
        for (const auto& member: key_val.second)
            content += std::string(4, ' ') + member + "\n";
        content += "};\n\n";
    }

    // Add actual inputs
    if (inputs.size() < 1)
    {
        debug(std::string("No UBO inputs present for ") + name_);
    }
    else
    {
        if (use_ubo_)
        {
            content += (boost::format("layout(shared, binding=%1%) uniform %2%_UBO {\n") %
                bind_id_ % name_).str();
            for (const auto& ipt: inputs)
                content += std::string(4, ' ') + ipt + "\n";
            content += std::string("} ") + name_ + ";\n";
        }
        else
        {
            content += "uniform struct {\n";
            for (const auto& ipt: inputs)
                content += std::string(4, ' ') + ipt + "\n";
            content += std::string("} ") + name_ + ";\n";
        }
    }

    content += "\n";
    return content;
}

}
