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

FlagUsedCellsStage::RequireType FlagUsedCellsStage::required_inputs;
FlagUsedCellsStage::RequireType FlagUsedCellsStage::required_pipes = { "GBuffer" };

FlagUsedCellsStage::ProduceType FlagUsedCellsStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("FlaggedCells", _cell_grid_flags->get_texture()),
    };
}

void FlagUsedCellsStage::create(void)
{
    _target = create_target("FlagUsedCells");
    _target->prepare_buffer();

    int culling_grid_slices = pipeline_.get_setting<int>("lighting.culling_grid_slices");

    _cell_grid_flags = Image::create_2d_array(
        "CellGridFlags", 0, 0,
        culling_grid_slices,
        "R8");

    _target->set_shader_input(ShaderInput("cellGridFlags", _cell_grid_flags->get_texture()));
}

void FlagUsedCellsStage::reload_shaders(void)
{
    _target->set_shader(load_shader({"flag_used_cells.frag.glsl"}));
}

void FlagUsedCellsStage::update(void)
{
    _cell_grid_flags->clear_image();
}

void FlagUsedCellsStage::set_dimensions(void)
{
    const LVecBase2i& tile_amount = pipeline_.get_light_mgr()->get_num_tiles();
    _cell_grid_flags->set_x_size(tile_amount.get_x());
    _cell_grid_flags->set_y_size(tile_amount.get_y());
}

std::string FlagUsedCellsStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
