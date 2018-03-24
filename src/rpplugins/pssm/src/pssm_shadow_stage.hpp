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

namespace rpplugins {

/** This stage generates the depth-maps used for rendering PSSM. */
class PSSMShadowStage : public rpcore::RenderStage
{
public:
    PSSMShadowStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "PSSMShadowStage") {}

    RequireType& get_required_inputs() const final { return required_inputs; }
    RequireType& get_required_pipes() const final { return required_pipes; }
    ProduceType get_produced_pipes() const final;

    void create() final;

    void set_shader_input(const ShaderInput& inp) final;

    SamplerState make_pcf_state() const;

    Texture* get_shadow_tex() const;

    void set_num_splits(int num_splits);
    void set_split_resolution(int split_resolution);

    const std::vector<PT(DisplayRegion)>& get_split_regions() const;

private:
    std::string get_plugin_id() const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    int _num_splits = 3;
    int _split_resolution = 512;
    std::vector<PT(DisplayRegion)> _split_regions;

    rpcore::RenderTarget* _target;
};

inline void PSSMShadowStage::set_num_splits(int num_splits)
{
    _num_splits = num_splits;
}

inline void PSSMShadowStage::set_split_resolution(int split_resolution)
{
    _split_resolution = split_resolution;
}

inline const std::vector<PT(DisplayRegion)>& PSSMShadowStage::get_split_regions() const
{
    return _split_regions;
}

}
