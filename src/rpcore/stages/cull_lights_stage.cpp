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

#include "render_pipeline/rpcore/stages/cull_lights_stage.hpp"

#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/light_manager.hpp"

namespace rpcore {

CullLightsStage::RequireType CullLightsStage::required_inputs_ = { "AllLightsData", "maxLightIndex" };
CullLightsStage::RequireType CullLightsStage::required_pipes_ = { "CellListBuffer" };

CullLightsStage::CullLightsStage(RenderPipeline& pipeline): RenderStage(pipeline, "CullLightsStage")
{
    max_lights_per_cell_ = pipeline_.get_setting<int>("lighting.max_lights_per_cell");

    if (max_lights_per_cell_ > (1<<16))
        fatal(std::string("lighting.max_lights_per_cell must be <=") + std::to_string(1<<16) + "!");

    slice_width_ = pipeline_.get_setting<int>("lighting.culling_slice_width");
    cull_threads_ = 32;

    // Amount of light classes.Has to match the ones in LightClassification.inc.glsl
    num_light_classes_ = 4;
}

CullLightsStage::ProduceType CullLightsStage::get_produced_pipes() const
{
    return {
        ShaderInput("PerCellLights", grouped_cell_lights_->get_texture()),
        ShaderInput("PerCellLightsCounts", grouped_cell_lights_counts_->get_texture()),
    };
}

CullLightsStage::DefinesType CullLightsStage::get_produced_defines() const
{
    return {
        { "LC_SLICE_WIDTH", std::to_string(slice_width_) },
        { "LC_CULL_THREADS", std::to_string(cull_threads_) },
        { "LC_LIGHT_CLASS_COUNT", std::to_string(num_light_classes_) },
    };
}

void CullLightsStage::create()
{
    target_visible_ = create_target("GetVisibleLights").get();
    target_visible_->set_size(16);
    target_visible_->prepare_buffer();

    target_cull_ = create_target("CullLights").get();
    target_cull_->set_size(0);
    target_cull_->prepare_buffer();

    target_group_ = create_target("GroupLightsByClass").get();
    target_group_->set_size(0);
    target_group_->prepare_buffer();

    frustum_lights_ctr_ = Image::create_counter("VisibleLightCount");
    frustum_lights_ = Image::create_buffer("FrustumLights", pipeline_.get_light_mgr()->max_lights_, "R16UI");
    per_cell_lights_ = Image::create_buffer("PerCellLights", 0, "R16UI");
    // Needs to be R32 for atomic add in cull stage
    per_cell_light_counts_ = Image::create_buffer("PerCellLightCounts", 0, "R32I");
    grouped_cell_lights_ = Image::create_buffer("GroupedPerCellLights", 0, "R16UI");
    grouped_cell_lights_counts_ = Image::create_buffer("GroupedPerCellLightsCount", 0, "R16UI");

    target_visible_->set_shader_input(ShaderInput("FrustumLights", frustum_lights_->get_texture()));
    target_visible_->set_shader_input(ShaderInput("FrustumLightsCount", frustum_lights_ctr_->get_texture()));
    target_cull_->set_shader_input(ShaderInput("PerCellLightsBuffer", per_cell_lights_->get_texture()));
    target_cull_->set_shader_input(ShaderInput("PerCellLightCountsBuffer", per_cell_light_counts_->get_texture()));
    target_cull_->set_shader_input(ShaderInput("FrustumLights", frustum_lights_->get_texture()));
    target_cull_->set_shader_input(ShaderInput("FrustumLightsCount", frustum_lights_ctr_->get_texture()));
    target_group_->set_shader_input(ShaderInput("PerCellLightsBuffer", per_cell_lights_->get_texture()));
    target_group_->set_shader_input(ShaderInput("PerCellLightCountsBuffer", per_cell_light_counts_->get_texture()));
    target_group_->set_shader_input(ShaderInput("GroupedCellLightsBuffer", grouped_cell_lights_->get_texture()));
    target_group_->set_shader_input(ShaderInput("GroupedPerCellLightsCountBuffer", grouped_cell_lights_counts_->get_texture()));

    target_cull_->set_shader_input(ShaderInput("threadCount", LVecBase4i(cull_threads_, 0, 0, 0)));
    target_group_->set_shader_input(ShaderInput("threadCount", LVecBase4i(1, 0, 0, 0)));
}

void CullLightsStage::reload_shaders()
{
    target_cull_->set_shader(load_shader({ "tiled_culling.vert.glsl", "cull_lights.frag.glsl" }));
    target_group_->set_shader(load_shader({ "tiled_culling.vert.glsl", "group_lights.frag.glsl" }));
    target_visible_->set_shader(load_shader({ "view_frustum_cull.frag.glsl" }));
}

void CullLightsStage::update()
{
    frustum_lights_ctr_->clear_image();
}

void CullLightsStage::set_dimensions()
{
    int max_cells = pipeline_.get_light_mgr()->get_total_tiles();
    int num_rows_threaded = static_cast<int>(std::ceil((max_cells * cull_threads_) / static_cast<float>(slice_width_)));
    int num_rows = static_cast<int>(std::ceil(max_cells / static_cast<float>(slice_width_)));
    per_cell_lights_->set_x_size(max_cells * max_lights_per_cell_);
    per_cell_light_counts_->set_x_size(max_cells);
    grouped_cell_lights_->set_x_size(max_cells * (max_lights_per_cell_ + num_light_classes_));
    target_cull_->set_size(slice_width_, num_rows_threaded);
    target_group_->set_size(slice_width_, num_rows);
    grouped_cell_lights_counts_->set_x_size(max_cells * (1 + num_light_classes_));
}

std::string CullLightsStage::get_plugin_id() const
{
    return std::string("render_pipeline_internal");
}

}
