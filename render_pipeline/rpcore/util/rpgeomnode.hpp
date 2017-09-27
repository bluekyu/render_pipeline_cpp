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
 * Decorator of GeomNode.
 */
class RENDER_PIPELINE_DECL RPGeomNode
{
public:
    RPGeomNode(const NodePath& nodepath);
    RPGeomNode(GeomNode* geomnode);

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

private:
    GeomNode* node_ = nullptr;
};

// ************************************************************************************************

inline RPGeomNode::RPGeomNode(GeomNode* geomnode): node_(geomnode)
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
