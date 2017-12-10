# Collision Detection

## Clicking on 3D Objects

See also https://www.panda3d.org/manual/index.php?title=Clicking_on_3D_Objects&language=cxx

Note that primitive type of the geometry should be `Geom::PT_polygons`. If not, collision is not detected.
(See `CollisionTraverser::compare_collider_to_geom` function)

```cpp
#include <mouseWatcher.h>
#include <collisionNode.h>
#include <collisionRay.h>
#include <collisionHandlerQueue.h>

MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    picker_ = std::make_unique<CollisionTraverser>("picker");

    PT(CollisionNode) picker_node = new CollisionNode("mouse_ray");
    NodePath picker_np = rpcore::Globals::base->get_cam().attach_new_node(picker_node);
    picker_node->set_from_collide_mask(GeomNode::get_default_collide_mask());
    picker_ray_ = new CollisionRay;
    picker_node->add_solid(picker_ray_);
    collision_handler_ = new CollisionHandlerQueue;
    picker_->add_collider(picker_np, collision_handler_);

    rpcore::Globals::base->accept("mouse3", [this](const Event*) { picking(); });
}

void MainUI::picking()
{
    auto mw_node = rpcore::Globals::base->get_mouse_watcher_node();
    if (!mw_node->has_mouse())
        return;

    const auto& mpos = mw_node->get_mouse();
    if (!picker_ray_->set_from_lens(rpcore::Globals::base->get_cam_node(), mpos))
        return;

    picker_->traverse(rpcore::Globals::render);
    if (collision_handler_->get_num_entries() > 0)
    {
        collision_handler_->sort_entries();

        NodePath found = collision_handler_->get_entry(0)->get_into_node_path();

        // handle found ...
    }
}
```
