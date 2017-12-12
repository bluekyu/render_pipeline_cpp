# Texture
**Translation**: [English](../../rendering/texture.md)

## Array 로부터 생성 방법
- 생성
```cpp
NodePath np = ...;

PT(Texture) tex = Texture::make_texture();

// NOTE: this texture has sRGB and 1-byte per component.
tex->setup_2d_texture(WIDTH, HEIGHT, Texture::ComponentType::T_unsigned_byte, Texture::Format::F_srgb);

PTA_uchar ram_image = tex->make_ram_image();
// NOTE: be careful the size
std::memcpy(ram_image.p(), COLOR_DATA_PTR, BYTE_SIZE);

np.set_texture(tex);
```

## 수정 방법
```cpp
PT(Texture) tex = ...

PTA_uchar ram_image = tex->modify_ram_image();

// NOTE: be careful the size
std::memcpy(ram_image.p(), COLOR_DATA_PTR, BYTE_SIZE);
```

## 주의 사항
텍스처에 유형에 따라서 텍스처 포맷을 선택해주어야 한다. Normal 텍스처와 같은 경우에는 `F_rgba8` 과 같이 Linear 타입을
사용해야 하며, diffuse 텍스처에서는 sRGB 일 경우 `F_srgb` 혹은 `F_srgb_alpha` 포맷을 사용해야 하고, HDR 이미지 같이
linear 색상일 경우에는 Linear 타입을 사용해야 한다.
만일, color 텍스처에서 sRGB 색상에 linear 타입을 사용하게 되면, 색상이 희뿌옇게 보일 수 있다.

더 자세한 내용은 https://www.panda3d.org/manual/index.php/Color_Spaces 을 참조.
