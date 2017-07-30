/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
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
#include <graphicsOutput.h>

#include <unordered_map>
#include <functional>

class CallbackObject;

namespace rpcore {

class PostProcessRegion
{
public:
    static PostProcessRegion* make(GraphicsOutput* internal_buffer);
    static PostProcessRegion* make(GraphicsOutput* internal_buffer, const LVecBase4f& dimensions);

public:
    PostProcessRegion(GraphicsOutput* internal_buffer);
    PostProcessRegion(GraphicsOutput* internal_buffer, const LVecBase4f& dimensions);

    void set_shader_input(const ShaderInput& inp, bool override_input=false);

    /** DisplayRegion functions with PostProcessRegion::region. */
    ///@{
    std::function<void(int)>    set_sort;
    std::function<void(void)>    disable_clears;
    std::function<void(bool)>    set_active;
    std::function<void(bool)>    set_clear_depth_active;
    std::function<void(PN_stdfloat)>    set_clear_depth;
    std::function<void(const NodePath&)>    set_camera;
    std::function<void(bool)>    set_clear_color_active;
    std::function<void(const LColor&)>    set_clear_color;
    std::function<void(CallbackObject*)> set_draw_callback;
    ///@}

    /** NodePath function with PostProcessRegion::tri. */
    ///@{
    std::function<void(int)>    set_instance_count;
    /** NodePath::set_shader(const Shader* sha, int priority=0) */
    std::function<void(const Shader*,int)> set_shader;
    std::function<void(const RenderAttrib*,int)> set_attrib;
    ///@}

    DisplayRegion* get_region(void) { return region; }
    NodePath get_node(void) const { return node; }

private:
    void init_function_pointers(void);
    void make_fullscreen_tri(void);
    void make_fullscreen_cam(void);

    PT(GraphicsOutput) _buffer;
    PT(DisplayRegion) region;
    NodePath node;
    NodePath tri;
    NodePath camera;
};

inline void PostProcessRegion::set_shader_input(const ShaderInput& inp, bool override_input)
{
    if (override_input)
        node.set_shader_input(inp);
    else
        tri.set_shader_input(inp);
}

}
