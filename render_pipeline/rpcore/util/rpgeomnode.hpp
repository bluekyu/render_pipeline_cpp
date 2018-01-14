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

#pragma once

#include <geomNode.h>

#include <render_pipeline/rpcore/config.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

class Texture;
class NodePath;

namespace rpcore {

/**
 * Adapater of GeomNode.
 */
class RENDER_PIPELINE_DECL RPGeomNode
{
public:
    RPGeomNode(const NodePath& nodepath);
    RPGeomNode(PT(GeomNode) geomnode);

    GeomNode& operator*() const;
    GeomNode* operator->() const;
    explicit operator bool() const;
    bool operator!() const;

    GeomNode* get_node() const;

    int get_num_geoms() const;

    bool has_material(int geom_index) const;
    RPMaterial get_material(int geom_index) const;
    void set_material(int geom_index, const RPMaterial& material);

    bool has_texture(int geom_index) const;
    Texture* get_texture(int geom_index) const;

    /** Set the texture on the first TextureStage or default stage if texture does not exist. */
    void set_texture(int geom_index, Texture* texture);

    /** Functions for vertex data. */
    ///@{

    size_t get_vertex_data_size(size_t array_index = 0, int geom_index = 0) const;

    bool get_vertex_data(std::vector<LVecBase3f>& vertices,
        std::vector<LVecBase3f>& normals,
        std::vector<LVecBase2f>& texcoords,
        int geom_index = 0) const;

    bool get_vertex_data(std::vector<LVecBase3f>& vertices,
        int geom_index = 0) const;

    /**
     * Read vertex data in the array with @p array_index.
     */
    bool get_vertex_data(std::vector<unsigned char>& data, size_t array_index = 0, int geom_index = 0) const;

    bool get_animated_vertex_data(std::vector<LVecBase3f>& vertices,
        std::vector<LVecBase3f>& normals,
        std::vector<LVecBase2f>& texcoords,
        int geom_index = 0) const;

    bool get_animated_vertex_data(std::vector<LVecBase3f>& vertices,
        int geom_index = 0) const;

    /**
     * Read animated vertex data in the array with @p array_index.
     */
    bool get_animated_vertex_data(std::vector<unsigned char>& data, size_t array_index = 0, int geom_index = 0) const;

    /**
     * Modify vertices, normal, texcoords in vertex data.
     *
     * The size among @p vertices, @p normals and @p texcoords should be same.
     * And the size should be same as the original size.
     */
    bool modify_vertex_data(const std::vector<LVecBase3f>& vertices,
        const std::vector<LVecBase3f>& normals,
        const std::vector<LVecBase2f>& texcoords,
        int geom_index = 0);

    bool modify_vertex_data(const std::vector<LVecBase3f>& vertices,
        int geom_index = 0);

    /**
     * Modify (sub) vertex data.
     *
     * This just copy the @p data to vertex data in geom, so the format of @p data
     * should be equal to the format of vertex data.
     *
     * @param[in]   data        The pointer of data.
     * @param[in]   data_size   The size of data in bytes.
     * @param[in]   start_index The starting index of the vertex data in geom.
     */
    bool modify_vertex_data(const void* data, size_t data_size, size_t start_index = 0,
        size_t array_index = 0, int geom_index = 0);

    bool get_index_data(std::vector<int>& indices, size_t primitive_index = 0, int geom_index = 0) const;

    ///@}

protected:
    bool check_index_bound(int geom_index) const;
    bool check_index_bound(const GeomVertexData* vdata, size_t array_index) const;

    bool get_vertex_data(const GeomVertexData* vdata, std::vector<LVecBase3f>& vertices,
        std::vector<LVecBase3f>& normals,
        std::vector<LVecBase2f>& texcoords) const;
    bool get_vertex_data(const GeomVertexData* vdata, std::vector<LVecBase3f>& vertices) const;
    bool get_vertex_data(const GeomVertexData* vdata, std::vector<unsigned char>& data,
        size_t array_index) const;

    PT(GeomNode) node_;
};

// ************************************************************************************************

inline RPGeomNode::RPGeomNode(PT(GeomNode) geomnode): node_(geomnode)
{
}

inline GeomNode& RPGeomNode::operator*() const
{
    return *node_;
}

inline GeomNode* RPGeomNode::operator->() const
{
    return node_;
}

inline RPGeomNode::operator bool() const
{
    return node_ != nullptr;
}

inline bool RPGeomNode::operator!() const
{
    return node_ == nullptr;
}

inline GeomNode* RPGeomNode::get_node() const
{
    return node_;
}

inline int RPGeomNode::get_num_geoms() const
{
    return node_->get_num_geoms();
}

// ************************************************************************************************

inline bool operator==(const RPGeomNode& a, const RPGeomNode& b)
{
    return a.get_node() == b.get_node();
}

inline bool operator!=(const RPGeomNode& a, const RPGeomNode& b)
{
    return a.get_node() != b.get_node();
}

inline bool operator==(const RPGeomNode& m, std::nullptr_t)
{
    return m.get_node() == nullptr;
}

inline bool operator==(std::nullptr_t, const RPGeomNode& m)
{
    return m.get_node() == nullptr;
}

inline bool operator!=(const RPGeomNode& m, std::nullptr_t)
{
    return m.get_node() != nullptr;
}

inline bool operator!=(std::nullptr_t, const RPGeomNode& m)
{
    return m.get_node() != nullptr;
}

}
