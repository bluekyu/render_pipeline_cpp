#include "gpu_command_queue.h"

#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/loader.h>
#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/stage_manager.h>
#include <render_pipeline/rpcore/image.h>
#include <render_pipeline/rpcore/native/gpu_command.h>
#include <render_pipeline/rpcore/native/gpu_command_list.h>

namespace rpcore {

GPUCommandQueue::GPUCommandQueue(RenderPipeline* pipeline): RPObject("GPUCommandQueue"), _pipeline(pipeline)
{
	_command_list = new GPUCommandList;
	_pta_num_commands = PTA_int::empty_array(1);
	create_data_storage();
	create_command_target();
	register_defines();
}

GPUCommandQueue::~GPUCommandQueue(void)
{
	delete _command_list;
}

size_t GPUCommandQueue::get_num_queued_commands(void) const
{
	return _command_list->get_num_commands();
}

void GPUCommandQueue::process_queue(void)
{
	PTA_uchar pointer = _data_texture->get_texture()->modify_ram_image();
	size_t num_commands_exec = _command_list->write_commands_to(pointer, _commands_per_frame);
	_pta_num_commands[0] = num_commands_exec;
}

void GPUCommandQueue::reload_shaders(void)
{
	_command_target->set_shader(RPLoader::load_shader({
		std::string("/$$rp/shader/default_post_process.vert.glsl"),
		std::string("/$$rp/shader/process_command_queue.frag.glsl")}
	));
}

void GPUCommandQueue::register_input(CPT_InternalName key, Texture* val)
{
	_command_target->set_shader_input(ShaderInput(std::move(key), val));
}

void GPUCommandQueue::register_defines(void)
{
	static_assert(GPUCommand::CommandType::CMD_type_count == 5, "GPUCommand::CommandType count is not the same with defined value");

	auto& defines = _pipeline->get_stage_mgr()->get_defines();
	defines["CMD_invalid"] = std::to_string(GPUCommand::CommandType::CMD_invalid);
	defines["CMD_store_light"] = std::to_string(GPUCommand::CommandType::CMD_store_light);
	defines["CMD_remove_light"] = std::to_string(GPUCommand::CommandType::CMD_remove_light);
	defines["CMD_store_source"] = std::to_string(GPUCommand::CommandType::CMD_store_source);
	defines["CMD_remove_sources"] = std::to_string(GPUCommand::CommandType::CMD_remove_sources);
	defines["GPU_CMD_INT_AS_FLOAT"] = std::string(GPUCommand::get_uses_integer_packing() ? "1": "0");
}

void GPUCommandQueue::create_data_storage(void)
{
	int command_buffer_size = _commands_per_frame * 32;
	debug(std::string("Allocating command buffer of size ") + std::to_string(command_buffer_size));
	_data_texture = Image::create_buffer("CommandQueue", command_buffer_size, "R32");
}

void GPUCommandQueue::create_command_target(void)
{
	_command_target = new RenderTarget("ExecCommandTarget");
	_command_target->set_size(1);
	_command_target->prepare_buffer();
    _command_target->set_shader_input(ShaderInput("CommandQueue", _data_texture->get_texture()));
	_command_target->set_shader_input(ShaderInput("commandCount", _pta_num_commands));
}

}	// namespace rpcore
