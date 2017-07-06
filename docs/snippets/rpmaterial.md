# Render Pipeline Material

## Transparent
To add:
```cpp
NodePath nodepath = ...;

rpcore::RPMaterial mat;
mat.set_shading_model(rpcore::RPMaterial::ShadingModel::TRANSPARENT_MODEL);
mat.set_alpha(0.3f);

// set to 0th geometry
rpcore::RPGeomNode(nodepath).set_material(0, mat);

// add effect
render_pipeline->prepare_scene(nodepath);
```

To remove:
```cpp
NodePath nodepath = ...;

rpcore::RPGeomNode gn(nodepath);

// get from 0th geometry
rpcore::RPMaterial mat(gn.get_material(0));
...

// set to 0th geometry
gn.set_material(0, mat);

// remove effect
rpcore::RenderPipeline::get_global_ptr()->clear_effect(np);
```
