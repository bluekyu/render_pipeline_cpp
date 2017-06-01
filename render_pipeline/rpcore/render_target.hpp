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

class RENDER_PIPELINE_DECL RenderTarget: public RPObject
{
public:
    static bool USE_R11G11B10;
    static std::vector<RenderTarget*> REGISTERED_TARGETS;
    static int CURRENT_SORT;

    RenderTarget(const std::string& name);
    ~RenderTarget(void);

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
    bool get_active(void) const;

    /** Set activity of this target and region. */
    void set_active(bool flag);

    /** Get the map of targets. */
    const std::unordered_map<std::string, PT(Texture)>& get_targets(void) const;

    /** Get the color texture. */
    Texture* get_color_tex(void) const;

    /** Get the depth texture. */
    Texture* get_depth_tex(void) const;

    /** Get the n-th aux textures. */
    Texture* get_aux_tex(size_t index) const;

    /** Sets a shader input available to the target. */
    void set_shader_input(const ShaderInput& inp, bool override_input=false);

    void set_shader(const Shader* sha);

    GraphicsBuffer* get_internal_buffer(void) const;
    DisplayRegion* get_display_region(void) const;
    PostProcessRegion* get_postprocess_region(void) const;

    /** Prepares to render a scene. */
    void prepare_render(const NodePath& camera);

    /** Prepares the target to render to an offscreen buffer. */
    void prepare_buffer(void);

    /** Prepares the target to render on the main window, to present the final rendered image. */
    void present_on_screen(void);

    /** Deletes this buffer, restoring the previous state. */
    void remove(void);

    /** Sets the  clear color. */
    void set_clear_color(const LColor& color);

    /** Sets the instance count. */
    void set_instance_count(int count);

    void consider_resize(void);

    const boost::optional<int>& get_sort(void) const NOEXCEPT;
    void set_sort(int sort) NOEXCEPT;

    bool get_support_transparency(void) const;
    bool get_create_default_region(void) const;

private:
    struct Impl;
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

}
