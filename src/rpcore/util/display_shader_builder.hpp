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
