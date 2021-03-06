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
#pragma include "includes/noise.inc.glsl"
#pragma include "includes/upsampling.inc.glsl"

uniform bool use_cropping;

#if STEREO_MODE
uniform sampler2DArray ShadedScene;
#else
uniform sampler2D ShadedScene;
#endif
out vec4 result;

void main() {
    #if 1
        vec2 texcoord = (ivec2(gl_FragCoord.xy) + 0.5) / NATIVE_SCREEN_SIZE;
        // vec4 scene_color = bicubic_filter(ShadedScene, texcoord);
        // vec4 scene_color = bilinear_filter(ShadedScene, texcoord);

        vec2 screen_ratio = NATIVE_SCREEN_SIZE / SCREEN_SIZE;
        if (use_cropping)
        {
            screen_ratio /= max(screen_ratio.x, screen_ratio.y);
            texcoord = texcoord * screen_ratio - (screen_ratio - 1.0f) / 2.0f;
        }
        else
        {
            screen_ratio /= min(screen_ratio.x, screen_ratio.y);
            texcoord = texcoord * screen_ratio - (screen_ratio - 1.0f) / 2.0f;
            if (min(texcoord.x, texcoord.y) < 0.0f || max(texcoord.x, texcoord.y) > 1.0f)
                return;
        }

        #if STEREO_MODE
            vec4 scene_color = directional_filter(ShadedScene, texcoord, gl_Layer);
        #else
            vec4 scene_color = directional_filter(ShadedScene, texcoord);
        #endif
    #else
        vec2 texcoord = (ivec2(gl_FragCoord.xy) + 0.5) / NATIVE_SCREEN_SIZE;
        vec4 scene_color = textureLod(ShadedScene, texcoord, 0);
    #endif

    result = vec4(scene_color.xyz, 1);
}
