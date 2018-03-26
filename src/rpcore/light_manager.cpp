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

#include "render_pipeline/rpcore/light_manager.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include "render_pipeline/rpcore/stages/apply_lights_stage.hpp"
#include "render_pipeline/rpcore/stages/shadow_stage.hpp"
#include "render_pipeline/rpcore/stages/flag_used_cells_stage.hpp"
#include "render_pipeline/rpcore/stages/collect_used_cells_stage.hpp"
#include "render_pipeline/rpcore/stages/cull_lights_stage.hpp"

#include "render_pipeline/rpcore/native/internal_light_manager.h"
#include "render_pipeline/rpcore/native/shadow_manager.h"
#include "render_pipeline/rpcore/native/rp_point_light.h"

#include "rpcore/gpu_command_queue.hpp"

namespace rpcore {

LightManager::LightManager(RenderPipeline& pipeline): RPObject("LightManager"), pipeline_(pipeline)
{
    compute_tile_size();
    init_internal_manager();
    init_command_queue();
    initshadow_manager();
    init_stages();
}

LightManager::~LightManager()
{
    internal_mgr_.reset();
    shadow_manager_.reset();

    StageManager* stage_mgr = pipeline_.get_stage_mgr();
    if (!stage_mgr)
        return;

    stage_mgr->remove_stage(flag_cells_stage_.get());
    stage_mgr->remove_stage(collect_cells_stage_.get());
    stage_mgr->remove_stage(cull_lights_stage_.get());
    stage_mgr->remove_stage(apply_lights_stage_.get());
    stage_mgr->remove_stage(shadow_stage_.get());
}

int LightManager::get_total_tiles() const
{
    return num_tiles_.get_x() * num_tiles_.get_y() * pipeline_.get_setting<int>("lighting.culling_grid_slices");
}

size_t LightManager::get_num_lights() const
{
    return internal_mgr_->get_num_lights();
}

size_t LightManager::get_num_shadow_sources() const
{
    return internal_mgr_->get_num_shadow_sources();
}

float LightManager::get_shadow_atlas_coverage() const
{
    return internal_mgr_->get_shadow_manager()->get_atlas()->get_coverage() * 100.0f;
}

void LightManager::add_light(PT(RPLight) light)
{
    internal_mgr_->add_light(light);
    pta_max_light_index_[0] = internal_mgr_->get_max_light_index();
}

void LightManager::remove_light(RPLight* light)
{
    internal_mgr_->remove_light(light);
    pta_max_light_index_[0] = internal_mgr_->get_max_light_index();
}

void LightManager::update()
{
    internal_mgr_->set_camera_pos(Globals::base->get_cam().get_pos(Globals::base->get_render()));
    internal_mgr_->update();
    shadow_manager_->update();
    cmd_queue_->process_queue();
}

void LightManager::reload_shaders()
{
    cmd_queue_->reload_shaders();
}

void LightManager::compute_tile_size()
{
    tile_size_ = LVecBase2i(
        pipeline_.get_setting<int>("lighting.culling_grid_size_x"),
        pipeline_.get_setting<int>("lighting.culling_grid_size_y"));

    int num_tiles_x = static_cast<int>(std::ceil(Globals::resolution.get_x() / static_cast<float>(tile_size_.get_x())));
    int num_tiles_y = static_cast<int>(std::ceil(Globals::resolution.get_y() / static_cast<float>(tile_size_.get_y())));
    debug(std::string("Tile size = ") + std::to_string(tile_size_.get_x()) + "x" + std::to_string(tile_size_.get_y()) +
        ", Num tiles = " + std::to_string(num_tiles_x) + "x" + std::to_string(num_tiles_y));
    num_tiles_ = LVecBase2i(num_tiles_x, num_tiles_y);
}

void LightManager::init_command_queue()
{
    cmd_queue_ = std::make_unique<GPUCommandQueue>(pipeline_);
    cmd_queue_->register_input("LightData", img_light_data_->get_texture());
    cmd_queue_->register_input("SourceData", img_source_data_->get_texture());
    internal_mgr_->set_command_list(cmd_queue_->get_command_list());
}

void LightManager::initshadow_manager()
{
    shadow_manager_ = std::make_unique<ShadowManager>();
    shadow_manager_->set_max_updates(pipeline_.get_setting<size_t>("shadows.max_updates"));
    shadow_manager_->set_scene(Globals::base->get_render());
    shadow_manager_->set_tag_state_manager(pipeline_.get_tag_mgr());
    shadow_manager_->set_atlas_size(pipeline_.get_setting<size_t>("shadows.atlas_size"));
    internal_mgr_->set_shadow_manager(shadow_manager_.get());
}

void LightManager::init_shadows()
{
    shadow_manager_->set_atlas_graphics_output(shadow_stage_->get_atlas_buffer());
    shadow_manager_->init();
}

void LightManager::init_internal_manager()
{
    internal_mgr_ = std::make_unique<InternalLightManager>();
    internal_mgr_->set_shadow_update_distance(pipeline_.get_setting<float>("shadows.max_update_distance"));
    
    // Storage for the Lights
    const int per_light_vec4s = 4;
    img_light_data_ = Image::create_buffer("LightData", max_lights_ * per_light_vec4s, "RGBA16");
    img_light_data_->clear_image();
    
    pta_max_light_index_ = PTA_int::empty_array(1);
    pta_max_light_index_[0] = 0;
    
    // Storage for the shadow sources
    const int per_source_vec4s = 5;
    
    // IMPORTANT: RGBA32 is really required here.Otherwise artifacts and bad
    // shadow filtering occur due to precision issues
    img_source_data_ = Image::create_buffer("ShadowSourceData", max_sources_ * per_source_vec4s, "RGBA32");

    // TODO: check if this is right.
    img_source_data_->clear_image();
    
    // Register the buffer
    pipeline_.get_stage_mgr()->add_input(ShaderInput("AllLightsData", img_light_data_->get_texture()));
    pipeline_.get_stage_mgr()->add_input(ShaderInput("ShadowSourceData", img_source_data_->get_texture()));
    pipeline_.get_stage_mgr()->add_input(ShaderInput("maxLightIndex", pta_max_light_index_));
}

void LightManager::init_stages()
{
    StageManager* stage_mgr = pipeline_.get_stage_mgr();
    
    flag_cells_stage_ = std::make_unique<FlagUsedCellsStage>(pipeline_);
    stage_mgr->add_stage(flag_cells_stage_.get());

    collect_cells_stage_ = std::make_unique<CollectUsedCellsStage>(pipeline_);
    stage_mgr->add_stage(collect_cells_stage_.get());

    cull_lights_stage_ = std::make_unique<CullLightsStage>(pipeline_);
    stage_mgr->add_stage(cull_lights_stage_.get());

    apply_lights_stage_ = std::make_unique<ApplyLightsStage>(pipeline_);
    stage_mgr->add_stage(apply_lights_stage_.get());
    
    shadow_stage_ = std::make_unique<ShadowStage>(pipeline_);
    shadow_stage_->set_size(shadow_manager_->get_atlas_size());
    stage_mgr->add_stage(shadow_stage_.get());
}

void LightManager::init_defines()
{
    auto& defines = pipeline_.get_stage_mgr()->get_defines();
    defines["LC_TILE_SIZE_X"] = std::to_string(tile_size_.get_x());
    defines["LC_TILE_SIZE_Y"] = std::to_string(tile_size_.get_y());
    defines["LC_TILE_SLICES"] = std::to_string(pipeline_.get_setting<unsigned int>("lighting.culling_grid_slices"));
    defines["LC_MAX_DISTANCE"] = std::to_string(pipeline_.get_setting<float>("lighting.culling_max_distance"));
    defines["LC_CULLING_SLICE_WIDTH"] = std::to_string(pipeline_.get_setting<unsigned int>("lighting.culling_slice_width"));
    defines["LC_MAX_LIGHTS_PER_CELL"] = std::to_string(pipeline_.get_setting<unsigned int>("lighting.max_lights_per_cell"));
    defines["SHADOW_ATLAS_SIZE"] = std::to_string(pipeline_.get_setting<unsigned int>("shadows.atlas_size"));

    // Register all light types as defines
    static_assert(RPPointLight::LightType::LT_type_count == 3, "LightType count is not the same with defined value");
    defines["LT_EMPTY"] = std::to_string(RPPointLight::LightType::LT_empty);
    defines["LT_POINT_LIGHT"] = std::to_string(RPPointLight::LightType::LT_point_light);
    defines["LT_SPOT_LIGHT"] = std::to_string(RPPointLight::LightType::LT_spot_light);
}

}
