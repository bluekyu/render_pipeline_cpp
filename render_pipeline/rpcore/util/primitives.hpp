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

/**
 * Create points primitive.
 */
RENDER_PIPELINE_DECL NodePath create_points(const std::string& name, const std::vector<LPoint3f>& positions,
    GeomEnums::UsageHint buffer_hint=Geom::UsageHint::UH_static);

RENDER_PIPELINE_DECL bool modify_points(GeomNode* geom_node, const std::vector<LPoint3f>& positions,
    int geom_index=0, int primitive_index=0);

/**
 * Create 2D plane primitive.
 *
 * left-bottom is (-0.5, 0, -0.5) and right-top is (0.5, 0, 0.5).
 */
RENDER_PIPELINE_DECL NodePath create_plane(const std::string& name);

/**
 * Create cube (box) primitive.
 *
 * min bound is (-0.5, -0.5, -0.5) and max bound is (0.5, 0.5, 0.5).
 */
RENDER_PIPELINE_DECL NodePath create_cube(const std::string& name);

/**
 * Create sphere primitiv.e
 *
 * Center is (0, 0, 0) and radius 1.0.
 */
RENDER_PIPELINE_DECL NodePath create_sphere(const std::string& name, unsigned int latitude, unsigned int longitude);

}
