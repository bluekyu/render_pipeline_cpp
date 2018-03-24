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

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpcore {

/**
 * This stage is constructed by the StageManager and stores all the
 * current pipes in the previous pipe storage.
 * This stage is a bit special, and not like the other stages, it does not
 * specify inputs, since the StageManager passes all required inputs on demand.
 * Also this stage does not load any shaders, but creates them on the fly.
 */
class UpdatePreviousPipesStage : public RenderStage
{
public:
    UpdatePreviousPipesStage(RenderPipeline& pipeline): RenderStage(pipeline, "UpdatePreviousPipesStage") {}

    RequireType& get_required_inputs() const final;
    RequireType& get_required_pipes() const final;

    /**
     * Adds a new texture which should be copied from "from_tex" to
     * "to_tex". This should be called before the stage gets constructed.
     * The source texture is expected to have the same size as the render
     * resolution.
     */
    void add_transfer(Texture* from_tex, Texture* to_tex);

    void create() final;

    /** Sets the dimensions on all targets. See RenderTarget::set_dimensions. */
    void set_dimensions() final;

    void reload_shaders() final;

    std::string get_sampler_type(Texture* tex, bool can_write=false);

    /**
     * Returns the matching GLSL sampler lookup for a texture, storing the
     * result in the given glsl variable.
     */
    std::string get_sampler_lookup(Texture* tex, const std::string& dest_name,
        const std::string& sampler_name, const std::string& coord_var);

    /** Returns the matching GLSL code to store the given data in a given texture. */
    std::string get_store_code(Texture* tex, const std::string& sampler_name,
        const std::string& coord_var, const std::string& data_var);

private:
    std::string get_plugin_id() const final;

    RenderTarget* target_;
    std::vector<std::pair<Texture*, Texture*>> transfers_;
};

// ************************************************************************************************
inline void UpdatePreviousPipesStage::add_transfer(Texture* from_tex, Texture* to_tex)
{
    transfers_.push_back({ from_tex, to_tex });
}

}
