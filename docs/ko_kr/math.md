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

자세한 설명은 다음을 참고.
- https://www.panda3d.org/manual/index.php/Matrix_Representation

## Quaternion
`LQuaternion(real, i, j, k)` 를 사용함.



## Projection Matrix
Panda3D 는 Z-up 좌표계이고, OpenGL 을 사용하면 Y-up 로 변환해야 한다.
이를 위해서 Model 및 View 좌표계까지는 Z-up 을 사용하며, projection 시에 Z-up 에서 Y-up 으로 변환이 수행된다.
(즉, projection matrix 에 Z-up 에서 Y-up 으로 변환하는 matrix 요소가 존재한다.)

```
v               // z-up
v' = v * M      // z-up
v'' = v' * V    // z-up
v''' = v'' * P  // y-up
```

따라서 Panda3D projection matrix (Z-up) 를 읽으면, 일반적인 OpenGL projection matrix (Y-up) 값과 다른 형태를 가진다.
Y-up 좌표계의 projection matrix 를 구하고 싶다면 아래 코드를 적용한다.

```cpp
// Panda3D to OpenGL for projection matrix
LMatrix4f::convert_mat(CS_yup_right, CS_zup_right) * Lens::get_projection_mat()
```

반대로, OpenGL Perspective Projection Matrix 를 계산하여, 이를 Panda3D 로 적용하고 싶다면 아래 코드를 적용한다.

```cpp
// y-up
LMatrix4f opengl_perspective(
    cotan_fovy / aspect_ratio, 0, 0, 0,
    0, cotan_fovy, 0, 0,
    0, 0, (near + far) / (near - far), -1,
    0, 0, 2.0 * near * far / (near - far), 0
);

// z-up
panda3d_projection = LMatrix4::convert_mat(CS_zup_right, CS_yup_right) * opengl_perspective;
```

추가적으로, Panda3D 의 Lens 를 사용해서 perspective projection 을 만들려면 아래와 같은 코드를 적용한다.
(이는 위의 projection matrix 와 동일한 값을 가진다.)

```cpp
PerspectiveLens* lens = new PerspectiveLens;
lens->set_near_far(near, far);
lens->set_film_size(width, height);
lens->set_aspect_ratio(aspect_ratio);
lens->set_fov(fovx);

// z-up
panda3d_projection = lens->get_projection_mat();
```
