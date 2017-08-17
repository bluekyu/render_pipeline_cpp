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

#include "render_pipeline/rpcore/stages/flag_used_cells_stage.hpp"

#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/light_manager.hpp"

namespace rpcore {

FlagUsedCellsStage::RequireType FlagUsedCellsStage::required_inputs_;
FlagUsedCellsStage::RequireType FlagUsedCellsStage::required_pipes_ = { "GBuffer" };

FlagUsedCellsStage::ProduceType FlagUsedCellsStage::get_produced_pipes() const
{
    return {
        ShaderInput("FlaggedCells", cell_grid_flags_->get_texture()),
    };
}

void FlagUsedCellsStage::create()
{
    target_ = create_target("FlagUsedCells");
    target_->prepare_buffer();

    int culling_grid_slices = pipeline_.get_setting<int>("lighting.culling_grid_slices");

    cell_grid_flags_ = Image::create_2d_array(
        "CellGridFlags", 0, 0,
        culling_grid_slices,
        "R8");

    target_->set_shader_input(ShaderInput("cellGridFlags", cell_grid_flags_->get_texture()));
}

void FlagUsedCellsStage::reload_shaders()
{
    target_->set_shader(load_shader({"flag_used_cells.frag.glsl"}));
}

void FlagUsedCellsStage::update()
{
    cell_grid_flags_->clear_image();
}

void FlagUsedCellsStage::set_dimensions()
{
    const LVecBase2i& tile_amount = pipeline_.get_light_mgr()->get_num_tiles();
    cell_grid_flags_->set_x_size(tile_amount.get_x());
    cell_grid_flags_->set_y_size(tile_amount.get_y());
}

std::string FlagUsedCellsStage::get_plugin_id() const
{
    return std::string("render_pipeline_internal");
}

}
