# Panda3D Math
**Translation**: [English](../math.md)

## Matrix Representation
- Memory layout: Row major (by C++)
- Representation: Column major
- Vector-Matrix Operation: Row Vector

예를 들면, translation 은 4행(3번 인덱스)에 x, y, z 위치 값이 들어간다.
```cpp
LMatrix4f translate_mat = LMatrix4f::translate_mat(x, y, z);
LVecBase3f translation = translate_mat.get_row3(3);
```

Matrix 곱셈 순서는 v*S*R*T 이고, Quaternion 곱 순서는
Q(R1 * R2 * R3) = Q(R1) * Q(R2) * Q(R3) 이다.

For details, refer to the following:
- https://www.panda3d.org/manual/index.php/Matrix_Representation

## Quaternion
`LQuaternion(real, i, j, k)` is used.

## Projection Matrix
```cpp
LMatrix4f::convert_mat(CS_yup_right, CS_zup_right) * Lens::get_projection_mat()
```
