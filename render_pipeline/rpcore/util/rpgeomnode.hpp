#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

class Texture;

namespace rpcore {

/**
 * Wrapper class of GeomNode.
 */
class RENDER_PIPELINE_DECL RPGeomNode
{
public:
    RPGeomNode(NodePath nodepath);

    NodePath get_nodepath(void) const;

    int get_num_geoms(void) const;

    bool has_material(int geom_index) const;
    RPMaterial get_material(int geom_index) const;
    void set_material(int geom_index, const RPMaterial& material);

    bool has_texture(int geom_index) const;
    Texture* get_texture(int geom_index) const;

    /** Set the texture on the first TextureStage or default stage if texture does not exist. */
    void set_texture(int geom_index, Texture* texture);

private:
    NodePath nodepath_;
};

// ************************************************************************************************

inline NodePath RPGeomNode::get_nodepath(void) const
{
    return nodepath_;
}

}
