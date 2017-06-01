#include "render_pipeline/rpcore/util/circular_points_node.hpp"

#include <shaderAttrib.h>
#include <geomNode.h>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/util/primitives.hpp"

namespace rpcore {

static_assert(std::is_standard_layout<LMatrix4f>::value, "std::is_standard_layout<LMatrix4f>::value");
static_assert(sizeof(LMatrix4f) == sizeof(float)*16, "sizeof(LMatrix4f) == sizeof(float)*16");

struct CircularPointsNode::Impl
{
    Impl(const std::string& name, const std::vector<LPoint3f>& positions, float radius, const std::string& effect_path,
        GeomEnums::UsageHint buffer_hint);

    void set_position(const LPoint3f& position, int point_index);
    void set_positions(const std::vector<LPoint3f>& positions);

    void set_radius(float radius);

    void upload_positions(void);

    void set_active_point_count(int count);

    bool dirty_ = true;
    NodePath points_np_;
    std::vector<LPoint3f> positions_;
};

CircularPointsNode::Impl::Impl(const std::string& name, const std::vector<LPoint3f>& positions, float radius,
    const std::string& effect_path, GeomEnums::UsageHint buffer_hint)
{
    points_np_ = create_points(name, positions);
    positions_ = positions;

    // Load the effect
    std::string epath = effect_path;
    if (epath.empty())
        epath = "effects/circular_points.yaml";

    rpcore::RenderPipeline::get_global_ptr()->set_effect(points_np_, epath);

    set_radius(radius);
    points_np_.set_attrib(DCAST(ShaderAttrib, points_np_.get_attrib(ShaderAttrib::get_class_type()))->set_flag(ShaderAttrib::F_shader_point_size, true));
}

void CircularPointsNode::Impl::set_position(const LPoint3f& position, int point_index)
{
    dirty_ = true;
    positions_[point_index] = position;
}

void CircularPointsNode::Impl::set_positions(const std::vector<LPoint3f>& positions)
{
    if (positions.size() == positions_.size())
    {
        dirty_ = true;
        positions_ = positions;
    }
    else
    {
        RPObject::global_error("CircularPointsNode",
            fmt::format("The size of positions ({}) is NOT same as current size ({}).", positions.size(), positions_.size()));
    }
}

void CircularPointsNode::Impl::set_radius(float radius)
{
    points_np_.set_shader_input("point_radius", radius);
}

void CircularPointsNode::Impl::upload_positions(void)
{
    if (!dirty_)
        return;

    PT(GeomVertexArrayData) vertex_array = DCAST(GeomNode, points_np_.node())->modify_geom(0)->modify_vertex_data()->modify_array(0);

    vertex_array->modify_handle()->copy_data_from(
        reinterpret_cast<const unsigned char*>(positions_.data()),
        positions_.size() * sizeof(decltype(positions_)::value_type));

    dirty_ = false;
}

void CircularPointsNode::Impl::set_active_point_count(int count)
{
    if (count > positions_.size())
    {
        RPObject::global_warn("CircularPointsNode",
            fmt::format("Given count ({}) is bigger than the count of stored points ({}).", count, positions_.size()));
        return;
    }

    DCAST(GeomNode, points_np_.node())->modify_geom(0)->modify_primitive(0)->modify_vertices(count);
}

// ************************************************************************************************
CircularPointsNode::CircularPointsNode(const std::string& name, const std::vector<LPoint3f>& positions, float radius,
    const std::string& effect_path, GeomEnums::UsageHint buffer_hint): impl_(std::make_unique<Impl>(name, positions,
        radius, effect_path, buffer_hint))
{
}

CircularPointsNode::~CircularPointsNode(void) = default;

NodePath CircularPointsNode::get_nodepath(void) const
{
    return impl_->points_np_;
}

int CircularPointsNode::get_point_count(void) const
{
    return static_cast<int>(impl_->positions_.size());
}

const LPoint3f& CircularPointsNode::get_position(int point_index) const
{
    return impl_->positions_[point_index];
}

const std::vector<LPoint3f>& CircularPointsNode::get_positions(void) const
{
    return impl_->positions_;
}

LPoint3f* CircularPointsNode::modify_positions(void)
{
    impl_->dirty_ = true;
    return impl_->positions_.data();
}

void CircularPointsNode::set_position(const LPoint3f& position, int point_index)
{
    if (point_index >= static_cast<int>(impl_->positions_.size()))
    {
        RPObject::global_error("CircularPointsNode", "Out of range of positions of CircularPointsNode.");
        return;
    }

    impl_->set_position(position, point_index);
}

void CircularPointsNode::set_positions(const std::vector<LPoint3f>& positions)
{
    impl_->set_positions(positions);
}

void CircularPointsNode::upload_positions(void)
{
    impl_->upload_positions();
}

void CircularPointsNode::set_radius(float radius)
{
    impl_->set_radius(radius);
}

void CircularPointsNode::set_active_point_count(int count)
{
    impl_->set_active_point_count(count);
}

}