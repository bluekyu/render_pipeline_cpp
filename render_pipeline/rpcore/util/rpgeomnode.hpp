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
