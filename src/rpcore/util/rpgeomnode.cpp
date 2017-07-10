#include "render_pipeline/rpcore/util/rpgeomnode.hpp"

#include <geomNode.h>
#include <materialAttrib.h>
#include <textureAttrib.h>

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rpcore/rpobject.hpp"

namespace rpcore {

RPGeomNode::RPGeomNode(NodePath nodepath)
{
    if (!nodepath.node()->is_geom_node())
    {
        RPObject::global_error("RPGeomNode", fmt::format("NodePath ({}) is NOT GeomNode.", nodepath));
        return;
    }

    nodepath_ = nodepath;
}

int RPGeomNode::get_num_geoms(void) const
{
    return DCAST(GeomNode, nodepath_.node())->get_num_geoms();
}

bool RPGeomNode::has_material(int geom_index) const
{
    return DCAST(GeomNode, nodepath_.node())->get_geom_state(geom_index)->has_attrib(MaterialAttrib::get_class_type());
}

RPMaterial RPGeomNode::get_material(int geom_index) const
{
    GeomNode* geom_node = DCAST(GeomNode, nodepath_.node());
    const RenderState* state = geom_node->get_geom_state(geom_index);

    if (!state->has_attrib(MaterialAttrib::get_class_type()))
        return RPMaterial(nullptr);

    return RPMaterial(DCAST(MaterialAttrib, state->get_attrib(MaterialAttrib::get_class_type()))->get_material());
}

void RPGeomNode::set_material(int geom_index, const RPMaterial& material)
{
    GeomNode* geom_node = DCAST(GeomNode, nodepath_.node());
    const RenderState* state = geom_node->get_geom_state(geom_index);
    geom_node->set_geom_state(geom_index, state->set_attrib(MaterialAttrib::make(material.get_material())));
}

bool RPGeomNode::has_texture(int geom_index) const
{
    return DCAST(GeomNode, nodepath_.node())->get_geom_state(geom_index)->has_attrib(TextureAttrib::get_class_type());
}

Texture* RPGeomNode::get_texture(int geom_index) const
{
    GeomNode* geom_node = DCAST(GeomNode, nodepath_.node());
    const RenderState* state = geom_node->get_geom_state(geom_index);

    if (!state->has_attrib(TextureAttrib::get_class_type()))
    {
        RPObject::global_warn("RPGeomNode", fmt::format("Geom {} has no texture!", geom_node->get_name()));
        return nullptr;
    }

    return DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()))->get_texture();
}

void RPGeomNode::set_texture(int geom_index, Texture* texture)
{
    GeomNode* geom_node = DCAST(GeomNode, nodepath_.node());
    const RenderState* state = geom_node->get_geom_state(geom_index);

    CPT(RenderAttrib) new_attrib;
    if (state->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()));

        new_attrib = tex_attrib->add_on_stage(tex_attrib->filter_to_max(1)->get_on_stage(0), texture);
    }
    else
    {
        new_attrib = TextureAttrib::make(texture);
    }
    geom_node->set_geom_state(geom_index, state->set_attrib(new_attrib));
}

}
