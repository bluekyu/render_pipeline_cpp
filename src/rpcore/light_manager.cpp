#include <render_pipeline/rpcore/light_manager.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/stage_manager.hpp>
#include <render_pipeline/rpcore/image.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

#include <render_pipeline/rpcore/stages/apply_lights_stage.hpp>
#include <render_pipeline/rpcore/stages/shadow_stage.hpp>
#include <render_pipeline/rpcore/stages/flag_used_cells_stage.hpp>
#include <render_pipeline/rpcore/stages/collect_used_cells_stage.hpp>
#include <render_pipeline/rpcore/stages/cull_lights_stage.hpp>

#include <render_pipeline/rpcore/native/internal_light_manager.h>
#include <render_pipeline/rpcore/native/shadow_manager.h>
#include <render_pipeline/rpcore/native/rp_point_light.h>

#include "rpcore/gpu_command_queue.hpp"

namespace rpcore {

LightManager::LightManager(RenderPipeline* pipeline): RPObject("LightManager"), pipeline(pipeline)
{
    compute_tile_size();
    init_internal_manager();
    init_command_queue();
    initshadow_manager();
    init_stages();
}

LightManager::~LightManager(void)
{
    delete cmd_queue;
    delete shadow_manager;
    delete internal_mgr;
}

int LightManager::get_total_tiles(void) const
{
    return num_tiles.get_x() * num_tiles.get_y() * pipeline->get_setting<int>("lighting.culling_grid_slices");
}

size_t LightManager::get_num_lights(void) const
{
    return internal_mgr->get_num_lights();
}

size_t LightManager::get_num_shadow_sources(void) const
{
    return internal_mgr->get_num_shadow_sources();
}

float LightManager::get_shadow_atlas_coverage(void) const
{
    return internal_mgr->get_shadow_manager()->get_atlas()->get_coverage() * 100.0f;
}

void LightManager::add_light(RPLight* light)
{
    internal_mgr->add_light(light);
    pta_max_light_index[0] = internal_mgr->get_max_light_index();
}

void LightManager::remove_light(RPLight* light)
{
    internal_mgr->remove_light(light);
    pta_max_light_index[0] = internal_mgr->get_max_light_index();
}

void LightManager::update(void)
{
    internal_mgr->set_camera_pos(Globals::base->get_cam().get_pos(Globals::base->get_render()));
    internal_mgr->update();
    shadow_manager->update();
    cmd_queue->process_queue();
}

void LightManager::reload_shaders(void)
{
    cmd_queue->reload_shaders();
}

void LightManager::compute_tile_size(void)
{
    tile_size = LVecBase2i(
        pipeline->get_setting<int>("lighting.culling_grid_size_x"),
        pipeline->get_setting<int>("lighting.culling_grid_size_y"));

    int num_tiles_x = int(std::ceil(Globals::resolution.get_x() / float(tile_size.get_x())));
    int num_tiles_y = int(std::ceil(Globals::resolution.get_y() / float(tile_size.get_y())));
    debug(std::string("Tile size = ") + std::to_string(tile_size.get_x()) + "x" + std::to_string(tile_size.get_y()) +
        ", Num tiles = " + std::to_string(num_tiles_x) + "x" + std::to_string(num_tiles_y));
    num_tiles = LVecBase2i(num_tiles_x, num_tiles_y);
}

void LightManager::init_command_queue(void)
{
    cmd_queue = new GPUCommandQueue(pipeline);
    cmd_queue->register_input("LightData", img_light_data->get_texture());
    cmd_queue->register_input("SourceData", img_source_data->get_texture());
    internal_mgr->set_command_list(cmd_queue->get_command_list());
}

void LightManager::initshadow_manager(void)
{
    shadow_manager = new ShadowManager;
    shadow_manager->set_max_updates(pipeline->get_setting<size_t>("shadows.max_updates"));
    shadow_manager->set_scene(Globals::base->get_render());
    shadow_manager->set_tag_state_manager(pipeline->get_tag_mgr());
    shadow_manager->set_atlas_size(pipeline->get_setting<size_t>("shadows.atlas_size"));
    internal_mgr->set_shadow_manager(shadow_manager);
}

void LightManager::init_shadows(void)
{
    shadow_manager->set_atlas_graphics_output(shadow_stage->get_atlas_buffer());
    shadow_manager->init();
}

void LightManager::init_internal_manager(void)
{
    internal_mgr = new InternalLightManager;
    internal_mgr->set_shadow_update_distance(pipeline->get_setting<float>("shadows.max_update_distance"));
    
    // Storage for the Lights
    const int per_light_vec4s = 4;
    img_light_data = Image::create_buffer("LightData", MAX_LIGHTS * per_light_vec4s, "RGBA16");
    img_light_data->clear_image();
    
    pta_max_light_index = PTA_int::empty_array(1);
    pta_max_light_index[0] = 0;
    
    // Storage for the shadow sources
    const int per_source_vec4s = 5;
    
    // IMPORTANT: RGBA32 is really required here.Otherwise artifacts and bad
    // shadow filtering occur due to precision issues
    img_source_data = Image::create_buffer("ShadowSourceData", MAX_SOURCES * per_source_vec4s, "RGBA32");

    // TODO: check if this is right.
    img_source_data->clear_image();
    
    // Register the buffer
    pipeline->get_stage_mgr()->add_input(ShaderInput("AllLightsData", img_light_data->get_texture()));
    pipeline->get_stage_mgr()->add_input(ShaderInput("ShadowSourceData", img_source_data->get_texture()));
    pipeline->get_stage_mgr()->add_input(ShaderInput("maxLightIndex", pta_max_light_index));
}

void LightManager::init_stages(void)
{
    StageManager* stage_mgr = pipeline->get_stage_mgr();
    
    stage_mgr->add_stage(std::make_shared<FlagUsedCellsStage>(*pipeline));
    stage_mgr->add_stage(std::make_shared<CollectUsedCellsStage>(*pipeline));
    stage_mgr->add_stage(std::make_shared<CullLightsStage>(*pipeline));
    stage_mgr->add_stage(std::make_shared<ApplyLightsStage>(*pipeline));
    
    shadow_stage = std::make_shared<ShadowStage>(*pipeline);
    shadow_stage->set_size(shadow_manager->get_atlas_size());
    stage_mgr->add_stage(shadow_stage);
}

void LightManager::init_defines(void)
{
    auto& defines = pipeline->get_stage_mgr()->get_defines();
    defines["LC_TILE_SIZE_X"] = std::to_string(tile_size.get_x());
    defines["LC_TILE_SIZE_Y"] = std::to_string(tile_size.get_y());
    defines["LC_TILE_SLICES"] = std::to_string(pipeline->get_setting<unsigned int>("lighting.culling_grid_slices"));
    defines["LC_MAX_DISTANCE"] = std::to_string(pipeline->get_setting<float>("lighting.culling_max_distance"));
    defines["LC_CULLING_SLICE_WIDTH"] = std::to_string(pipeline->get_setting<unsigned int>("lighting.culling_slice_width"));
    defines["LC_MAX_LIGHTS_PER_CELL"] = std::to_string(pipeline->get_setting<unsigned int>("lighting.max_lights_per_cell"));
    defines["SHADOW_ATLAS_SIZE"] = std::to_string(pipeline->get_setting<unsigned int>("shadows.atlas_size"));

    // Register all light types as defines
    static_assert(RPPointLight::LightType::LT_type_count == 3, "LightType count is not the same with defined value");
    defines["LT_EMPTY"] = std::to_string(RPPointLight::LightType::LT_empty);
    defines["LT_POINT_LIGHT"] = std::to_string(RPPointLight::LightType::LT_point_light);
    defines["LT_SPOT_LIGHT"] = std::to_string(RPPointLight::LightType::LT_spot_light);
}

}    //    namespace rpcore
