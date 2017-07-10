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

    RPLoader(void);
};

// ************************************************************************************************
inline RPLoader::RPLoader(void): RPObject("RPLoader")
{
}

}
