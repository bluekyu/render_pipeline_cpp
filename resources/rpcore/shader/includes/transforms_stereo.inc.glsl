/**
 *
 * RenderPipeline
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#pragma once

#if !STEREO_MODE
#error USE "transforms.inc.glsl" file.
#endif

// Computes linear Z from a given Z value, and near and far plane
float get_linear_z_from_z(float z, float near, float far) {
    return 2.0 * near * far / (far + near - (z * 2.0 - 1) * (far - near));
}

// Computes linear Z from a given Z value
float get_linear_z_from_z(float z) {
    return get_linear_z_from_z(z, CAMERA_NEAR, CAMERA_FAR);
}

// Computes the Z component from a position in NDC space
float get_z_from_ndc(vec3 ndc_pos) {
    return get_linear_z_from_z(ndc_pos.z);
}

// Computes linear Z from a given Z value, and near and far plane, for orthographic projections
float get_linear_z_from_z_ortographic(float z, float near, float far) {
    return 2.0 / (far + near - fma(z, 2.0, -1.0) * (far - near));
}

// Computes the surface position based on a given Z, a texcoord, and the Inverse MVP matrix
vec3 calculate_surface_pos(float z, vec2 tcoord, mat4 inverse_mvp) {
    vec3 ndc_pos = fma(vec3(tcoord.xy, z), vec3(2.0), vec3(-1.0));
    float clip_w = get_z_from_ndc(ndc_pos);

    vec4 proj = inverse_mvp * vec4(ndc_pos * clip_w, clip_w);
    return proj.xyz / proj.w;
}

// Computes the surface position based on a given Z and a texcoord
vec3 calculate_surface_pos(float z, vec2 tcoord, int view_index) {
    return calculate_surface_pos(z, tcoord, MainSceneData.stereo_ViewProjectionMatrixInverse[view_index]);
}

// Computes the surface position based on a given Z and a texcoord, aswell as a
// custom near and far plane, and the inverse MVP. This is for orthographic projections
vec3 calculate_surface_pos_ortho(float z, vec2 tcoord, float near, float far, mat4 inverse_mvp) {
    vec3 ndc_pos = fma(vec3(tcoord.xy, z), vec3(2.0), vec3(-1.0));
    float clip_w = get_linear_z_from_z_ortographic(z, near, far);
    vec4 result = inverse_mvp * vec4(ndc_pos * clip_w, clip_w);
    return result.xyz / result.w;
}

// Computes the view position from a given Z value and texcoord
vec3 calculate_view_pos(float z, vec2 tcoord, int view_index) {
    vec4 view_pos = MainSceneData.stereo_inv_proj_mat[view_index] *
        vec4(fma(tcoord.xy, vec2(2.0), vec2(-1.0)), z, 1.0);
    return view_pos.xyz / view_pos.w;
}

// Computes the NDC position from a given view position
vec3 view_to_screen(vec3 view_pos, int view_index) {
    vec4 projected = MainSceneData.stereo_proj_mat[view_index] * vec4(view_pos, 1);
    projected.xyz /= projected.w;
    projected.xy = fma(projected.xy, vec2(0.5), vec2(0.5));
    return projected.xyz;
}

// Converts a world space position to screen space position (NDC)
vec3 world_to_screen(vec3 world_pos, int view_index) {
    vec4 proj = MainSceneData.stereo_ViewProjectionMatrix[view_index] * vec4(world_pos, 1);
    proj.xyz /= proj.w;
    proj.xyz = fma(proj.xyz, vec3(0.5), vec3(0.5));
    return proj.xyz;
}

// Converts a world space normal to view space
vec3 world_normal_to_view(vec3 world_normal, int view_index) {
    return normalize(MainSceneData.stereo_view_mat_z_up[view_index] * vec4(world_normal, 0)).xyz;
}
