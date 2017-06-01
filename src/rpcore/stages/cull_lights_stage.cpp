#include <render_pipeline/rpcore/stages/cull_lights_stage.h>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/light_manager.hpp>

namespace rpcore {

CullLightsStage::RequireType CullLightsStage::required_inputs = { "AllLightsData", "maxLightIndex" };
CullLightsStage::RequireType CullLightsStage::required_pipes = { "CellListBuffer" };

CullLightsStage::CullLightsStage(RenderPipeline& pipeline): RenderStage(pipeline, "CullLightsStage")
{
	_max_lights_per_cell = pipeline_.get_setting<int>("lighting.max_lights_per_cell");

	if (_max_lights_per_cell > (1<<16))
		fatal(std::string("lighting.max_lights_per_cell must be <=") + std::to_string(1<<16) + "!");

	_slice_width = pipeline_.get_setting<int>("lighting.culling_slice_width");

	// Amount of light classes.Has to match the ones in LightClassification.inc.glsl
	_num_light_classes = 4;
}

CullLightsStage::ProduceType CullLightsStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("PerCellLights", _grouped_cell_lights->get_texture()),
        ShaderInput("PerCellLightsCounts", _grouped_cell_lights_counts->get_texture()),
    };
}

CullLightsStage::DefinesType CullLightsStage::get_produced_defines(void) const
{
	return {
		{ "LC_SLICE_WIDTH", std::to_string(_slice_width) },
		{ "LC_CULL_THREADS", std::to_string(_cull_threads) },
		{ "LC_LIGHT_CLASS_COUNT", std::to_string(_num_light_classes) },
	};
}

void CullLightsStage::create(void)
{
	_target_visible = create_target("GetVisibleLights");
	_target_visible->set_size(16);
	_target_visible->prepare_buffer();

	_target_cull = create_target("CullLights");
	_target_cull->set_size(0);
	_target_cull->prepare_buffer();

	_target_group = create_target("GroupLightsByClass");
	_target_group->set_size(0);
	_target_group->prepare_buffer();

	_frustum_lights_ctr = Image::create_counter("VisibleLightCount");
    _frustum_lights = Image::create_buffer("FrustumLights", pipeline_.get_light_mgr()->MAX_LIGHTS, "R16UI");
	_per_cell_lights = Image::create_buffer("PerCellLights", 0, "R16UI");
	// Needs to be R32 for atomic add in cull stage
	_per_cell_light_counts = Image::create_buffer("PerCellLightCounts", 0, "R32I");
	_grouped_cell_lights = Image::create_buffer("GroupedPerCellLights", 0, "R16UI");
	_grouped_cell_lights_counts = Image::create_buffer("GroupedPerCellLightsCount", 0, "R16UI");

    _target_visible->set_shader_input(ShaderInput("FrustumLights", _frustum_lights->get_texture()));
    _target_visible->set_shader_input(ShaderInput("FrustumLightsCount", _frustum_lights_ctr->get_texture()));
    _target_cull->set_shader_input(ShaderInput("PerCellLightsBuffer", _per_cell_lights->get_texture()));
    _target_cull->set_shader_input(ShaderInput("PerCellLightCountsBuffer", _per_cell_light_counts->get_texture()));
    _target_cull->set_shader_input(ShaderInput("FrustumLights", _frustum_lights->get_texture()));
    _target_cull->set_shader_input(ShaderInput("FrustumLightsCount", _frustum_lights_ctr->get_texture()));
    _target_group->set_shader_input(ShaderInput("PerCellLightsBuffer", _per_cell_lights->get_texture()));
    _target_group->set_shader_input(ShaderInput("PerCellLightCountsBuffer", _per_cell_light_counts->get_texture()));
    _target_group->set_shader_input(ShaderInput("GroupedCellLightsBuffer", _grouped_cell_lights->get_texture()));
    _target_group->set_shader_input(ShaderInput("GroupedPerCellLightsCountBuffer", _grouped_cell_lights_counts->get_texture()));

	_target_cull->set_shader_input(ShaderInput("threadCount", LVecBase4i(_cull_threads, 0, 0, 0)));
	_target_group->set_shader_input(ShaderInput("threadCount", LVecBase4i(1, 0, 0, 0)));
}

void CullLightsStage::reload_shaders(void)
{
	_target_cull->set_shader(load_shader({"tiled_culling.vert.glsl", "cull_lights.frag.glsl"}));
	_target_group->set_shader(load_shader({"tiled_culling.vert.glsl", "group_lights.frag.glsl"}));
	_target_visible->set_shader(load_shader({"view_frustum_cull.frag.glsl"}));
}

void CullLightsStage::update(void)
{
	_frustum_lights_ctr->clear_image();
}

void CullLightsStage::set_dimensions(void)
{
    int max_cells = pipeline_.get_light_mgr()->get_total_tiles();
	int num_rows_threaded = int(std::ceil((max_cells * _cull_threads) / float(_slice_width)));
	int num_rows = int(std::ceil(max_cells / float(_slice_width)));
	_per_cell_lights->set_x_size(max_cells * _max_lights_per_cell);
	_per_cell_light_counts->set_x_size(max_cells);
	_grouped_cell_lights->set_x_size(max_cells * (_max_lights_per_cell + _num_light_classes));
	_target_cull->set_size(_slice_width, num_rows_threaded);
	_target_group->set_size(_slice_width, num_rows);
	_grouped_cell_lights_counts->set_x_size(max_cells * (1 + _num_light_classes));
}

std::string CullLightsStage::get_plugin_id(void) const
{
	return std::string("render_pipeline_internal");
}

}
