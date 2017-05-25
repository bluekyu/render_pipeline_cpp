#include "render_pipeline/rpcore/util/instancing_node.hpp"

#include <omniBoundingVolume.h>

#include "render_pipeline/rpcore/render_pipeline.h"

namespace rpcore {

static_assert(std::is_standard_layout<LMatrix4f>::value, "std::is_standard_layout<LMatrix4f>::value");
static_assert(sizeof(LMatrix4f) == sizeof(float)*16, "sizeof(LMatrix4f) == sizeof(float)*16");

struct InstancingNode::Impl
{
    Impl(NodePath np, const std::string& effect_path, GeomEnums::UsageHint buffer_hint);

    void set_instance_count(int instance_count);

    void set_transform(const LMatrix4f& transform, int instance_index);
    void set_transforms(const std::vector<LMatrix4f>& transforms);

    void upload_transforms(void);

    bool dirty_ = true;
    NodePath instanced_np_;
    std::vector<LMatrix4f> transforms_;
    PT(Texture) buffer_texture_;
};

InstancingNode::Impl::Impl(NodePath np, const std::string& effect_path, GeomEnums::UsageHint buffer_hint): instanced_np_(np)
{
    // Allocate storage for the matrices, each matrix has 16 elements,
    // but because one pixel has four components, we need amount * 4 pixels.
    buffer_texture_ = new Texture;
    buffer_texture_->setup_buffer_texture(4, Texture::T_float, Texture::F_rgba32, buffer_hint);

    // Load the effect
    std::string epath = effect_path;
    if (epath.empty())
        epath = "effects/basic_instancing.yaml";

    rpcore::RenderPipeline::get_global_ptr()->set_effect(instanced_np_, epath, {});

    instanced_np_.set_shader_input("InstancingData", buffer_texture_);
    instanced_np_.set_shader_input("instanceNode", instanced_np_);

    // We have do disable culling, so that all instances stay visible
    instanced_np_.node()->set_bounds(new OmniBoundingVolume);
    instanced_np_.node()->set_final(true);
}

void InstancingNode::Impl::set_instance_count(int instance_count)
{
    dirty_ = true;
    transforms_.resize(instance_count);
}

void InstancingNode::Impl::set_transform(const LMatrix4f& transform, int instance_index)
{
    dirty_ = true;
    transforms_[instance_index] = transform;
}

void InstancingNode::Impl::set_transforms(const std::vector<LMatrix4f>& transforms)
{
    dirty_ = true;
    transforms_ = transforms;
}

void InstancingNode::Impl::upload_transforms(void)
{
    if (!dirty_)
        return;

    const int instance_count = static_cast<int>(transforms_.size());
    if (instanced_np_.get_instance_count() != instance_count)
    {
        instanced_np_.set_instance_count(instance_count);

        // Allocate storage for the matrices, each matrix has 16 elements,
        // but because one pixel has four components, we need amount * 4 pixels.
        buffer_texture_->set_x_size(instance_count * 4);
    }

    auto ram_image = buffer_texture_->modify_ram_image();
    std::memcpy(ram_image.p(), &transforms_[0], instance_count * sizeof(LMatrix4f));

    dirty_ = false;
}

InstancingNode::InstancingNode(NodePath np, const std::string& effect_path, GeomEnums::UsageHint buffer_hint):
    impl_(std::make_unique<Impl>(np, effect_path, buffer_hint))
{
}

InstancingNode::InstancingNode(NodePath np, const std::vector<LMatrix4f>& transforms,
    const std::string& effect_path, GeomEnums::UsageHint buffer_hint): InstancingNode(np, effect_path, buffer_hint)
{
    set_transforms(transforms);
}

InstancingNode::~InstancingNode(void) = default;

NodePath InstancingNode::get_instanced_node(void) const
{
    return impl_->instanced_np_;
}

int InstancingNode::get_instance_count(void) const
{
    return static_cast<int>(impl_->transforms_.size());
}

void InstancingNode::set_instance_count(int instance_count)
{
    impl_->set_instance_count(instance_count);
}

void InstancingNode::add_instance(const LMatrix4f& transform, bool upload)
{
    impl_->dirty_ = true;
    impl_->transforms_.push_back(transform);

    if (upload)
        upload_transforms();
}

void InstancingNode::remove_instance(int instance_index, bool upload)
{
    if (instance_index >= static_cast<int>(impl_->transforms_.size()))
    {
        RPObject::global_error("InstancingNode", "Out of range in transform of InstancingNode.");
        return;
    }

    impl_->dirty_ = true;
    impl_->transforms_.erase(impl_->transforms_.begin() + instance_index);

    if (upload)
        upload_transforms();
}

const LMatrix4f& InstancingNode::get_transform(int instance_index) const
{
    return impl_->transforms_[instance_index];
}

const std::vector<LMatrix4f>& InstancingNode::get_transforms(void) const
{
    return impl_->transforms_;
}

std::vector<LMatrix4f>& InstancingNode::modify_transforms(void)
{
    impl_->dirty_ = true;
    return impl_->transforms_;
}

void InstancingNode::set_transform(const LMatrix4f& transform, int instance_index)
{
    if (instance_index >= static_cast<int>(impl_->transforms_.size()))
    {
        RPObject::global_error("InstancingNode", "Out of range in transform of InstancingNode.");
        return;
    }

    impl_->set_transform(transform, instance_index);
}

void InstancingNode::set_transforms(const std::vector<LMatrix4f>& transforms)
{
    impl_->set_transforms(transforms);
}

void InstancingNode::upload_transforms(void)
{
    impl_->upload_transforms();
}

LMatrix4f InstancingNode::get_matrix_of_child(const NodePath& child, int instance_index, const NodePath& other) const
{
    return child.get_mat(impl_->instanced_np_) * get_transform(instance_index) * impl_->instanced_np_.get_mat(other);
}

}