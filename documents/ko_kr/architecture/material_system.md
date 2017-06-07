# Material System  {#ko_kr_material_system}
Render Pipeline 에서는 physically based material 을 사용하고 있다. 이는 Panda3D 에서 기본적으로 사용하는 material 과 약간
다르기 때문에, 모델링 시에 physically based material 을 가지는 모델로 변경을 해야 한다.

## Modeling
Blender 의 플러그인(Panda3D Bam Exporter)을 사용하여 모델에 physically based material 을 추가할 수 있다.
Panda3D Bam Exporter (https://github.com/tobspr/Panda3D-Bam-Exporter) 는 Blender 에서 모델을 bam 파일로 export 할 수 있게
해주며, physically based material 속성을 반영할 수 있도록 한다. 또한, 기존 YABEE 플러그인에서 egg 파일을
export 할 때에도 physically based material 을 반영할 수 있도록 해준다.

## Property
Physically based material 를 사용하기 위해 아래 Panda3D 의 material 속성들을 이용한다.
* Base color: `baseColor`
* Specular IOR: `refractiveIndex`
* Metallic: `metallic`
* Roughness: `roughness`
* Shading Model: `emission.x`
* Normal factor: `emission.y`
* Arbitrary0: `emission.z`
