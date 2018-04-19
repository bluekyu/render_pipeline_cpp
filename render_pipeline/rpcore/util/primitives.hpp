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

namespace rpcore {

RENDER_PIPELINE_DECL NodePath create_line(const std::string& name,
    const std::vector<LVecBase3>& vertices,
    float thickness = 1.0f,
    GeomEnums::UsageHint vertex_buffer_hint = Geom::UsageHint::UH_static);

/**
 * Create a mesh has a general geometry using triangle primitive without index.
 *
 * @return  NodePath of GeomNode. If failed, return empty NodePath.
 */
RENDER_PIPELINE_DECL NodePath create_triangle_mesh(const std::string& name,
    const std::vector<LVecBase3>& vertices,
    const std::vector<LVecBase3>& normals,
    const std::vector<LVecBase2>& texcoords,
    GeomEnums::UsageHint vertex_buffer_hint = Geom::UsageHint::UH_static,
    GeomEnums::UsageHint index_buffer_hint = Geom::UsageHint::UH_static);

/**
 * Create a mesh has a general geometry using triangle primitive.
 *
 * @return  NodePath of GeomNode. If failed, return empty NodePath.
 */
RENDER_PIPELINE_DECL NodePath create_triangle_mesh(const std::string& name,
    const std::vector<LVecBase3>& vertices,
    const std::vector<LVecBase3>& normals,
    const std::vector<LVecBase2>& texcoords,
    const std::vector<int>& indices,
    GeomEnums::UsageHint vertex_buffer_hint = Geom::UsageHint::UH_static,
    GeomEnums::UsageHint index_buffer_hint = Geom::UsageHint::UH_static);

/**
 * Create a mesh has a 2D plane geometry.
 *
 * left-bottom is (-0.5, -0.5, 0) and right-top is (0.5, 0.5, 0).
 */
RENDER_PIPELINE_DECL NodePath create_plane(const std::string& name);

/**
 * Create a mesh has a cube (box) geometry.
 *
 * min bound is (-0.5, -0.5, -0.5) and max bound is (0.5, 0.5, 0.5).
 */
RENDER_PIPELINE_DECL NodePath create_cube(const std::string& name);

/**
 * Create a mesh has a sphere geometry.
 *
 * Center is (0, 0, 0) and radius 1.0.
 */
RENDER_PIPELINE_DECL NodePath create_sphere(const std::string& name, unsigned int latitude, unsigned int longitude);

}
