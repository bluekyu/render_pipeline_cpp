#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.h>

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
class RPCPP_DECL InstancingNode
{
public:
    InstancingNode(NodePath np, const std::string& effect_path="",
        GeomEnums::UsageHint buffer_hint=GeomEnums::UH_static);
    InstancingNode(NodePath np, const std::vector<LMatrix4f>& transforms,
        const std::string& effect_path="", GeomEnums::UsageHint buffer_hint=GeomEnums::UH_static);

    ~InstancingNode(void);

    /** Get the count of instances. */
    int get_instance_count(void) const;

    /** Set the count of instances. */
    void set_instance_count(int instance_count);

    void add_instance(const LMatrix4f& transform, bool upload=true);

    void remove_instance(int instance_index, bool upload=true);

    /** Get tranform matrix. */
    const LMatrix4f& get_transform(int instance_index) const;

    /** Get tranforms. */
    const std::vector<LMatrix4f>& get_transforms(void) const;

    /** Set local transform. */
    void set_transform(const LMatrix4f& transform, int instance_index);

    /** Set local transforms and change instance count to the size of @p transforms. */
    void set_transforms(const std::vector<LMatrix4f>& transforms);

    /** Upload transform buffer texture to GPU. */
    void upload_transforms(void);

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

}
