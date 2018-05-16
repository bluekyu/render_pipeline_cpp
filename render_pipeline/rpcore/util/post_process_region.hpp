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

#include <unordered_map>
#include <functional>

#include <render_pipeline/rpcore/rpobject.hpp>

class GraphicsOutput;
class CallbackObject;
class Geom;

namespace rpcore {

class RENDER_PIPELINE_DECL PostProcessRegion
{
public:
    static std::unique_ptr<PostProcessRegion> make(GraphicsOutput* internal_buffer, bool use_point = false);
    static std::unique_ptr<PostProcessRegion> make(GraphicsOutput* internal_buffer, const LVecBase4f& dimensions, bool use_point = false);

public:
    PostProcessRegion(GraphicsOutput* internal_buffer, bool use_point);
    PostProcessRegion(GraphicsOutput* internal_buffer, const LVecBase4f& dimensions, bool use_point);

    void set_shader_input(const ShaderInput& inp, bool override_input=false);

    /** DisplayRegion functions with PostProcessRegion::region. */
    ///@{
    std::function<void(int)>    set_sort;
    std::function<void()>    disable_clears;
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

    DisplayRegion* get_region() const;
    NodePath get_node() const;

private:
    void init_function_pointers();
    void make_fullscreen_tri();
    void make_single_point();
    void make_geom_node(Geom* geom);
    void make_fullscreen_cam();

    PT(DisplayRegion) region_;
    NodePath node;
    NodePath geom_np_;
    NodePath camera;
};

inline void PostProcessRegion::set_shader_input(const ShaderInput& inp, bool override_input)
{
    if (override_input)
        node.set_shader_input(inp);
    else
        geom_np_.set_shader_input(inp);
}

inline DisplayRegion* PostProcessRegion::get_region() const
{
    return region_;
}

inline NodePath PostProcessRegion::get_node() const
{
    return node;
}

}
