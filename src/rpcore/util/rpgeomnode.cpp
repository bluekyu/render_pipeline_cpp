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
#include <geomVertexReader.h>
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

Texture* RPGeomNode::get_texture(int geom_index, TextureStage* stage) const
{
    if (!check_index_bound(geom_index))
        return nullptr;

    const RenderState* state = node_->get_geom_state(geom_index);

    if (!state->has_attrib(TextureAttrib::get_class_type()))
    {
        RPObject::global_warn("RPGeomNode", fmt::format("Geom {} has no texture!", node_->get_name()));
        return nullptr;
    }

    return DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()))->get_on_texture(stage);
}

void RPGeomNode::set_texture(int geom_index, Texture* texture, int priority)
{
    if (!check_index_bound(geom_index))
        return;

    const RenderState* state = node_->get_geom_state(geom_index);

    if (state->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()));
        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(tex_attrib->filter_to_max(1)->get_on_stage(0), texture, priority);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib, 
            state->get_override(TextureAttrib::get_class_type())));
    }
    else
    {
        CPT(RenderAttrib) new_attrib = TextureAttrib::make(texture);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib));
    }
}

void RPGeomNode::set_texture(int geom_index, TextureStage* stage, Texture* texture, int priority)
{
    if (!check_index_bound(geom_index))
        return;

    const RenderState* state = node_->get_geom_state(geom_index);

    if (state->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()));
        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(stage, texture, priority);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib,
            state->get_override(TextureAttrib::get_class_type())));
    }
    else
    {
        CPT(TextureAttrib) new_attrib = DCAST(TextureAttrib, TextureAttrib::make());
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib->add_on_stage(stage, texture, priority)));
    }
}

void RPGeomNode::set_basecolor_texture(int geom_index, Texture* texture, int priority)
{
    if (!check_index_bound(geom_index))
        return;

    const RenderState* state = node_->get_geom_state(geom_index);

    if (state->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()));
        auto stage = tex_attrib->get_on_stage(0);

        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(stage, texture, priority);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib,
            state->get_override(TextureAttrib::get_class_type())));
    }
    else
    {
        CPT(TextureAttrib) new_attrib = DCAST(TextureAttrib, TextureAttrib::make());
        PT(TextureStage) stage = new TextureStage("basecolor-0");
        stage->set_sort(0);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib->add_on_stage(stage, texture, priority)));
    }
}

void RPGeomNode::set_normal_texture(int geom_index, Texture* texture, int priority)
{
    if (!check_index_bound(geom_index))
        return;

    const RenderState* state = node_->get_geom_state(geom_index);

    if (state->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()));

        PT(TextureStage) stage;
        if (tex_attrib->get_num_on_stages() < 2)
        {
            stage = new TextureStage("normal-10");
            stage->set_sort(10);
        }
        else
        {
            stage = tex_attrib->get_on_stage(1);
        }

        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(stage, texture, priority);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib,
            state->get_override(TextureAttrib::get_class_type())));
    }
    else
    {
        RPObject::global_error("RPGeomNode", "The geom does NOT have any texture.");
    }
}

void RPGeomNode::set_specular_texture(int geom_index, Texture* texture, int priority)
{
    if (!check_index_bound(geom_index))
        return;

    const RenderState* state = node_->get_geom_state(geom_index);

    if (state->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()));

        PT(TextureStage) stage;
        const auto num_on_stages = tex_attrib->get_num_on_stages();
        if (num_on_stages > 2)
        {
            stage = tex_attrib->get_on_stage(2);
        }
        else if (num_on_stages > 1)
        {
            stage = new TextureStage("specular-20");
            stage->set_sort(20);
        }
        else
        {
            RPObject::global_error("RPGeomNode", "The geom does NOT have 'basecolor' or 'normal' texture.");
        }

        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(stage, texture, priority);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib,
            state->get_override(TextureAttrib::get_class_type())));
    }
    else
    {
        RPObject::global_error("RPGeomNode", "The geom does NOT have any texture.");
    }
}

void RPGeomNode::set_roughness_texture(int geom_index, Texture* texture, int priority)
{
    if (!check_index_bound(geom_index))
        return;

    const RenderState* state = node_->get_geom_state(geom_index);

    if (state->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state->get_attrib(TextureAttrib::get_class_type()));

        PT(TextureStage) stage;
        const auto num_on_stages = tex_attrib->get_num_on_stages();
        if (num_on_stages > 3)
        {
            stage = tex_attrib->get_on_stage(3);
        }
        else if (num_on_stages > 2)
        {
            stage = new TextureStage("roughness-30");
            stage->set_sort(30);
        }
        else
        {
            RPObject::global_error("RPGeomNode", "The geom does NOT have 'basecolor', 'normal' or 'specular' texture.");
        }

        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(stage, texture, priority);
        node_->set_geom_state(geom_index, state->set_attrib(new_attrib,
            state->get_override(TextureAttrib::get_class_type())));
    }
    else
    {
        RPObject::global_error("RPGeomNode", "The geom does NOT have any texture.");
    }
}

size_t RPGeomNode::get_vertex_data_size(int geom_index, size_t array_index) const
{
    if (!check_index_bound(geom_index))
        return 0;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data();

    if (!check_index_bound(vdata, array_index))
        return 0;

    return vdata->get_array(array_index)->get_data_size_bytes();
}

int RPGeomNode::get_vertex_count(int geom_index) const
{
    if (!check_index_bound(geom_index))
        return 0;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data();

    return vdata->get_num_rows();
}

bool RPGeomNode::get_vertex_data(std::vector<LVecBase3f>& vertices,
    std::vector<LVecBase3f>& normals,
    std::vector<LVecBase2f>& texcoords,
    int geom_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data();

    return get_vertex_data(vdata, vertices, normals, texcoords);
}

bool RPGeomNode::get_vertex_data(std::vector<LVecBase3f>& vertices,
    int geom_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data();

    return get_vertex_data(vdata, vertices);
}

bool RPGeomNode::get_vertex_data(std::vector<unsigned char>& data, int geom_index, size_t array_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data();

    return get_vertex_data(vdata, data, array_index);
}

bool RPGeomNode::get_animated_vertex_data(std::vector<LVecBase3f>& vertices,
    std::vector<LVecBase3f>& normals,
    std::vector<LVecBase2f>& texcoords,
    int geom_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data()->animate_vertices(true, Thread::get_current_thread());

    return get_vertex_data(vdata, vertices, normals, texcoords);
}

bool RPGeomNode::get_animated_vertex_data(std::vector<LVecBase3f>& vertices,
    int geom_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data()->animate_vertices(true, Thread::get_current_thread());

    return get_vertex_data(vdata, vertices);
}

bool RPGeomNode::get_animated_vertex_data(std::vector<unsigned char>& data, int geom_index, size_t array_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    CPT(GeomVertexData) vdata = node_->get_geom(geom_index)->get_vertex_data()->animate_vertices(true, Thread::get_current_thread());

    return get_vertex_data(vdata, data, array_index);
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

bool RPGeomNode::modify_vertex_data(const unsigned char* data,
    size_t data_size, size_t start_index, int geom_index, size_t array_index)
{
    if (!check_index_bound(geom_index))
        return false;

    PT(GeomVertexData) vdata = node_->modify_geom(geom_index)->modify_vertex_data();

    if (!check_index_bound(vdata, array_index))
        return false;

    // copy_subdata_from() will adjust the size correctly.
    // so, we do not need to check the bounds.
    vdata->modify_array_handle(array_index)->copy_subdata_from(start_index, data_size,
        data, 0, data_size);

    return true;
}

int RPGeomNode::get_index_count(int geom_index, size_t primitive_index) const
{
    if (!check_index_bound(geom_index))
        return 0;

    CPT(Geom) geom = node_->get_geom(geom_index);

    if (primitive_index >= geom->get_num_primitives())
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The primitive index is greater or equal than the number of primitive ({})", geom->get_num_primitives()));
        return false;
    }

    const auto& primitive = geom->get_primitive(primitive_index);
    return primitive->get_num_vertices();
}

bool RPGeomNode::get_index_data(std::vector<int>& indices, int geom_index, size_t primitive_index) const
{
    if (!check_index_bound(geom_index))
        return false;

    CPT(Geom) geom = node_->get_geom(geom_index);

    if (primitive_index >= geom->get_num_primitives())
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The primitive index is greater or equal than the number of primitive ({})", geom->get_num_primitives()));
        return false;
    }

    const auto& primitive = geom->get_primitive(primitive_index);
    const GeomPrimitivePipelineReader reader(primitive, Thread::get_current_thread());

    indices.clear();
    indices.reserve(reader.get_num_vertices());
    for (int k = 0, k_end = reader.get_num_vertices(); k < k_end; ++k)
        indices.push_back(reader.get_vertex(k));

    return true;
}

bool RPGeomNode::modify_index_data(const unsigned char* indices, size_t data_size, int geom_index, size_t primitive_index)
{
    if (!check_index_bound(geom_index))
        return false;

    if (primitive_index >= node_->get_geom(geom_index)->get_num_primitives())
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The primitive index is greater or equal than the number of primitive ({})",
                node_->get_geom(geom_index)->get_num_primitives()));
        return false;
    }

    PT(Geom) geom = node_->modify_geom(geom_index);

    auto primitive = geom->modify_primitive(primitive_index);

    size_t type_size = 0;
    switch (primitive->get_index_type())
    {
    case GeomEnums::NT_uint8:
        type_size = sizeof(uint8_t);
        break;
    case GeomEnums::NT_uint16:
        type_size = sizeof(uint16_t);
        break;
    case GeomEnums::NT_uint32:
        type_size = sizeof(uint32_t);
        break;
    default:
        return false;
    }

    if ((data_size % type_size) != 0)
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The size {} of indices is NOT multiple of the size ({}) of type", data_size,
                type_size));
        return false;
    }

    const size_t num_rows = data_size / type_size;
    if (num_rows > static_cast<size_t>((std::numeric_limits<int>::max)()))
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The number {} of indices is more than the maximum size ({}).", num_rows,
            (std::numeric_limits<int>::max)()));
        return false;
    }

    // see GeomPrimitive::add_vertex
    auto handle = primitive->modify_vertices()->modify_handle();
    handle->set_num_rows(static_cast<int>(num_rows));
    std::copy(indices, indices + data_size, handle->get_write_pointer());

    return true;
}

bool RPGeomNode::modify_index_data(const std::vector<int>& indices, int geom_index, size_t primitive_index)
{
    if (indices.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The number {} of indices is more than the maximum size ({}).", indices.size(),
            (std::numeric_limits<int>::max)()));
        return false;
    }

    if (!check_index_bound(geom_index))
        return false;

    if (primitive_index >= node_->get_geom(geom_index)->get_num_primitives())
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The primitive index is greater or equal than the number of primitive ({})",
                node_->get_geom(geom_index)->get_num_primitives()));
        return false;
    }

    PT(Geom) geom = node_->modify_geom(geom_index);

    auto primitive = geom->modify_primitive(primitive_index);

    // see GeomPrimitive::add_vertex
    auto handle = primitive->modify_vertices()->modify_handle();
    handle->set_num_rows(static_cast<int>(indices.size()));

    unsigned char* ptr = handle->get_write_pointer();
    switch (primitive->get_index_type())
    {
        case GeomEnums::NT_uint8:
        {
            for (size_t k = 0, k_end = indices.size(); k < k_end; ++k)
                reinterpret_cast<uint8_t*>(ptr)[k] = indices[k];
            break;
        }
        case GeomEnums::NT_uint16:
        {
            for (size_t k = 0, k_end = indices.size(); k < k_end; ++k)
                reinterpret_cast<uint16_t*>(ptr)[k] = indices[k];
            break;
        }
        case GeomEnums::NT_uint32:
        {
            static_assert(sizeof(int) == sizeof(uint32_t), "ERROR: type size is not same in std::copy");
            std::copy(indices.begin(), indices.end(), reinterpret_cast<uint32_t*>(ptr));
            break;
        }
        default:
        {
            return false;
        }
    }

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

bool RPGeomNode::check_index_bound(const GeomVertexData* vdata, size_t array_index) const
{
    if (array_index >= vdata->get_num_arrays())
    {
        RPObject::global_error("RPGeomNode",
            fmt::format("The array index is greater or equal than the number of array ({})", vdata->get_num_arrays()));
        return false;
    }
    else
    {
        return true;
    }
}

bool RPGeomNode::get_vertex_data(const GeomVertexData* vdata, std::vector<LVecBase3f>& vertices,
    std::vector<LVecBase3f>& normals,
    std::vector<LVecBase2f>& texcoords) const
{
    GeomVertexReader geom_vertices(vdata, InternalName::get_vertex());
    GeomVertexReader geom_normals(vdata, InternalName::get_normal());
    GeomVertexReader geom_texcoord0(vdata, InternalName::get_texcoord());

    if (geom_vertices.has_column())
    {
        vertices.clear();
        vertices.reserve(static_cast<size_t>(vdata->get_num_rows()));

        while (!geom_vertices.is_at_end())
            vertices.push_back(geom_vertices.get_data3f());
    }

    if (geom_normals.has_column())
    {
        normals.clear();
        normals.reserve(static_cast<size_t>(vdata->get_num_rows()));

        while (!geom_normals.is_at_end())
            normals.push_back(geom_normals.get_data3f());
    }

    if (geom_texcoord0.has_column())
    {
        texcoords.clear();
        texcoords.reserve(static_cast<size_t>(vdata->get_num_rows()));

        while (!geom_texcoord0.is_at_end())
            texcoords.push_back(geom_texcoord0.get_data2f());
    }

    return true;
}

bool RPGeomNode::get_vertex_data(const GeomVertexData* vdata, std::vector<LVecBase3f>& vertices) const
{
    GeomVertexReader geom_vertices(vdata, InternalName::get_vertex());

    if (geom_vertices.has_column())
    {
        vertices.clear();
        vertices.reserve(static_cast<size_t>(vdata->get_num_rows()));

        while (!geom_vertices.is_at_end())
            vertices.push_back(geom_vertices.get_data3f());
    }

    return true;
}

bool RPGeomNode::get_vertex_data(const GeomVertexData* vdata, std::vector<unsigned char>& data, size_t array_index) const
{
    if (!check_index_bound(vdata, array_index))
        return false;

    const size_t dest_total_bytes = vdata->get_array(array_index)->get_data_size_bytes();

    data.resize(dest_total_bytes);

    const unsigned char* read_pointer = vdata->get_array_handle(array_index)->get_read_pointer(true);
    std::copy(read_pointer, read_pointer + dest_total_bytes, data.data());

    return true;
}

}
