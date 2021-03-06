# Render Pipeline Material
**Translation**: [English](../../rendering/rpmaterial.md)

Render Pipeline 에서 사용하는 Material 은 physically based material 로, Panda3D 에서 보통 사용되는
Material 값과는 다르게 사용된다.

Render Pipeline 에서는 Panda3D 의 Material 데이터를 그대로 사용하되, 일부 값들은 의미를 다르게 해서 사용한다.
따라서 이를 쉽게 사용하기 위해서 `rpcore::RPMaterial` 클래스를 사용할 수 있으며, Render Pipeline 에서 사용하는
의미로 값을 저장 및 불러올 수 있다.

## 파라미터
### Shading Model
Shading Model 은 shading 할 방식을 정하며, 6개 방식을 지원한다.

- Default
- Emissive
- Clearcoat
- Transparent
- Skin
- Foliage

### Base Color
Base Color 는 Material 의 기본 색상을 지정한다. 기본값은 `(0.8, 0.8, 0.8, 1.0)` 이다.
텍스처 사용 시 0번 텍스처(`p3d_Texture0`)에 매핑되며, material 의 기본 색상과 곱해서 적용이 된다.

### Specular IOR
[1.0, 2.51] 값을 가진다. 기본값은 1.51 이다.
텍스처 사용 시 2번 텍스처(`p3d_Texture2`)에 매핑된다.

### Metallic
금속(metal) 같은 효과의 정도를 지정하며, [0, 1] 값을 가진다. 1로 갈수록 효과가 강해진다. 기본값은 0 이다.

### Roughness
거친 정도를 지정하며, [0, 1] 값을 가진다. 0으로 갈수록 매끄럽고, 1로 갈수록 거칠어진다. 기본값은 0.8 이다.
텍스처 사용 시 3번 텍스처(`p3d_Texture3`)에 매핑된다.

### Normal Factor
[0, 1] 값을 가진다. 기본값은 0 이다.

### Alpha
Transparent shading 일 경우에 사용되며, 알파 값을 지정하고 [0, 1] 값을 가진다.
만일, 알파 텍스처 모드로 지정할 경우에는 0번 텍스처(`p3d_Texture0`)의 A 값을 가져와서 사용한다.

알파 텍스처를 사용할 경우에는 Panda3D 에서 RGBA 텍스처로 변환하여 위의 형태로 사용하게 된다.

### 텍스처 적용
```cpp
rpcore::RPRenderState state(MODEL_NODEPATH.get_state());

PT(Texture) t = rpcore::RPLoader::load_texture(BASE_COLOR_PATH);
t->set_format(Texture::Format::F_srgb);         // NOTE: use if the texture is sRGB format.

state.set_basecolor_texture(t)
     .set_normal_texture(rpcore::RPLoader::load_texture(NORMAL_PATH))
     .set_specular_texture(rpcore::RPLoader::load_texture(SPECULAR_PATH))
     .set_roughness_texture(rpcore::RPLoader::load_texture(ROUGHNESS_PATH));

MODEL_NODEPATH.set_state(state.get_state());
```



## Shading Model
### Transparent
#### 코드
Transparent 추가:
```cpp
NodePath nodepath = ...;

rpcore::RPMaterial mat;
mat.set_shading_model(rpcore::RPMaterial::ShadingModel::TRANSPARENT_MODEL);
mat.set_alpha(0.3f);

nodepath.set_material(mat.get_material());

// add effect
render_pipeline->prepare_scene(nodepath);
```

Transparent 제거:
```cpp
NodePath nodepath = ...;

rpcore::RPMaterial mat(nodepath.get_material());

// change material
...

nodepath.set_material(mat.get_material());

// remove effect
rpcore::RenderPipeline::get_global_ptr()->clear_effect(np);
```
