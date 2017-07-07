# Render Pipeline Material

## Transparent
To add:
```cpp
NodePath nodepath = ...;

rpcore::RPMaterial mat;
mat.set_shading_model(rpcore::RPMaterial::ShadingModel::TRANSPARENT_MODEL);
mat.set_alpha(0.3f);

nodepath.set_material(mat.get_material());

// add effect
render_pipeline->prepare_scene(nodepath);
```

To remove:
```cpp
NodePath nodepath = ...;

rpcore::RPMaterial mat(nodepath.get_material());

// change material
...

nodepath.set_material(mat.get_material());

// remove effect
rpcore::RenderPipeline::get_global_ptr()->clear_effect(np);
```
