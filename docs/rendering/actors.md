# Panda3D Actor
**Languages**: [한국어](../ko_kr/rendering/actors.md)



## Using Actor


















## Joint Transform for Details










### Joint Tree














#### Constructing Joint Tree





```cpp
/**
 * Create control joint and sub control joints.
 */
void control_joint_tree(PartGroup* joint, NodePath parent)
{
    if (!joint)
        return;

    // Skip if the non-joint
    if (joint->is_of_type(MovingPartMatrix::get_class_type()))
    {
        const auto& joint_name = joint->get_name();

        NodePath node = parent.attach_new_node(new ModelNode(joint_name));

        node.set_mat(DCAST(MovingPartMatrix, joint)->get_default_value());

        if (!joint->apply_control(node.node()))
            std::cout << "Cannot control joint " << joint_name << std::endl;

        parent = node;
    }

    for (int k = 0, k_end = joint->get_num_children(); k < k_end; ++k)
        control_joint_tree(joint->get_child(k), parent);
}

/**
 * Control a joint and sub-joints given joint_name and part_name, lod_name.
 *
 * @return  NodePath of control joint given joint_name and has children for sub-joints.
 *          The default parent of the node is a character node.
 */
NodePath control_joint_tree(rppanda::Actor* actor, const std::string& part_name, const std::string& joint_name, const std::string& lod_name)
{
    auto part_def = actor->get_part_def(part_name, lod_name);
    if (!part_def)
    {
        std::cout << "No part named: " << part_name << std::endl;
        return {};
    }

    auto joint = part_def->get_bundle()->find_child(joint_name);
    if (joint && joint->is_of_type(MovingPartMatrix::get_class_type()))
    {
        const auto& joint_name = joint->get_name();

        NodePath node = part_def->part_bundle_np.attach_new_node(new ModelNode(joint_name));

        node.set_mat(DCAST(MovingPartMatrix, joint)->get_default_value());

        if (!joint->apply_control(node.node()))
            std::cout << "Cannot control joint " << joint_name << std::endl;

        // build subtree of the joint more faster than Actor::control_joint
        for (int k = 0, k_end = joint->get_num_children(); k < k_end; ++k)
            control_joint_tree(joint->get_child(k), node);

        return node;
    }
    else
    {
        return {};
    }
}

/**
 * Control all joints given part_name and lod_name.
 *
 * @return  NodePath of a root of joints tree. This node is not in world.
 */
NodePath control_all_joints(rppanda::Actor* actor, const std::string& part_name, const std::string& lod_name)
{
    auto bundle = actor->get_part_bundle(part_name, lod_name);
    if (!bundle)
    {
        std::cout << "No part named: " << part_name << std::endl;
        return {};
    }

    NodePath node(bundle->get_name());
    control_joint_tree(bundle, node);
    return node;
}
```
