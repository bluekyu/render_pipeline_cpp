#pragma once

#include <shader.h>

class Texture;

namespace rpcore {

/**
 * Utility class to generate shaders on the fly to display texture previews
 * and also buffers.
 */
class DisplayShaderBuilder
{
public:
	/**
	 * Builds a shader to display <texture> in a view port with the size
	 * <view_width> * <view_height>.
	 */
	static PT(Shader) build(Texture* texture, int view_width, int view_height);

private:
	/** Internal method to build a fragment shader displaying the texture. */
	static std::string build_fragment_shader(Texture* texture, int view_width, int view_height);

	/** Generates the GLSL code to sample a texture and also returns the GLSL sampler type. */
	static std::pair<std::string, std::string> generate_sampling_code(Texture* texture, int view_width, int view_height);
};

}
