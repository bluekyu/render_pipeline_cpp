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

#include "render_pipeline/rpcore/util/instancing_node.hpp"

#include <omniBoundingVolume.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"

namespace rpcore {

static_assert(std::is_standard_layout<LMatrix4f>::value, "std::is_standard_layout<LMatrix4f>::value");
static_assert(sizeof(LMatrix4f) == sizeof(float)*16, "sizeof(LMatrix4f) == sizeof(float)*16");

class InstancingNode::Impl
{
public:
    void initilize(RenderPipeline& pipeline, NodePath np, const Filename& effect_path, GeomEnums::UsageHint buffer_hint);

    void set_instance_count(int instance_count);

    void set_transform(const LMatrix4f& transform, int instance_index);
    void set_transforms(const std::vector<LMatrix4f>& transforms);

    void upload_transforms();

public:
    bool dirty_ = true;
    NodePath instanced_np_;
    std::vector<LMatrix4f> transforms_;
    PT(Texture) buffer_texture_;
};

void InstancingNode::Impl::initilize(RenderPipeline& pipeline, NodePath np, const Filename& effect_path, GeomEnums::UsageHint buffer_hint)
{
    instanced_np_ = np;

    // Allocate storage for the matrices, each matrix has 16 elements,
    // but because one pixel has four components, we need amount * 4 pixels.
    buffer_texture_ = new Texture;
    buffer_texture_->setup_buffer_texture(4, Texture::T_float, Texture::F_rgba32, buffer_hint);

    // Load the effect
    Filename epath = effect_path;
    if (epath.empty())
        epath = "/$$rp/effects/basic_instancing.yaml";

    pipeline.set_effect(instanced_np_, epath, {});

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

void InstancingNode::Impl::upload_transforms()
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
    memcpy(ram_image.p(), &transforms_[0], instance_count * sizeof(LMatrix4f));

    dirty_ = false;
}

// ************************************************************************************************

InstancingNode::InstancingNode(RenderPipeline& pipeline, NodePath np, const Filename& effect_path, GeomEnums::UsageHint buffer_hint):
    impl_(std::make_unique<Impl>())
{
    impl_->initilize(pipeline, np, effect_path, buffer_hint);
}

InstancingNode::InstancingNode(RenderPipeline& pipeline, NodePath np, const std::vector<LMatrix4f>& transforms,
    const Filename& effect_path, GeomEnums::UsageHint buffer_hint): InstancingNode(pipeline, np, effect_path, buffer_hint)
{
    set_transforms(transforms);
}

InstancingNode::~InstancingNode() = default;

NodePath InstancingNode::get_instanced_node() const
{
    return impl_->instanced_np_;
}

int InstancingNode::get_instance_count() const
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

const std::vector<LMatrix4f>& InstancingNode::get_transforms() const
{
    return impl_->transforms_;
}

std::vector<LMatrix4f>& InstancingNode::modify_transforms()
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

void InstancingNode::upload_transforms()
{
    impl_->upload_transforms();
}

LMatrix4f InstancingNode::get_matrix_of_child(const NodePath& child, int instance_index, const NodePath& other) const
{
    return child.get_mat(impl_->instanced_np_) * get_transform(instance_index) * impl_->instanced_np_.get_mat(other);
}

}