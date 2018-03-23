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
#include <geomPoints.h>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/util/rpgeomnode.hpp"

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
    set_positions(positions);

    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3(), buffer_hint);
    PT(GeomPoints) prim = new GeomPoints(buffer_hint);

    const int count = static_cast<int>(positions_.size());

    vdata->unclean_set_num_rows(count);
    vdata->modify_array(0)->modify_handle()->copy_data_from(
        reinterpret_cast<const unsigned char*>(positions_.data()),
        count * sizeof(std::remove_reference<decltype(positions_)>::type::value_type));

    prim->clear_vertices();
    prim->add_consecutive_vertices(0, count);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    CPT(RenderState) state = RenderState::make(DCAST(ShaderAttrib, ShaderAttrib::make_default())->set_flag(ShaderAttrib::F_shader_point_size, true));

    PT(GeomNode) geom_node = new GeomNode(name);
    geom_node->add_geom(geom, state);

    // default material
    points_np_ = NodePath(geom_node);
    RPGeomNode gn(geom_node);
    gn.set_material(0, RPMaterial());

    set_radius(radius);
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
    if (positions.size() > (std::numeric_limits<int>::max)())
    {
        RPObject::global_error("PointsNode", fmt::format("The size {} of points is more than the maximum size ({}).",
            positions.size(), (std::numeric_limits<int>::max)()));
        return;
    }

    positions_ = positions;

    dirty_ = true;
}

void PointsNode::Impl::set_radius(float radius)
{
    points_np_.set_shader_input("point_radius", LVecBase4(radius));
}

void PointsNode::Impl::upload_positions()
{
    if (!dirty_)
        return;

    auto geom = DCAST(GeomNode, points_np_.node())->modify_geom(0);
    auto vdata = geom->modify_vertex_data();

    const int count = static_cast<int>(positions_.size());
    if (vdata->get_num_rows() != count)
    {
        auto prim = geom->modify_primitive(0);

        vdata->unclean_set_num_rows(count);

        prim->clear_vertices();
        prim->add_consecutive_vertices(0, count);
        prim->close_primitive();
    }

    vdata->modify_array(0)->modify_handle()->copy_data_from(
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

void PointsNode::set_square_point_effect(NodePath np)
{
    rpcore::RenderPipeline::get_global_ptr()->set_effect(np, "/$$rp/effects/square_point.yaml");
}

void PointsNode::set_disk_point_effect(NodePath np)
{
    rpcore::RenderPipeline::get_global_ptr()->set_effect(np, "/$$rp/effects/disk_point.yaml");
}

void PointsNode::set_sphere_point_effect(NodePath np)
{
    rpcore::RenderPipeline::get_global_ptr()->set_effect(np, "/$$rp/effects/sphere_point.yaml");
}

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

void PointsNode::set_square_point_effect() const
{
    set_square_point_effect(impl_->points_np_);
}

void PointsNode::set_disk_point_effect() const
{
    set_disk_point_effect(impl_->points_np_);
}

void PointsNode::set_sphere_point_effect() const
{
    set_sphere_point_effect(impl_->points_np_);
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