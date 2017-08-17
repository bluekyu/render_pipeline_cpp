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

#include "render_pipeline/rpcore/util/points_node.hpp"

#include <shaderAttrib.h>
#include <geomNode.h>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/util/primitives.hpp"

namespace rpcore {

static_assert(std::is_standard_layout<LMatrix4f>::value, "std::is_standard_layout<LMatrix4f>::value");
static_assert(sizeof(LMatrix4f) == sizeof(float)*16, "sizeof(LMatrix4f) == sizeof(float)*16");

class PointsNode::Impl
{
public:
    void initialize(const std::string& name, const std::vector<LPoint3f>& positions, float radius,
        GeomEnums::UsageHint buffer_hint);

    int get_active_point_count() const;

    void set_position(const LPoint3f& position, int point_index);
    void set_positions(const std::vector<LPoint3f>& positions);

    void set_radius(float radius);

    void upload_positions();

    void set_active_point_count(int count);

public:
    bool dirty_ = true;
    NodePath points_np_;
    std::vector<LPoint3f> positions_;
};

void PointsNode::Impl::initialize(const std::string& name, const std::vector<LPoint3f>& positions, float radius,
    GeomEnums::UsageHint buffer_hint)
{
    points_np_ = create_points(name, positions_ = positions);

    set_radius(radius);
    points_np_.set_attrib(DCAST(ShaderAttrib, points_np_.get_attrib(ShaderAttrib::get_class_type()))->set_flag(ShaderAttrib::F_shader_point_size, true));
}

int PointsNode::Impl::get_active_point_count() const
{
    return DCAST(GeomNode, points_np_.node())->get_geom(0)->get_primitive(0)->get_num_vertices();
}

void PointsNode::Impl::set_position(const LPoint3f& position, int point_index)
{
    dirty_ = true;
    positions_[point_index] = position;
}

void PointsNode::Impl::set_positions(const std::vector<LPoint3f>& positions)
{
    if (positions.size() == positions_.size())
        positions_ = positions;
    else
        modify_points(DCAST(GeomNode, points_np_.node()), positions_ = positions);

    dirty_ = true;
}

void PointsNode::Impl::set_radius(float radius)
{
    points_np_.set_shader_input("point_radius", radius);
}

void PointsNode::Impl::upload_positions()
{
    if (!dirty_)
        return;

    PT(GeomVertexArrayData) vertex_array = DCAST(GeomNode, points_np_.node())->modify_geom(0)->modify_vertex_data()->modify_array(0);

    vertex_array->modify_handle()->copy_data_from(
        reinterpret_cast<const unsigned char*>(positions_.data()),
        positions_.size() * sizeof(decltype(positions_)::value_type));

    dirty_ = false;
}

void PointsNode::Impl::set_active_point_count(int count)
{
    if (count > positions_.size())
    {
        RPObject::global_warn("PointsNode",
            fmt::format("Given count ({}) is bigger than the count of stored points ({}).", count, positions_.size()));
        return;
    }

    DCAST(GeomNode, points_np_.node())->modify_geom(0)->modify_primitive(0)->modify_vertices(count);
}

// ************************************************************************************************
PointsNode::PointsNode(const std::string& name, const std::vector<LPoint3f>& positions, float radius,
    GeomEnums::UsageHint buffer_hint): impl_(std::make_unique<Impl>())
{
    impl_->initialize(name, positions, radius, buffer_hint);
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
PointsNode::PointsNode(PointsNode&&) = default;
#endif

PointsNode::~PointsNode() = default;

#if !defined(_MSC_VER) || _MSC_VER >= 1900
PointsNode& PointsNode::operator=(PointsNode&& other) = default;
#endif

NodePath PointsNode::get_nodepath() const
{
    return impl_->points_np_;
}

int PointsNode::get_point_count() const
{
    return static_cast<int>(impl_->positions_.size());
}

int PointsNode::get_active_point_count() const
{
    return impl_->get_active_point_count();
}

float PointsNode::get_radius() const
{
    return impl_->points_np_.get_shader_input("point_radius").get_vector().get_x();
}

void PointsNode::set_circular_point() const
{
    rpcore::RenderPipeline::get_global_ptr()->set_effect(impl_->points_np_, "/$$rp/effects/circular_points.yaml");
}

const LPoint3f& PointsNode::get_position(int point_index) const
{
    return impl_->positions_[point_index];
}

const std::vector<LPoint3f>& PointsNode::get_positions() const
{
    return impl_->positions_;
}

std::vector<LPoint3f>& PointsNode::modify_positions()
{
    impl_->dirty_ = true;
    return impl_->positions_;
}

void PointsNode::set_position(const LPoint3f& position, int point_index)
{
    if (point_index >= static_cast<int>(impl_->positions_.size()))
    {
        RPObject::global_error("PointsNode", "Out of range of positions of PointsNode.");
        return;
    }

    impl_->set_position(position, point_index);
}

void PointsNode::set_positions(const std::vector<LPoint3f>& positions)
{
    impl_->set_positions(positions);
}

void PointsNode::upload_positions()
{
    impl_->upload_positions();
}

void PointsNode::set_radius(float radius)
{
    impl_->set_radius(radius);
}

void PointsNode::set_active_point_count(int count)
{
    impl_->set_active_point_count(count);
}

}