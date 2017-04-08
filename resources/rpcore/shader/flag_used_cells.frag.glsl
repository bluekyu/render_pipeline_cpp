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

#version 430

#pragma include "render_pipeline_base.inc.glsl"
#pragma include "includes/light_culling.inc.glsl"
#pragma include "includes/transforms.inc.glsl"

#define USE_GBUFFER_EXTENSIONS 1
#pragma include "includes/gbuffer.inc.glsl"

uniform writeonly image2DArray RESTRICT cellGridFlags;

void main() {
    vec2 texcoord = get_texcoord();

#if STEREO_MODE

    // Get the distance to the camera
    float surf_dist_L = distance(MainSceneData.stereo_camera_pos[0], get_world_pos_at(texcoord, 0));
    float surf_dist_R = distance(MainSceneData.stereo_camera_pos[1], get_world_pos_at(texcoord, 1));

    // Find the affected cell
    ivec3 tile_L = get_lc_cell_index(ivec2(gl_FragCoord.xy), surf_dist_L);
    ivec3 tile_R = get_lc_cell_index(ivec2(gl_FragCoord.xy), surf_dist_R);

    // Mark the cell as used
    imageStore(cellGridFlags, tile_L, vec4(1));
    if (tile_L != tile_R)
        imageStore(cellGridFlags, tile_R, vec4(1));

#else   // #if STEREO_MODE

    // Get the distance to the camera
    vec3 surf_pos = get_world_pos_at(texcoord);
    float surf_dist = distance(MainSceneData.camera_pos, surf_pos);

    // Find the affected cell
    ivec3 tile = get_lc_cell_index(ivec2(gl_FragCoord.xy), surf_dist);

    // Mark the cell as used
    imageStore(cellGridFlags, tile, vec4(1));

#endif
}
