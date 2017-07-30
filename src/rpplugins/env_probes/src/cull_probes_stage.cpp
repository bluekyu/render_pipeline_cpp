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
