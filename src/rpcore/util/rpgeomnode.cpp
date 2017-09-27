/**
 * Render Pipeline C++
 *
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

#include "render_pipeline/rpcore/util/rpgeomnode.hpp"

#include <nodePath.h>
#include <materialAttrib.h>
#include <textureAttrib.h>

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rpcore/rpobject.hpp"

namespace rpcore {

RPGeomNode::RPGeomNode(const NodePath& nodepath)
{
    if (!nodepath.node()->is_geom_node())
    {
        RPObject::global_error("RPGeomNode", fmt::format("NodePath ({}) is NOT GeomNode.", nodepath));
        return;
    }

    node_ = DCAST(GeomNode, nodepath.node());
}

bool RPGeomNode::has_material(int geom_index) const
{
    return node_->get_geom_state(geom_index)->has_attrib(MaterialAttrib::get_class_type());
}

RPMaterial RPGeomNode::get_material(int geom_index) const
{
    const RenderState* state = node_->get_geom_state(geom_index);

    if (!state->has_attrib(MaterialAttrib::get_class_type()))
        return RPMaterial(nullptr);

    return RPMaterial(DCAST(MaterialAttrib, state->get_attrib(MaterialAttrib::get_class_type()))->get_material());
}

void RPGeomNode::set_material(int geom_index, const RPMaterial& material)
{
    const RenderState* state = node_->get_geom_state(geom_index);
    node_->set_geom_state(geom_index, state->set_attrib(MaterialAttrib::make(material.get_material())));
}

bool RPGeomNode::has_texture(int geom_index) const
{
    return node_->get_geom_state(geom_index)->has_attrib(TextureAttrib::get_class_type());
}

Texture* RPGeomNode::get_texture(int geom_index) const
{
    const RenderState* state = node_->get_geom_state(geom_index);

    if (!state->has_attrib(TextureAttrib::get_class_type()))
    {
        RPObject::global_warn("RPGeomNode", fmt::format("Geom {} has no texture!", node_->get_name()));
        return nullptr;
    }

    return DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()))->get_texture();
}

void RPGeomNode::set_texture(int geom_index, Texture* texture)
{
    const RenderState* state = node_->get_geom_state(geom_index);

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
    node_->set_geom_state(geom_index, state->set_attrib(new_attrib));
}

}
