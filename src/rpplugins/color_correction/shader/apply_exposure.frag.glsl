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

#if STEREO_MODE
uniform sampler2DArray ShadedScene;
#else
uniform sampler2D ShadedScene;
#endif
uniform samplerBuffer Exposure;

out vec3 result;

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    #if STEREO_MODE
        vec3 scene_color = texelFetch(ShadedScene, ivec3(coord, gl_Layer), 0).xyz;
    #else
        vec3 scene_color = texelFetch(ShadedScene, coord, 0).xyz;
    #endif

    #if !DEBUG_MODE
        #if STEREO_MODE
            float exposure = texelFetch(Exposure, gl_Layer).x;
        #else
            float exposure = texelFetch(Exposure, 0).x;
        #endif
        scene_color *= exposure;
    #endif

    result = scene_color;
}
