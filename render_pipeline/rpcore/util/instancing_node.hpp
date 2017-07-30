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
 * Wrapper class to add instanced mesh.
 *
 * This class is wrapper class to add instanced mesh, so removing this class does NOT affect instancing.
 * Instanced node has additional local transforms.
 *
 * The general model matrix is
 * "world_matrix_to_instanced_node * local_transform_of_subtree_of_instanced_node * vertex".
 *
 * But model matrix of instanced nodes is calculated by
 * "world_matrix_to_instanced_node * local_transform[index] * local_transform_of_child_of_instanced_node * vertex".
 */
class RENDER_PIPELINE_DECL InstancingNode
{
public:
    InstancingNode(NodePath np, const std::string& effect_path="",
        GeomEnums::UsageHint buffer_hint=GeomEnums::UH_static);
    InstancingNode(NodePath np, const std::vector<LMatrix4f>& transforms,
        const std::string& effect_path="", GeomEnums::UsageHint buffer_hint=GeomEnums::UH_static);

    ~InstancingNode(void);

    NodePath get_instanced_node(void) const;

    /** Get the count of instances. */
    int get_instance_count(void) const;

    /** Set the count of instances. */
    void set_instance_count(int instance_count);

    void add_instance(const LMatrix4f& transform, bool upload=false);

    void remove_instance(int instance_index, bool upload=false);

    /** Get local instancing tranform matrix. */
    const LMatrix4f& get_transform(int instance_index) const;

    /** Get local instancing tranforms. */
    const std::vector<LMatrix4f>& get_transforms(void) const;

    /** Modify local instancing tranforms. */
    std::vector<LMatrix4f>& modify_transforms(void);

    /** Set local instancing transform. */
    void set_transform(const LMatrix4f& transform, int instance_index);

    /** Set local instancing transforms and change instance count to the size of @p transforms. */
    void set_transforms(const std::vector<LMatrix4f>& transforms);

    /** Upload transform buffer texture to GPU. */
    void upload_transforms(void);

    /**
     * Get child matrix relative to other with instancing transform.
     * child.get_mat(instanced_node) * get_transform(instance_index) * instanced_node.get_mat(other)
     */
    LMatrix4f get_matrix_of_child(const NodePath& child, int instance_index, const NodePath& other) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
