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

#include "render_pipeline/rpcore/stages/collect_used_cells_stage.hpp"

#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/light_manager.hpp"

namespace rpcore {

CollectUsedCellsStage::RequireType CollectUsedCellsStage::required_inputs;
CollectUsedCellsStage::RequireType CollectUsedCellsStage::required_pipes = { "FlaggedCells" };

CollectUsedCellsStage::ProduceType CollectUsedCellsStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("CellListBuffer", _cell_list_buffer->get_texture()),
        ShaderInput("CellIndices", _cell_index_buffer->get_texture()),
    };
}

void CollectUsedCellsStage::create(void)
{
    _target = create_target("CollectUsedCells");
    _target->set_size(0);
    _target->prepare_buffer();

    _cell_list_buffer = Image::create_buffer("CellList", 0, "R32I");
    _cell_index_buffer = Image::create_2d_array("CellIndices", 0, 0, 0, "R32I");

    _target->set_shader_input(ShaderInput("CellListBuffer", _cell_list_buffer->get_texture()));
    _target->set_shader_input(ShaderInput("CellListIndices", _cell_index_buffer->get_texture()));
}

void CollectUsedCellsStage::reload_shaders(void)
{
    _target->set_shader(load_shader({ "collect_used_cells.frag.glsl" }));
}

void CollectUsedCellsStage::update(void)
{
    _cell_list_buffer->clear_image();
    _cell_index_buffer->clear_image();
}

void CollectUsedCellsStage::set_dimensions(void)
{
    const auto& tile_amount = pipeline_.get_light_mgr()->get_num_tiles();
    int num_slices = pipeline_.get_setting<int>("lighting.culling_grid_slices");
    int max_cells = tile_amount.get_x() * tile_amount.get_y() * num_slices;

    _cell_list_buffer->set_x_size(1 + max_cells);
    _cell_index_buffer->set_x_size(tile_amount.get_x());
    _cell_index_buffer->set_y_size(tile_amount.get_y());

    // in stereo mode, set double slices.
    _cell_index_buffer->set_z_size(num_slices);

    _cell_list_buffer->clear_image();
    _cell_index_buffer->clear_image();

    _target->set_size(tile_amount);
}

std::string CollectUsedCellsStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
