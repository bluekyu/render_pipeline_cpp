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

#include <render_pipeline/rpcore/render_stage.hpp>
#include <render_pipeline/rpcore/image.hpp>

namespace rpcore {

class RenderTarget;

/**
 * This stage collects the flagged cells from the FlagUsedCellsStage and
 * makes a list of them.
 */
class CollectUsedCellsStage: public RenderStage
{
public:
    CollectUsedCellsStage(RenderPipeline& pipeline): RenderStage(pipeline, "CollectUsedCellsStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

    void update(void) final;
    void set_dimensions(void) final;

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    std::shared_ptr<Image> _cell_list_buffer;
    std::shared_ptr<Image> _cell_index_buffer;

    std::shared_ptr<RenderTarget> _target = nullptr;
};

}
