# Texture
**Translation**: [한국어](../ko_kr/rendering/texture.md)

## Creation from Array
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

## Modification
```cpp
PT(Texture) tex = ...

PTA_uchar ram_image = tex->modify_ram_image();

// NOTE: be careful the size
std::memcpy(ram_image.p(), COLOR_DATA_PTR, BYTE_SIZE);
```

## Caution
