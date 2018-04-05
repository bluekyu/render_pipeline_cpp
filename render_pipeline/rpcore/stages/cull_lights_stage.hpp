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

class Image;

/** This stage takes the list of used cells and creates a list of lights for each cell.*/
class RENDER_PIPELINE_DECL CullLightsStage : public RenderStage
{
public:
    CullLightsStage(RenderPipeline& pipeline);

    static RequireType& get_global_required_inputs() { return required_inputs_; }
    static RequireType& get_global_required_pipes() { return required_pipes_; }

    RequireType& get_required_inputs() const final { return required_inputs_; }
    RequireType& get_required_pipes() const final { return required_pipes_; }
    ProduceType get_produced_pipes() const final;
    DefinesType get_produced_defines() const final;

    void create() final;
    void reload_shaders() final;

    void update() final;
    void set_dimensions() final;

private:
    std::string get_plugin_id() const final;

    static RequireType required_inputs_;
    static RequireType required_pipes_;

    int max_lights_per_cell_;
    int slice_width_;
    int cull_threads_;
    int num_light_classes_;

    std::unique_ptr<Image> frustum_lights_ctr_;
    std::unique_ptr<Image> frustum_lights_;
    std::unique_ptr<Image> per_cell_lights_;
    std::unique_ptr<Image> per_cell_light_counts_;
    std::unique_ptr<Image> grouped_cell_lights_;
    std::unique_ptr<Image> grouped_cell_lights_counts_;

    RenderTarget* target_visible_;
    RenderTarget* target_cull_;
    RenderTarget* target_group_;
};

}
