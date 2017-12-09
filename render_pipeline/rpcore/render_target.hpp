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

#include <texture.h>

#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/rpobject.hpp>

class ShaderInput;
class GraphicsBuffer;
class DisplayRegion;

namespace rpcore {

class PostProcessRegion;

class RENDER_PIPELINE_DECL RenderTarget : public RPObject
{
public:
    static bool USE_R11G11B10;
    static std::vector<RenderTarget*> REGISTERED_TARGETS;
    static int CURRENT_SORT;

    RenderTarget(const std::string& name);
    RenderTarget(const RenderTarget&) = delete;

    ~RenderTarget();

    RenderTarget& operator=(const RenderTarget&) = delete;

    void add_color_attachment(const LVecBase3i& bits);
    void add_color_attachment(const LVecBase4i& bits);
    void add_color_attachment(int bits=8, bool alpha=false);
    void add_depth_attachment(int bits=32);
    void add_aux_attachment(int bits=8);
    void add_aux_attachments(int bits=8, int count=1);
    void set_layers(int layers);

    /**
     * Sets the render target size. This can be either a single integer,
     * in which case it applies to both dimensions, or two integers.
     * You can also pass a string containing a percentage, e.g. '25%', '50%'
     * or '100%' (the default).
     */
    void set_size(const std::string& width, const std::string& height) NOEXCEPT;
    void set_size(const std::string& size) NOEXCEPT;

    /**
     * Set size_constraint.
     * If width or height has negative value,
     * it will be re-calculated to be proportional to resolution.
     */
    void set_size(const LVecBase2i& size) NOEXCEPT;
    /** @overload set_size(const LVecBase2i&) */
    void set_size(int size) NOEXCEPT;
    /** @overload set_size(const LVecBase2i&) */
    void set_size(int width, int height) NOEXCEPT;

    /** Get current active. */
    bool get_active() const;

    /** Set activity of this target and region. */
    void set_active(bool flag);

    /** Get the map of targets. */
    const std::unordered_map<std::string, PT(Texture)>& get_targets() const;

    /** Get the color texture. */
    Texture* get_color_tex() const;

    /** Get the depth texture. */
    Texture* get_depth_tex() const;

    /** Get the n-th aux textures. */
    Texture* get_aux_tex(size_t index) const;

    /** Sets a shader input available to the target. */
    void set_shader_input(const ShaderInput& inp, bool override_input=false);

    void set_shader(const Shader* sha);

    GraphicsBuffer* get_internal_buffer() const;
    DisplayRegion* get_display_region() const;
    PostProcessRegion* get_postprocess_region() const;

    /** Prepares to render a scene. */
    void prepare_render(const NodePath& camera);

    /** Prepares the target to render to an offscreen buffer. */
    void prepare_buffer();

    /** Prepares the target to render on the main window, to present the final rendered image. */
    void present_on_screen();

    /** Deletes this buffer, restoring the previous state. */
    void remove();

    /** Sets the  clear color. */
    void set_clear_color(const LColor& color);

    /** Sets the instance count. */
    void set_instance_count(int count);

    void consider_resize();

    const boost::optional<int>& get_sort() const NOEXCEPT;
    void set_sort(int sort) NOEXCEPT;

    bool get_support_transparency() const;
    bool get_create_default_region() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// ************************************************************************************************
inline void RenderTarget::add_color_attachment(const LVecBase3i& bits)
{
    add_color_attachment(LVecBase4i(bits, 0));
}

inline void RenderTarget::add_color_attachment(int bits, bool alpha)
{
    add_color_attachment(LVecBase4i(bits, bits, bits, alpha ? bits : 0));
}

inline void RenderTarget::set_size(const std::string& size) NOEXCEPT
{
    set_size(size, size);
}

}
