#include "cull_probes_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/light_manager.hpp>

namespace rpplugins {

CullProbesStage::RequireType CullProbesStage::required_inputs = { "EnvProbes" };
CullProbesStage::RequireType CullProbesStage::required_pipes = { "CellListBuffer" };

CullProbesStage::CullProbesStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "CullProbesStage")
{
    _slice_width = pipeline_.get_setting<int>("lighting.culling_slice_width");
}

CullProbesStage::ProduceType CullProbesStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("PerCellProbes", _per_cell_probes->get_texture()),
    };
}

CullProbesStage::DefinesType CullProbesStage::get_produced_defines(void) const
{
    return {
        { "MAX_PROBES_PER_CELL", std::to_string(_max_probes_per_cell) }
    };
}

void CullProbesStage::create(void)
{
    _target = create_target("CullProbes");
    _target->set_size(0);
    _target->prepare_buffer();

    _per_cell_probes = rpcore::Image::create_buffer("PerCellProbes", 0, "R32I");
    _per_cell_probes->clear_image();
    _target->set_shader_input(ShaderInput("PerCellProbes", _per_cell_probes->get_texture()));
    _target->set_shader_input(ShaderInput("threadCount", LVecBase4i(1, 0, 0, 0)));
}

void CullProbesStage::set_dimensions(void)
{
    int max_cells = pipeline_.get_light_mgr()->get_total_tiles();
    int num_rows = int(std::ceil(max_cells / float(_slice_width)));
    _per_cell_probes->set_x_size(max_cells * _max_probes_per_cell);
    _target->set_size(_slice_width, num_rows);
}

void CullProbesStage::reload_shaders(void)
{
    _target->set_shader(load_plugin_shader({"/$$rp/shader/tiled_culling.vert.glsl", "cull_probes.frag.glsl"}));
}

std::string CullProbesStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
