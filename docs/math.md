# Panda3D Math
**Translation**: [한국어](ko_kr/math.md)

## Matrix Representation
- Memory layout: Row major (by C++)
- Representation: Column major
- Vector-Matrix Operation: Row Vector

For example, translation has x, y and z value on 4th (3-index) row.
```cpp
LMatrix4f translate_mat = LMatrix4f::translate_mat(x, y, z);
LVecBase3f translation = translate_mat.get_row3(3);
```

The order of matrix multiplication is v*S*R*T and the order of quaternion multiplication is
Q(R1 * R2 * R3) = Q(R1) * Q(R2) * Q(R3)

For details, refer to the following:
- https://www.panda3d.org/manual/index.php/Matrix_Representation

## Quaternion
`LQuaternion(real, i, j, k)` is used.

## Projection Matrix
```cpp
LMatrix4f::convert_mat(CS_yup_right, CS_zup_right) * Lens::get_projection_mat()
```
