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
#pragma include "includes/color_spaces.inc.glsl"

#if STEREO_MODE
uniform sampler2DArray ShadedScene;
uniform writeonly image2DArray RESTRICT DestTex;
#else
uniform sampler2D ShadedScene;
uniform writeonly image2D RESTRICT DestTex;
#endif

void main() {
    #if STEREO_MODE

    for (gl_Layer = 0; gl_Layer < 2; ++gl_Layer)
    {

    vec3 texcoord = vec3(get_texcoord(), gl_Layer);
    ivec3 coord = ivec3(gl_FragCoord.xy, gl_Layer);

    #else   // STEREO_MODE

    vec2 texcoord = get_texcoord();
    ivec2 coord = ivec2(gl_FragCoord.xy);

    #endif  // STEREO_MODE

    vec3 scene_color = textureLod(ShadedScene, texcoord, 0).xyz;
    float luma = get_luminance(scene_color);

    // We don't use a threshold for blur, instead we perform the bloom everything,
    // which is physically more correct
    vec3 bloom_color = scene_color;
    bloom_color *= GET_SETTING(bloom, bloom_strength) * 0.005;
    bloom_color = clamp(bloom_color, vec3(0), vec3(25000.0));

    #if DEBUG_MODE
        bloom_color *= 0;
    #endif

    imageStore(DestTex, coord, vec4(bloom_color, 0));

    #if STEREO_MODE
    }
    #endif
}
