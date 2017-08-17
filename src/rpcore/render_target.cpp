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

#include "render_pipeline/rpcore/render_target.hpp"

#include <graphicsWindow.h>
#include <graphicsEngine.h>
#include <graphicsBuffer.h>
#include <colorWriteAttrib.h>
#include <auxBitplaneAttrib.h>
#include <transparencyAttrib.h>

#include <spdlog/fmt/fmt.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include "rpcore/util/post_process_region.hpp"

namespace rpcore {

inline static int max_color_bits(const LVecBase4i& color_bits)
{
    return (std::max)(
        (std::max)(color_bits[0], color_bits[1]),
        (std::max)(color_bits[2], color_bits[3]));
}

// ************************************************************************************************
class RenderTarget::Impl
{
public:
    Impl(RenderTarget& self);

    void initilize();

    void set_active(bool flag);
    void prepare_render(const NodePath& camera_np);
    void remove();

    int percent_to_number(const std::string& v) const NOEXCEPT;

    void create_buffer();
    void compute_size_from_constraint();
    void setup_textures();
    void make_properties(WindowProperties& window_props, FrameBufferProperties& buffer_props);
    bool create();

public:
    RenderTarget& self_;

    PT(GraphicsBuffer) internal_buffer_ = nullptr;
    GraphicsWindow* source_window_ = nullptr;

    PT(DisplayRegion) source_display_region_ = nullptr;
    PostProcessRegion* source_postprocess_region_ = nullptr;

    GraphicsEngine* engine_ = nullptr;

    bool active_ = false;
    bool support_transparency_ = false;
    bool create_default_region_ = true;

    boost::optional<int> sort_;
    std::unordered_map<std::string, PT(Texture)> targets_;
    LVecBase4i color_bits_ = LVecBase4i(0, 0, 0, 0);
    int aux_bits_ = 8;
    int aux_count_ = 0;
    int depth_bits_ = 0;
    int layers_ = 1;
    LVecBase2i size_ = LVecBase2i(-1);
    LVecBase2i size_constraint_ = LVecBase2i(-1);
};

RenderTarget::Impl::Impl(RenderTarget& self): self_(self)
{
}

void RenderTarget::Impl::initilize()
{
    source_window_ = Globals::base->get_win();

    // Public attributes
    engine_ = Globals::base->get_graphics_engine();

    // Disable all global clears, since they are not required
    const int num_display_region = source_window_->get_num_display_regions();
    for (int k = 0; k < num_display_region; k++)
        source_window_->get_display_region(k)->disable_clears();
}

void RenderTarget::Impl::set_active(bool flag)
{
    const int num_display_regions = internal_buffer_->get_num_display_regions();
    for (int k = 0; k < num_display_regions; k++)
        internal_buffer_->get_display_region(k)->set_active(flag);
    active_ = flag;
}

void RenderTarget::Impl::prepare_render(const NodePath& camera_np)
{
    create_default_region_ = false;
    create_buffer();
    source_display_region_ = internal_buffer_->get_display_region(0);
    if (source_postprocess_region_)
    {
        delete source_postprocess_region_;
        source_postprocess_region_ = nullptr;
    }

    if (!camera_np.is_empty())
    {
        Camera* camera = DCAST(Camera, camera_np.node());

        NodePath initial_state("rtis");
        initial_state.set_state(camera->get_initial_state());

        if (aux_count_)
            initial_state.set_attrib(AuxBitplaneAttrib::make(aux_bits_), 20);

        initial_state.set_attrib(TransparencyAttrib::make(TransparencyAttrib::M_none), 20);

        if (max_color_bits(color_bits_) == 0)
            initial_state.set_attrib(ColorWriteAttrib::make(ColorWriteAttrib::C_off), 20);

        // Disable existing regions of the camera
        for (int k = 0, k_end = camera->get_num_display_regions(); k < k_end; ++k)
            DCAST(DisplayRegion, camera->get_display_region(k))->set_active(false);

        // Remove the existing display region of the camera
        for (int k = 0, k_end = source_window_->get_num_display_regions(); k < k_end; ++k)
        {
            DisplayRegion* region = source_window_->get_display_region(k);
            if (region && region->get_camera() == camera_np)
                source_window_->remove_display_region(region);
        }

        camera->set_initial_state(initial_state.get_state());
        source_display_region_->set_camera(camera_np);
    }

    internal_buffer_->disable_clears();
    source_display_region_->disable_clears();
    source_display_region_->set_active(true);
    source_display_region_->set_sort(20);

    // Reenable depth-clear, usually desireable
    source_display_region_->set_clear_depth_active(true);
    source_display_region_->set_clear_depth(1.0f);
    active_ = true;
}

void RenderTarget::Impl::remove()
{
    if (internal_buffer_)
    {
        internal_buffer_->clear_render_textures();
        engine_->remove_window(internal_buffer_);

        RenderTarget::REGISTERED_TARGETS.erase(std::find(RenderTarget::REGISTERED_TARGETS.begin(), RenderTarget::REGISTERED_TARGETS.end(), &self_));
        internal_buffer_.clear();
    }
    else
    {
        delete source_postprocess_region_;
        source_postprocess_region_ = nullptr;
    }

    active_ = false;
    for (auto& target: targets_)
        target.second->release_all();
    targets_.clear();
}

int RenderTarget::Impl::percent_to_number(const std::string& v) const NOEXCEPT
{
    static const std::unordered_map<std::string, int> percent_to_number_map = {
        { "100%", -1 },
        { "50%", -2 },
        { "25%", -4 }
    };

    try
    {
        return percent_to_number_map.at(v);
    }
    catch (...)
    {
        self_.error(fmt::format("Invalid percent: {}", v));
        return -1;
    }
}

void RenderTarget::Impl::create_buffer()
{
    compute_size_from_constraint();
    if (!create())
    {
        self_.error("Failed to create buffer!");
        return;
    }

    if (create_default_region_)
    {
        source_postprocess_region_ = PostProcessRegion::make(internal_buffer_);
        source_display_region_.clear();

        if (max_color_bits(color_bits_) == 0)
            source_postprocess_region_->set_attrib(ColorWriteAttrib::make(ColorWriteAttrib::M_none), 1000);
    }
}

void RenderTarget::Impl::compute_size_from_constraint()
{
    const int w = rpcore::Globals::resolution.get_x();
    const int h = rpcore::Globals::resolution.get_y();
    size_ = size_constraint_;

    const int size_constraint_x = size_constraint_.get_x();
    const int size_constraint_y = size_constraint_.get_y();
    if (size_constraint_x < 0)
        size_.set_x((w - size_constraint_x - 1) / (-size_constraint_x));
    if (size_constraint_y < 0)
        size_.set_y((h - size_constraint_y - 1) / (-size_constraint_y));
}

void RenderTarget::Impl::setup_textures()
{
    for (int k = 0; k < aux_count_; k++)
    {
        targets_[std::string("aux_") + std::to_string(k)] = new Texture(self_.get_debug_name() + "_aux_" + std::to_string(k));
    }

    for (auto& tex: targets_)
    {
        if (layers_ > 1)
            tex.second->setup_2d_texture_array(layers_);

        tex.second->set_wrap_u(SamplerState::WM_clamp);
        tex.second->set_wrap_v(SamplerState::WM_clamp);
        tex.second->set_anisotropic_degree(0);
        tex.second->set_x_size(size_.get_x());
        tex.second->set_y_size(size_.get_y());
        tex.second->set_minfilter(SamplerState::FT_linear);
        tex.second->set_magfilter(SamplerState::FT_linear);
    }
}

void RenderTarget::Impl::make_properties(WindowProperties& window_props, FrameBufferProperties& buffer_props)
{
    window_props = WindowProperties::size(size_.get_x(), size_.get_y());

    if (size_constraint_.get_x() == 0 || size_constraint_.get_y() == 0)
        window_props = WindowProperties::size(1, 1);

    const int color_bits_r = color_bits_.get_x();
    const int color_bits_g = color_bits_.get_y();
    const int color_bits_b = color_bits_.get_z();
    const int color_bits_a = color_bits_.get_w();
    if (color_bits_ == LVecBase4i(16, 16, 16, 0))
    {
        if (RenderTarget::USE_R11G11B10)
            buffer_props.set_rgba_bits(11, 11, 10, 0);
        else
            buffer_props.set_rgba_bits(color_bits_r, color_bits_g, color_bits_b, color_bits_a);
    }
    else if (color_bits_r == 8 || color_bits_g == 8 || color_bits_b == 8 || color_bits_a == 8)
    {
        // When specifying 8 bits, specify 1 bit, this is a workarround
        // to a legacy logic in panda
        buffer_props.set_rgba_bits(
            color_bits_r != 8 ? color_bits_r : 1,
            color_bits_g != 8 ? color_bits_g : 1,
            color_bits_b != 8 ? color_bits_b : 1,
            color_bits_a != 8 ? color_bits_a : 1);
    }
    else
    {
        buffer_props.set_rgba_bits(color_bits_r, color_bits_g, color_bits_b, color_bits_a);
    }

    buffer_props.set_accum_bits(0);
    buffer_props.set_stencil_bits(0);
    buffer_props.set_back_buffers(0);
    buffer_props.set_coverage_samples(0);
    buffer_props.set_depth_bits(depth_bits_);

    if (depth_bits_ == 32)
        buffer_props.set_float_depth(true);

    buffer_props.set_float_color(max_color_bits(color_bits_) > 8);

    buffer_props.set_force_hardware(true);
    buffer_props.set_multisamples(0);
    buffer_props.set_srgb_color(false);
    buffer_props.set_stereo(false);
    buffer_props.set_stencil_bits(0);

    if (aux_bits_ == 8)
        buffer_props.set_aux_rgba(aux_count_);
    else if (aux_bits_ == 16)
        buffer_props.set_aux_hrgba(aux_count_);
    else if (aux_bits_ == 32)
        buffer_props.set_aux_float(aux_count_);
    else
        self_.error("Invalid aux bits");
}

bool RenderTarget::Impl::create()
{
    setup_textures();
    WindowProperties window_props;
    FrameBufferProperties buffer_props;
    make_properties(window_props, buffer_props);

    internal_buffer_ = DCAST(GraphicsBuffer, engine_->make_output(
        source_window_->get_pipe(),
        self_.get_debug_name(),
        1,
        buffer_props,
        window_props,
        GraphicsPipe::BF_refuse_window | GraphicsPipe::BF_resizeable,
        source_window_->get_gsg(),
        source_window_));

    if (internal_buffer_ == nullptr)
    {
        self_.error("Failed to create buffer.");
        return false;
    }

    GraphicsOutput::RenderTextureMode rtmode = GraphicsOutput::RTM_bind_or_copy;
    if (layers_ > 1)
        rtmode = GraphicsOutput::RTM_bind_layered;

    if (depth_bits_)
    {
        internal_buffer_->add_render_texture(self_.get_depth_tex(), rtmode,
            GraphicsOutput::RTP_depth);
    }

    if (max_color_bits(color_bits_) > 0)
    {
        internal_buffer_->add_render_texture(self_.get_color_tex(), rtmode,
            GraphicsOutput::RTP_color);
    }

    int aux_prefix =
        aux_bits_ == 8 ? int(GraphicsOutput::RTP_aux_rgba_0) : (
        aux_bits_ == 16 ? int(GraphicsOutput::RTP_aux_hrgba_0) : int(GraphicsOutput::RTP_aux_float_0));

    for (int k = 0; k < aux_count_; k++)
    {
        int target_mode = aux_prefix + k;
        internal_buffer_->add_render_texture(self_.get_aux_tex(k), rtmode,
            DrawableRegion::RenderTexturePlane(target_mode));
    }

    if (!sort_.is_initialized())
    {
        RenderTarget::CURRENT_SORT += 20;
        sort_ = RenderTarget::CURRENT_SORT;
    }

    internal_buffer_->set_sort(sort_.get());
    internal_buffer_->disable_clears();
    internal_buffer_->get_display_region(0)->disable_clears();
    internal_buffer_->get_overlay_display_region()->disable_clears();
    internal_buffer_->get_overlay_display_region()->set_active(false);

    RenderTarget::REGISTERED_TARGETS.push_back(&self_);

    return true;
}

// ************************************************************************************************
bool RenderTarget::USE_R11G11B10 = true;
std::vector<RenderTarget*> RenderTarget::REGISTERED_TARGETS;
int RenderTarget::CURRENT_SORT = -300;

RenderTarget::RenderTarget(const std::string& name): RPObject(name), impl_(std::make_unique<Impl>(*this))
{
    impl_->initilize();
}

RenderTarget::~RenderTarget()
{
    remove();
}

void RenderTarget::set_active(bool flag) { impl_->set_active(flag); }
void RenderTarget::remove() { impl_->remove(); }
void RenderTarget::prepare_render(const NodePath& camera_np) { impl_->prepare_render(camera_np); }

void RenderTarget::add_color_attachment(const LVecBase4i& bits)
{
    impl_->targets_["color"] = new Texture(get_debug_name() + "_color");
    impl_->color_bits_ = bits;
}

void RenderTarget::add_depth_attachment(int bits)
{
    impl_->targets_["depth"] = new Texture(get_debug_name() + "_depth");
    impl_->depth_bits_ = bits;
}

void RenderTarget::add_aux_attachment(int bits)
{
    impl_->aux_bits_ = bits;
    ++impl_->aux_count_;

    // RTP_aux_rgba_0 ~ RTP_aux_rgba_3
    if (impl_->aux_count_ > 4)
    {
        warn(get_debug_name() + ": The maximum number of AUX textures is 4!");
        impl_->aux_count_ = (std::min)(impl_->aux_count_, 4);
    }
}

void RenderTarget::add_aux_attachments(int bits, int count)
{
    impl_->aux_bits_ = bits;
    impl_->aux_count_ += count;

    // RTP_aux_rgba_0 ~ RTP_aux_rgba_3
    if (impl_->aux_count_ > 4)
    {
        warn(get_debug_name() + ": The maximum number of AUX textures is 4!");
        impl_->aux_count_ = (std::min)(impl_->aux_count_, 4);
    }
}

void RenderTarget::set_layers(int layers)
{
    impl_->layers_ = layers;
}

void RenderTarget::set_size(int width, int height) NOEXCEPT
{
    impl_->size_constraint_ = LVecBase2i(width, height);
}

void RenderTarget::set_size(int size) NOEXCEPT
{
    impl_->size_constraint_ = LVecBase2i(size);
}

void RenderTarget::set_size(const LVecBase2i& size) NOEXCEPT
{
    impl_->size_constraint_ = size;
}

const decltype(RenderTarget::Impl::targets_)& RenderTarget::get_targets() const
{
    return impl_->targets_;
}

Texture* RenderTarget::get_color_tex() const
{
    return impl_->targets_.at("color");
}

Texture* RenderTarget::get_depth_tex() const
{
    return impl_->targets_.at("depth");
}

Texture* RenderTarget::get_aux_tex(size_t index) const
{
    return impl_->targets_.at(std::string("aux_") + std::to_string(index));
}

const boost::optional<int>& RenderTarget::get_sort() const NOEXCEPT
{
    return impl_->sort_;
}

void RenderTarget::set_sort(int sort) NOEXCEPT
{
    impl_->sort_ = sort;
}

void RenderTarget::set_size(const std::string& width, const std::string& height) NOEXCEPT
{
    impl_->size_constraint_ = LVecBase2i(impl_->percent_to_number(width), impl_->percent_to_number(height));
}

void RenderTarget::prepare_buffer()
{
    impl_->create_buffer();
    impl_->active_ = true;
}

void RenderTarget::present_on_screen()
{
    impl_->source_postprocess_region_ = PostProcessRegion::make(impl_->source_window_);
    impl_->source_postprocess_region_->set_sort(5);
}

void RenderTarget::set_clear_color(const LColor& color)
{
    impl_->internal_buffer_->set_clear_color_active(true);
    impl_->internal_buffer_->set_clear_color(color);
}

void RenderTarget::set_instance_count(int count)
{
    impl_->source_postprocess_region_->set_instance_count(count);
}

bool RenderTarget::get_active() const
{
    return impl_->active_;
}

void RenderTarget::set_shader_input(const ShaderInput& inp, bool override_input)
{
    if (impl_->create_default_region_)
        impl_->source_postprocess_region_->set_shader_input(inp, override_input);
}

void RenderTarget::set_shader(const Shader* sha)
{
    if (!sha)
    {
        error("shader must not be nullptr!");
        return;
    }
    impl_->source_postprocess_region_->set_shader(sha, 0);
}

GraphicsBuffer* RenderTarget::get_internal_buffer() const
{
    return impl_->internal_buffer_;
}

DisplayRegion* RenderTarget::get_display_region() const
{
    return impl_->source_display_region_;
}

PostProcessRegion* RenderTarget::get_postprocess_region() const
{
    return impl_->source_postprocess_region_;
}

void RenderTarget::consider_resize()
{
    const LVecBase2i current_size = impl_->size_;
    impl_->compute_size_from_constraint();
    if (current_size != impl_->size_)
    {
        if (impl_->internal_buffer_)
            impl_->internal_buffer_->set_size(impl_->size_.get_x(), impl_->size_.get_y());
    }
}

bool RenderTarget::get_support_transparency() const
{
    return impl_->support_transparency_;
}

bool RenderTarget::get_create_default_region() const
{
    return impl_->create_default_region_;
}

}
