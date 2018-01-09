# Panda3D Actor
**Languages**: [English](../../rendering/actors.md)

Panda3D C++ 에서는 Actor 클래스가 없으나 Render Pipeline C++ 에서 Actor 클래스를 제공하며, 동일한 기능을 가진다.

## Actor 사용하기
Actor 사용법은 Python 방식과 거의 유사하며, Python 과 동일한 API 를 제공한다.

```cpp
PT(Actor) nodepath = new Actor(
    Actor::ModelsType{"Model Path"},
    Actor::AnimsType{
        {"Animation Name 1", "Animation Path 1"},
        {"Animation Name 2", "Animation Path 2"}});
```

세부 설명은 아래 Panda3D 문서를 참고하면 된다.
- [Panda3D Manual: Loading Actors and Animations](https://www.panda3d.org/manual/index.php/Loading_Actors_and_Animations)
- [Panda3D Manual: Actor Animations](https://www.panda3d.org/manual/index.php/Actor_Animations)
- [Panda3D Manual: Attaching an Object to a Joint](https://www.panda3d.org/manual/index.php/Attaching_an_Object_to_a_Joint)
- [Panda3D Manual: Controlling a Joint Procedurally](https://www.panda3d.org/manual/index.php/Controlling_a_Joint_Procedurally)



## Joint Transform 설명
Actor 클래스에서는 `control_joint` 와 `expose_joint` 를 통해서 joint 데이터를 외부에서 접근할 수 있도록 한다.
Control joint 는 NodePath 의 local transform 값을 연결된 joint 로 반영하고,
expose joint 는 연결된 joint 의 transform 결과를 NodePath 에 반영한다.

이때, `expose_joint` 를 수행할 때 `local_transform` 인자 값으로 false (기본값)가 주어지면 NodePath 에
root joint 까지의 net transform 값을 제공하고, true 가 주어지면 local transform 값을 제공한다.

따라서 control joint 의 경우에 리턴된 NodePath 의 net transform (world transform) 은 의미가 없을 수 있다.
그리고 expose joint 도 마찬가지로 다른 부모를 가질 경우 net transform 의미가 없어지게 된다.

### Joint Tree
Control joint 의 net transform 값을 의미있게 하기 위해서는 joint tree 를 구성할 필요가 있다.
한 방법은 원하는 joint 의 부모 joint 를 expose 하는 방법이고, 다른 방법은 루트 joint 부터
원하는 joint 까지 전부 control joint 로 만드는 방법이다.

전자는 control joint 의 부모 joint 에 `expose_joint` 를 수행하고 리턴된 NodePath 를 control joint 의
부모로 변경해주면 된다. 결과적으로 control joint 의 net transform 은 expose joint 까지의 transform 와
control joint 의 local transform 을 반영한 것이 되므로 의미있는 값을 가지게 된다.

후자는 Actor 의 joint 구조를 그대로 가져와서 scene graph 를 구성하는 방식으로, 구조가 같기 때문에 자연스럽게
의미있는 transform 값을 가진다. 다만, 루트 joint 부터 tree 를 구성하지 않는다면 전자와 같은 방식이 필요하다.

Expose joint 의 경우에는 joint tree 가 필요 없을 수 있으나, joint 간의 연결이 필요하다면
`local_transform` 값에 true 를 넘겨서 joint tree 를 위와 같은 방법으로 구성할 수 있다.

#### Joint Tree 구성하기
후자의 방식을 사용한다면, tree 에 해당하는 joint 들에 `Actor::control_joint` 함수를 사용하면 된다.
다만, 많은 수의 joint 에 대해서는 매번 joint 를 찾는 과정을 수행하기 때문에 비효율적일 수 있다.

아래 코드에서는 joint tree 를 직접 traverse 하기 때문에 효율적이며, 모든 joint 의 이름을 미리 알지 않아도 되는 장점이 있다.

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
