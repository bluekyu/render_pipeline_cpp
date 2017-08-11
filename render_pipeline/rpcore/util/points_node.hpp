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

class RENDER_PIPELINE_DECL PointsNode
{
public:
    PointsNode(const std::string& name, const std::vector<LPoint3f>& positions, float radius=1.0f, 
        GeomEnums::UsageHint buffer_hint=GeomEnums::UH_static);

    PointsNode(const PointsNode&) = delete;
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    PointsNode(PointsNode&&);
#endif

    ~PointsNode(void);

    PointsNode& operator=(const PointsNode&) = delete;
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    PointsNode& operator=(PointsNode&& other);
#endif

    NodePath get_nodepath(void) const;

    /** Set circular effect. */
    void set_circular_point(void) const;

    /** Get position. */
    const LPoint3f& get_position(int point_index) const;

    /** Get positions. */
    const std::vector<LPoint3f>& get_positions(void) const;

    std::vector<LPoint3f>& modify_positions(void);

    /** Set the position on the point_index-th point. */
    void set_position(const LPoint3f& positions, int point_index);

    /**
     * Set the positions.
     *
     * If the size is different with the original size,
     * then re-create the vertex memory and the index memory.
     */
    void set_positions(const std::vector<LPoint3f>& positions);

    /** Upload transform buffer texture to GPU. */
    void upload_positions(void);

    /** Get the total count of points. */
    int get_point_count(void) const;

    /** Get the active count of points. */
    int get_active_point_count(void) const;

    /**
     * Set a count of active points.
     *
     * Panda3D will draw points as given counts.
     * If @a count is -1, all points will be used.
     */
    void set_active_point_count(int count);

    float get_radius(void) const;
    void set_radius(float radius);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}

