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

#include "render_pipeline/rpcore/util/line_node.hpp"

#include <geomLinestrips.h>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/util/rpgeomnode.hpp"
#include "render_pipeline/rpcore/util/rpmaterial.hpp"

namespace rpcore {

class LineNode::Impl
{
public:
    void initialize(const std::string& name, const std::vector<LPoint3f>& positions, float thickness,
        GeomEnums::UsageHint buffer_hint);

    int get_active_point_count() const;

    void set_position(const LPoint3f& position, int point_index);
    void set_positions(const std::vector<LPoint3f>& positions);

    void set_thickness(float thickness);

    void upload_positions();

    void set_active_point_count(int count);

public:
    bool dirty_ = true;
    NodePath line_np_;
    std::vector<LPoint3f> positions_;
};

void LineNode::Impl::initialize(const std::string& name, const std::vector<LPoint3f>& positions, float thickness,
    GeomEnums::UsageHint buffer_hint)
{
    set_positions(positions);

    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3(), buffer_hint);
    PT(GeomLinestrips) prim = new GeomLinestrips(buffer_hint);

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

    CPT(RenderAttrib) thick = RenderModeAttrib::make(RenderModeAttrib::M_unchanged, thickness);
    CPT(RenderState) state = RenderState::make(thick);

    PT(GeomNode) geom_node = new GeomNode(name);
    geom_node->add_geom(geom, state);

    // default material
    line_np_ = NodePath(geom_node);
    RPGeomNode gn(geom_node);
    gn.set_material(0, RPMaterial());
}

int LineNode::Impl::get_active_point_count() const
{
    return DCAST(GeomNode, line_np_.node())->get_geom(0)->get_primitive(0)->get_num_vertices();
}

void LineNode::Impl::set_position(const LPoint3f& position, int point_index)
{
    dirty_ = true;
    positions_[point_index] = position;
}

void LineNode::Impl::set_positions(const std::vector<LPoint3f>& positions)
{
    if (positions.size() > (std::numeric_limits<int>::max)())
    {
        RPObject::global_error("LineNode", fmt::format("The size {} of points is more than the maximum size ({}).",
            positions.size(), (std::numeric_limits<int>::max)()));
        return;
    }

    positions_ = positions;

    dirty_ = true;
}

void LineNode::Impl::set_thickness(float thickness)
{
    line_np_.set_render_mode_thickness(thickness);
}

void LineNode::Impl::upload_positions()
{
    if (!dirty_)
        return;

    auto geom = DCAST(GeomNode, line_np_.node())->modify_geom(0);
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

void LineNode::Impl::set_active_point_count(int count)
{
    if (count > positions_.size())
    {
        RPObject::global_warn("LineNode",
            fmt::format("Given count ({}) is bigger than the count of stored points ({}).", count, positions_.size()));
        return;
    }

    DCAST(GeomNode, line_np_.node())->modify_geom(0)->modify_primitive(0)->modify_vertices(count);
}

// ************************************************************************************************

void LineNode::set_vertex_color_line_effect(NodePath np)
{
    rpcore::RenderPipeline::get_global_ptr()->set_effect(np, "/$$rp/effects/vcolor_line.yaml");
}

void LineNode::set_line_effect(NodePath np)
{
    auto pipeline = rpcore::RenderPipeline::get_global_ptr();
    if (pipeline->is_stereo_mode())
        pipeline->set_effect(np, "/$$rp/effects/line.yaml");
}

LineNode::LineNode(const std::string& name, const std::vector<LPoint3f>& points,
    float thickness, GeomEnums::UsageHint buffer_hint): impl_(std::make_unique<Impl>())
{
    impl_->initialize(name, points, thickness, buffer_hint);
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
LineNode::LineNode(LineNode&&) = default;
#endif

LineNode::~LineNode() = default;

#if !defined(_MSC_VER) || _MSC_VER >= 1900
LineNode& LineNode::operator=(LineNode&& other) = default;
#endif

NodePath LineNode::get_nodepath() const
{
    return impl_->line_np_;
}

int LineNode::get_point_count() const
{
    return static_cast<int>(impl_->positions_.size());
}

int LineNode::get_active_point_count() const
{
    return impl_->get_active_point_count();
}

float LineNode::get_thickness() const
{
    return impl_->line_np_.get_render_mode_thickness();
}

void LineNode::set_line_effect() const
{
    set_line_effect(impl_->line_np_);
}

const LPoint3f& LineNode::get_position(int point_index) const
{
    return impl_->positions_[point_index];
}

const std::vector<LPoint3f>& LineNode::get_positions() const
{
    return impl_->positions_;
}

std::vector<LPoint3f>& LineNode::modify_positions()
{
    impl_->dirty_ = true;
    return impl_->positions_;
}

void LineNode::set_position(const LPoint3f& position, int point_index)
{
    if (point_index >= static_cast<int>(impl_->positions_.size()))
    {
        RPObject::global_error("LineNode", "Out of range of positions of LineNode.");
        return;
    }

    impl_->set_position(position, point_index);
}

void LineNode::set_positions(const std::vector<LPoint3f>& positions)
{
    impl_->set_positions(positions);
}

void LineNode::upload_positions()
{
    impl_->upload_positions();
}

void LineNode::set_thickness(float thickness)
{
    impl_->set_thickness(thickness);
}

void LineNode::set_active_point_count(int count)
{
    impl_->set_active_point_count(count);
}

}