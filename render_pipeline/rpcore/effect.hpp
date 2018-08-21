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
#include <memory>

#include <render_pipeline/rpcore/rpobject.hpp>

class Shader;
class Filename;

namespace rpcore {

class RenderPipeline;

/**
 * This class represents an instance of a compiled effect. It can be loaded
 * from a file.
 */
class RENDER_PIPELINE_DECL Effect : public RPObject
{
public:
    using OptionType = std::unordered_map<std::string, bool>;

public:
    static std::shared_ptr<Effect> load(RenderPipeline& pipeline, const Filename& filename, const OptionType& options);
    static const OptionType& get_default_options();

    Effect();
    Effect(const Effect&) = delete;
    Effect(Effect&&);

    ~Effect();

    Effect& operator=(const Effect&) = delete;
    Effect& operator=(Effect&&);

    int get_effect_id() const;

    bool get_option(const std::string& name) const;

    /** Sets the effect options, overriding the default options. */
    void set_options(const OptionType& options);

    bool do_load(RenderPipeline& pipeline, const Filename& filename);

    /** Returns a handle to the compiled shader object for a given render pass. */
    Shader* get_shader_obj(const std::string& pass_id) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
