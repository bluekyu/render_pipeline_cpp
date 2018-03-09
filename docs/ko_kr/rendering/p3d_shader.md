# Shader in Panda3D
**Translation**: [English](../../rendering/p3d_shader.md)

## Shader 에서의 Panda3D 좌표계
GLSL 을 기준으로 최종 계산은 OpenGL 좌표계로 변환을 해야 하는 것이 맞다.
다만, 그 전까지는 어떠한 좌표계를 쓰더라도 문제가 없기 때문에,
Panda3D 에서 제공하는 input 값들은 Panda3D 좌표계를 가지게 된다.

예를 들어, p3d_Vertex attribute 나 p3d_ModelViewMatrix uniform 등은 Panda3D 좌표계 형태를 그대로 사용한다.
따라서 vertex 좌표도 Z-up 이며, matrix 도 Z-up 이다.
그리고 p3d_ProjectionMatrix 를 사용하여 projection 할 때 Z-up 에서 OpenGL Y-up 좌표계로 변환이 수행된다.
([Math](math.md) 문서의 Projection Matrix 항목 참조)

```
vec4 p3d_Vertex;                                    // z-up
vec4 world_pos = p3d_ModelMatrix * p3d_Vertex;      // z-up
vec4 eye_pos = p3d_ViewMatrix * world_pos;          // z-up
vec4 ndc_pos = p3d_ProjectionMatrix * eye_pos;      // y-up
```
