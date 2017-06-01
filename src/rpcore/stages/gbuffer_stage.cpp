#include <render_pipeline/rpcore/stages/gbuffer_stage.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/shader_input_blocks.h>
#include <render_pipeline/rppanda/showbase/showbase.h>

namespace rpcore {

GBufferStage::RequireType GBufferStage::required_inputs = { "DefaultEnvmap" };
GBufferStage::RequireType GBufferStage::required_pipes;

GBufferStage::ProduceType GBufferStage::get_produced_pipes(void) const
{
	return {
		make_gbuffer_ubo(),
		ShaderInput("SceneDepth", target->get_depth_tex()),
	};
}

std::shared_ptr<SimpleInputBlock> GBufferStage::make_gbuffer_ubo(void) const
{
	std::shared_ptr<SimpleInputBlock> ubo(new SimpleInputBlock("GBuffer"));
	ubo->add_input("Depth", target->get_depth_tex());
	ubo->add_input("Data0", target->get_color_tex());
	ubo->add_input("Data1", target->get_aux_tex(0));
	ubo->add_input("Data2", target->get_aux_tex(1));

	return ubo;
}

void GBufferStage::create(void)
{
	const bool stereo_mode = pipeline_.get_setting<bool>("pipeline.stereo_mode");

	target = create_target("GBuffer");
	target->add_color_attachment(16, true);
	target->add_depth_attachment(32);
	target->add_aux_attachments(16, 2);
	if (stereo_mode)
		target->set_layers(2);
	target->prepare_render(Globals::base->get_cam());
}

void GBufferStage::set_shader_input(const ShaderInput& inp)
{
	rpcore::Globals::render.set_shader_input(inp);
}

std::string GBufferStage::get_plugin_id(void) const
{
	return std::string("render_pipeline_internal");
}

}
