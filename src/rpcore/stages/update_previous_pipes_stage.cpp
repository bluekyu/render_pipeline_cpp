#include <render_pipeline/rpcore/stages/update_previous_pipes_stage.hpp>

#include <virtualFileSystem.h>

#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpcore {

UpdatePreviousPipesStage::RequireType& UpdatePreviousPipesStage::get_required_inputs(void) const
{
	static RequireType required_inputs;
	return required_inputs;
}

UpdatePreviousPipesStage::RequireType& UpdatePreviousPipesStage::get_required_pipes(void) const
{
	static RequireType required_pipes;
	return required_pipes;
}

void UpdatePreviousPipesStage::create(void)
{
	debug("Creating previous pipes stage ..");
	_target = create_target("StorePreviousPipes");
	_target->prepare_buffer();

	// Set inputs
	for (size_t i=0, i_end=_transfers.size(); i < i_end; ++i)
	{
		const std::string string_i(std::to_string(i));
		_target->set_shader_input(ShaderInput(std::string("SrcTex") + string_i, _transfers[i].first));
		_target->set_shader_input(ShaderInput(std::string("DestTex") + string_i, _transfers[i].second));
	}
}

void UpdatePreviousPipesStage::set_dimensions(void)
{
	for (auto& from_to: _transfers)
	{
		from_to.second->set_x_size(Globals::resolution.get_x());
		from_to.second->set_y_size(Globals::resolution.get_y());

        if (from_to.second->get_texture_type() == Texture::TextureType::TT_2d_texture_array)
            from_to.second->set_z_size(from_to.first->get_z_size());
	}
}

void UpdatePreviousPipesStage::reload_shaders(void)
{
	std::vector<std::string> uniforms;
	std::vector<std::string> lines;

	// Collect all samplers and generate the required uniforms and copy code
	for (size_t i=0, i_end=_transfers.size(); i < i_end; ++i)
	{
		std::string index(std::to_string(i));
		uniforms.push_back(get_sampler_type(_transfers[i].first) + " " + "SrcTex" + index);
		uniforms.push_back(get_sampler_type(_transfers[i].second, true) + " " + "DestTex" + index);

		lines.push_back(std::string("\n  // Copying ") + _transfers[i].first->get_name() + " to " + _transfers[i].second->get_name());

        if (_transfers[i].first->get_texture_type() == Texture::TextureType::TT_2d_texture_array)
        {
            lines.push_back(std::string("for (int z = 0, z_end=textureSize(SrcTex") + index + ", 0).z; z < z_end; ++z) {");
            lines.push_back(get_sampler_lookup(_transfers[i].first, "data" + index, "SrcTex" + index, "ivec3(coord_2d_int, z)"));
            lines.push_back(get_store_code(_transfers[i].second, "DestTex" + index, "ivec3(coord_2d_int, z)", "data" + index));
            lines.push_back("}\n");
        }
        else
        {
            lines.push_back(get_sampler_lookup(_transfers[i].first, "data" + index, "SrcTex" + index, "coord_2d_int"));
            lines.push_back(get_store_code(_transfers[i].second, "DestTex" + index, "coord_2d_int", "data" + index));
        }

		lines.push_back("\n");
	}

	// Actually create the shader
	std::string fragment =
		"#version 430\n"
		"\n// Autogenerated, do not edit! Your changes will be lost.\n\n";

	for (const auto& uniform: uniforms)
		fragment += std::string("uniform ") + uniform + ";\n";

	fragment += std::string(
		"\nvoid main() {\n"
		"  const ivec2 coord_2d_int = ivec2(gl_FragCoord.xy);\n");

	for (const auto& line: lines)
		fragment += std::string("  ") + line + "\n";

	fragment += "}\n";

	// Write the shader
	std::string shader_dest("/$$rptemp/$$update_previous_pipes.frag.glsl");
	VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
	try
	{
		std::ostream* file = vfs->open_write_file(shader_dest, false, false);
		*file << fragment;
		vfs->close_write_file(file);
	}
	catch (const std::exception& err)
	{
		error(std::string("Error writing shader autoconfig: ") + err.what());
	}

	// Load it back again
	_target->set_shader(load_shader({shader_dest}));
}

std::string UpdatePreviousPipesStage::get_sampler_type(Texture* tex, bool can_write)
{
	if (can_write)
	{
        if (tex->get_texture_type() == Texture::TextureType::TT_2d_texture_array)
            return "writeonly image2DArray";
        else
		    return "writeonly image2D";
	}
	else
	{
		if (tex->get_texture_type() == Texture::TextureType::TT_2d_texture_array)
			return "sampler2DArray";
		else
			return "sampler2D";
	}
}

std::string UpdatePreviousPipesStage::get_sampler_lookup(Texture* tex, const std::string& dest_name,
	const std::string& sampler_name, const std::string& coord_var)
{
	return std::string("vec4 ") + dest_name + " = texelFetch(" + sampler_name + ", " + coord_var + ", 0);";
}

std::string UpdatePreviousPipesStage::get_store_code(Texture* tex, const std::string& sampler_name,
	const std::string& coord_var, const std::string& data_var)
{
	return std::string("imageStore(") + sampler_name + ", " + coord_var + ", vec4(" + data_var + "));";
}

std::string UpdatePreviousPipesStage::get_plugin_id(void) const
{
	return std::string("render_pipeline_internal");
}

}
