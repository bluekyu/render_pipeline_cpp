/**
 *
 * RenderPipeline C++
 *
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
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

#pragma include "/$$rptemp/$$pipeline_shader_config.inc.glsl"

// NVIDIA Single Pass Stereo
#if NVIDIA_STEREO_VIEW
#extension GL_NV_viewport_array2 : require
#extension GL_NV_stereo_view_rendering : require
layout(secondary_view_offset=1) out highp int gl_Layer;
#endif

in vec4 p3d_Vertex;

void main() {
    gl_Position = vec4(p3d_Vertex.xz, 0, 1);

#if NVIDIA_STEREO_VIEW
    gl_SecondaryPositionNV = gl_Position;
    gl_Layer = 0;
#endif
}
