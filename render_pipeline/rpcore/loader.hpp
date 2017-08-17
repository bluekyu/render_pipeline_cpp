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
#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.hpp>

class TextFont;

namespace rpcore {

/**
 * Generic loader class used by the pipeline. All loading of assets happens
 * here, which enables us to keep track of used resources.
 */
class RENDER_PIPELINE_DECL RPLoader: public RPObject
{
public:
    /** Loads a 2D-texture from disk. */
    static Texture* load_texture(const Filename& filename);

    /** Loads a cube map from disk. */
    static Texture* load_cube_map(const Filename& filename, bool read_mipmaps=false);

    /** Loads a 3D-texture from disk. */
    static Texture* load_3d_texture(const Filename& filename);

    /** Loads a font from disk. */
    static TextFont* load_font(const Filename& filename);

    /** Loads a shader from disk. */
    static PT(Shader) load_shader(const std::vector<Filename>& path_args);

    /** Loads a model from disk. */
    static NodePath load_model(const Filename& filename);

    static Texture* load_sliced_3d_texture(const Filename& filename, int tile_size_x);
    static Texture* load_sliced_3d_texture(const Filename& filename, int tile_size_x, int tile_size_y);

    /**
     * Loads a texture from the given filename and dimensions. If only
     * one dimensions is specified, the other dimensions are assumed to be
     * equal. This internally loads the texture into ram, splits it into smaller
     * sub-images, and then calls the load_3d_texture from the Panda loader
     */
    static Texture* load_sliced_3d_texture(const Filename& filename, int tile_size_x, int tile_size_y, int num_tiles);

    RPLoader();
};

// ************************************************************************************************
inline RPLoader::RPLoader(): RPObject("RPLoader")
{
}

}
