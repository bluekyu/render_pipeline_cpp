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
