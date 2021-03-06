/**
 * Render Pipeline C++
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/effect.hpp>

namespace rpcore {

class RenderPipeline;

class RENDER_PIPELINE_DECL LineNode
{
public:
    static const Effect::SourceType vertex_color_line_effect_source;
    static const Effect::SourceType line_effect_source;

public:
    LineNode(NodePath np);

    /**
     * Set vertex-color line effect.
     *
     * This function sets `vcolor_line.yaml` effect to given NodePath.
     * This effect enable Panda3D line geometry with color like LineSegs to work.
     * However, you should set Render Pipeline material to the node.
     */
    void set_vertex_color_line_effect(RenderPipeline& pipeline);

    /**
     * Set line effect.
     *
     * This function sets `line.yaml` effect to given NodePath.
     * However, Panda3D line geometry does not work in only stereo mode.
     * Therefore, in non-stereo mode, this does not apply the effect actually.
     */
    void set_line_effect(RenderPipeline& pipeline);

private:
    NodePath np_;
};

}
