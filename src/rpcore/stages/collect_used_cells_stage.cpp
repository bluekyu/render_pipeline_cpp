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

CollectUsedCellsStage::ProduceType CollectUsedCellsStage::get_produced_pipes() const
{
    return {
        ShaderInput("CellListBuffer", cell_list_buffer_->get_texture()),
        ShaderInput("CellIndices", cell_index_buffer_->get_texture()),
    };
}

void CollectUsedCellsStage::create()
{
    target_ = create_target("CollectUsedCells").get();
    target_->set_size(0);
    target_->prepare_buffer();

    cell_list_buffer_ = Image::create_buffer("CellList", 0, "R32I");
    cell_index_buffer_ = Image::create_2d_array("CellIndices", 0, 0, 0, "R32I");

    target_->set_shader_input(ShaderInput("CellListBuffer", cell_list_buffer_->get_texture()));
    target_->set_shader_input(ShaderInput("CellListIndices", cell_index_buffer_->get_texture()));
}

void CollectUsedCellsStage::reload_shaders()
{
    target_->set_shader(load_shader({ "collect_used_cells.frag.glsl" }));
}

void CollectUsedCellsStage::update()
{
    cell_list_buffer_->clear_image();
    cell_index_buffer_->clear_image();
}

void CollectUsedCellsStage::set_dimensions()
{
    const auto& tile_amount = pipeline_.get_light_mgr()->get_num_tiles();
    int num_slices = pipeline_.get_setting<int>("lighting.culling_grid_slices");
    int max_cells = tile_amount.get_x() * tile_amount.get_y() * num_slices;

    cell_list_buffer_->set_x_size(1 + max_cells);
    cell_index_buffer_->set_x_size(tile_amount.get_x());
    cell_index_buffer_->set_y_size(tile_amount.get_y());

    // in stereo mode, set double slices.
    cell_index_buffer_->set_z_size(num_slices);

    cell_list_buffer_->clear_image();
    cell_index_buffer_->clear_image();

    target_->set_size(tile_amount);
}

std::string CollectUsedCellsStage::get_plugin_id() const
{
    return std::string("render_pipeline_internal");
}

}
