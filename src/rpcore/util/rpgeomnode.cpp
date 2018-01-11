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
#include <geomVertexWriter.h>

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
    if (!check_index_bound(geom_index))
        return false;

    return node_->get_geom_state(geom_index)->has_attrib(MaterialAttrib::get_class_type());
}

RPMaterial RPGeomNode::get_material(int geom_index) const
{
    if (!check_index_bound(geom_index))
        return RPMaterial(nullptr);

    const RenderState* state = node_->get_geom_state(geom_index);

    if (!state->has_attrib(MaterialAttrib::get_class_type()))
        return RPMaterial(nullptr);

    return RPMaterial(DCAST(MaterialAttrib, state->get_attrib(MaterialAttrib::get_class_type()))->get_material());
}

void RPGeomNode::set_material(int geom_index, const RPMaterial& material)
{
    if (!check_index_bound(geom_index))
        return;

    const RenderState* state = node_->get_geom_state(geom_index);
    node_->set_geom_state(geom_index, state->set_attrib(MaterialAttrib::make(material.get_material())));
}

bool RPGeomNode::has_texture(int geom_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    return node_->get_geom_state(geom_index)->has_attrib(TextureAttrib::get_class_type());
}

Texture* RPGeomNode::get_texture(int geom_index) const
{
    if (!check_index_bound(geom_index))
        return nullptr;

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
    if (!check_index_bound(geom_index))
        return;

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

bool RPGeomNode::modify_vertex_data(const std::vector<LVecBase3>& vertices,
    const std::vector<LVecBase3>& normals, const std::vector<LVecBase2>& texcoords,
    int geom_index)
{
    if (!check_index_bound(geom_index))
        return false;

    if (!(vertices.size() == normals.size() && normals.size() == texcoords.size()))
    {
        RPObject::global_error("RPGeomNode", "The counts of vertices, normal, texcoords is NOT same.");
        return false;
    }

    PT(GeomVertexData) vdata = node_->modify_geom(geom_index)->modify_vertex_data();

    if (static_cast<size_t>(vdata->get_num_rows()) != vertices.size())
    {
        RPObject::global_error("RPGeomNode", 
            fmt::format("The size ({}) of vertices is not same as the original size ({}) of vertices in Geom.",
                vertices.size(), static_cast<size_t>(vdata->get_num_rows())));
        return false;
    }

    GeomVertexWriter geom_vertices(vdata, InternalName::get_vertex());
    GeomVertexWriter geom_normals(vdata, InternalName::get_normal());
    GeomVertexWriter geom_texcoord0(vdata, InternalName::get_texcoord());

    for (size_t k = 0, k_end = vertices.size(); k < k_end; ++k)
    {
        const auto& v = vertices[k];
        const auto& n = normals[k];
        const auto& t = texcoords[k];

        geom_vertices.set_data3(v[0], v[1], v[2]);
        geom_normals.set_data3(n[0], n[1], n[2]);
        geom_texcoord0.set_data2(t[0], t[1]);
    }

    return true;
}

bool RPGeomNode::modify_vertex_data(const std::vector<LVecBase3>& vertices,
    int geom_index)
{
    if (!check_index_bound(geom_index))
        return false;

    PT(GeomVertexData) vdata = node_->modify_geom(geom_index)->modify_vertex_data();

    if (static_cast<size_t>(vdata->get_num_rows()) != vertices.size())
    {
        RPObject::global_error("RPGeomNode", 
            fmt::format("The size ({}) of vertices is not same as the original size ({}) of vertices in Geom.",
                vertices.size(), static_cast<size_t>(vdata->get_num_rows())));
        return false;
    }

    GeomVertexWriter geom_vertices(vdata, InternalName::get_vertex());

    for (size_t k = 0, k_end = vertices.size(); k < k_end; ++k)
    {
        const auto& v = vertices[k];
        geom_vertices.set_data3(v[0], v[1], v[2]);
    }

    return true;
}

bool RPGeomNode::modify_vertex_data(const unsigned char* v3n3t2_data,
    size_t data_size, int geom_index)
{
    if (!check_index_bound(geom_index))
        return false;

    PT(GeomVertexData) vdata = node_->modify_geom(geom_index)->modify_vertex_data();

    const size_t num_rows = data_size / (sizeof(LVecBase3f) + sizeof(LVecBase3f) + sizeof(LVecBase2f));
    if (static_cast<size_t>(vdata->get_num_rows()) != num_rows)
    {
        RPObject::global_error("RPGeomNode", 
            fmt::format("The size ({}) of vertices is not same as the original size ({}) of vertices in Geom.",
                num_rows, static_cast<size_t>(vdata->get_num_rows())));
        return false;
    }

    if (vdata->get_num_arrays() != 1)
    {
        RPObject::global_error("RPGeomNode", "The number of vertex array is not one.");
        return false;
    }

    const size_t dest_total_bytes = vdata->get_array(0)->get_data_size_bytes();
    if (dest_total_bytes != data_size)
    {
        RPObject::global_error("RPGeomNode", 
            fmt::format("Total size ({}) is not same as the size ({}) of vertices in Geom.", data_size, dest_total_bytes));
        return false;
    }

    vdata->modify_array_handle(0)->copy_data_from(v3n3t2_data, data_size);

    return true;
}

bool RPGeomNode::check_index_bound(int geom_index) const
{
    if (geom_index < 0 || geom_index >= node_->get_num_geoms())
    {
        RPObject::global_error("RPGeomNode", "The geom index is out of bound.");
        return false;
    }
    else
    {
        return true;
    }
}

}
