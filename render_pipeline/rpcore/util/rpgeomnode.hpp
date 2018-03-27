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

#include <render_pipeline/rpcore/util/rprender_state.hpp>

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

    RPRenderState get_state(int geom_index) const;
    void set_state(int geom_index, const RPRenderState& state);
    void set_state(int geom_index, const RenderState* state);

    bool has_material(int geom_index) const;
    RPMaterial get_material(int geom_index) const;
    void set_material(int geom_index, const RPMaterial& material);

    bool has_texture(int geom_index) const;
    Texture* get_texture(int geom_index) const;
    Texture* get_texture(int geom_index, TextureStage* stage) const;

    /**
     * Get the texture on specific type.
     * @return  Texture pointer or nullptr if empty or failed.
     */
    Texture* get_basecolor_texture(int geom_index) const;
    Texture* get_normal_texture(int geom_index) const;
    Texture* get_specular_texture(int geom_index) const;
    Texture* get_roughness_texture(int geom_index) const;

    /** Set the texture on the first TextureStage or default stage if texture does not exist. */
    void set_texture(int geom_index, Texture* texture, int priority = 0);

    /** Set the texture on the given TextureStage. */
    void set_texture(int geom_index, TextureStage* stage, Texture* texture, int priority = 0);

    /** Set the texture on specific type. */
    void set_basecolor_texture(int geom_index, Texture* texture, int priority = 0);
    void set_normal_texture(int geom_index, Texture* texture, int priority = 0);
    void set_specular_texture(int geom_index, Texture* texture, int priority = 0);
    void set_roughness_texture(int geom_index, Texture* texture, int priority = 0);

    /** Functions for vertex data. */
    ///@{

    /** Get total size of vertex data in bytes. */
    size_t get_vertex_data_size(int geom_index, size_t array_index) const;

    /** Get the number of vertices. */
    int get_vertex_count(int geom_index) const;

    bool get_vertex_data(std::vector<LVecBase3f>& vertices,
        std::vector<LVecBase3f>& normals,
        std::vector<LVecBase2f>& texcoords,
        int geom_index) const;

    bool get_vertex_data(std::vector<LVecBase3f>& vertices,
        int geom_index) const;

    /**
     * Read vertex data in the array with @p array_index.
     */
    bool get_vertex_data(std::vector<unsigned char>& data, int geom_index, size_t array_index) const;

    bool get_animated_vertex_data(std::vector<LVecBase3f>& vertices,
        std::vector<LVecBase3f>& normals,
        std::vector<LVecBase2f>& texcoords,
        int geom_index) const;

    bool get_animated_vertex_data(std::vector<LVecBase3f>& vertices,
        int geom_index) const;

    /**
     * Read animated vertex data in the array with @p array_index.
     */
    bool get_animated_vertex_data(std::vector<unsigned char>& data, int geom_index, size_t array_index) const;

    /**
     * Modify vertices, normal, texcoords in vertex data.
     *
     * The size among @p vertices, @p normals and @p texcoords should be same.
     * And the size should be same as the original size.
     */
    bool modify_vertex_data(const std::vector<LVecBase3f>& vertices,
        const std::vector<LVecBase3f>& normals,
        const std::vector<LVecBase2f>& texcoords,
        int geom_index);

    bool modify_vertex_data(const std::vector<LVecBase3f>& vertices,
        int geom_index);

    /**
     * Modify (sub) vertex data.
     *
     * This just copy the @p data to vertex data in geom, so the format of @p data
     * should be equal to the format of vertex data.
     *
     * For example, if the format consists of position, normal and texture coordinates,
     *
     * ```
     * struct Vertex { LVecBase3f pos; LVecBase3f norm; LVecBase2f tex; };
     * std::vector<Vertex> vertices;
     *
     * modify_vertex_data(reinterpret_cast<const unsigned char*>(vertices.data()),
     *     vertices.size() * sizeof(Vertex), 0, 0, 0);
     * ```
     *
     * @param[in]   data        The pointer of data.
     * @param[in]   data_size   The size of data in bytes.
     * @param[in]   start_index The starting index of the vertex data in geom.
     */
    bool modify_vertex_data(const unsigned char* data, size_t data_size, size_t start_index,
        int geom_index, size_t array_index);

    /** Get the number of indices. */
    int get_index_count(int geom_index, size_t primitive_index) const;

    bool get_index_data(std::vector<int>& indices, int geom_index, size_t primitive_index) const;

    bool modify_index_data(const unsigned char* indices, size_t data_size, int geom_index, size_t primitive_index);

    bool modify_index_data(const std::vector<int>& indices, int geom_index, size_t primitive_index);

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

inline RPRenderState RPGeomNode::get_state(int geom_index) const
{
    return RPRenderState(node_->get_geom_state(geom_index));
}

inline void RPGeomNode::set_state(int geom_index, const RPRenderState& state)
{
    node_->set_geom_state(geom_index, state.get_state());
}

inline void RPGeomNode::set_state(int geom_index, const RenderState* state)
{
    node_->set_geom_state(geom_index, state);
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
