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

#include "../include/volumetrics_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

class VolumetricsStage::Impl
{
public:
    Impl(VolumetricsStage& self);

    void create(void);
    void reload_shaders(void);

public:
    static RequireType required_inputs;
    static RequireType required_pipes;

    VolumetricsStage& self_;

    bool enable_volumetric_shadows_ = false;

    std::shared_ptr<rpcore::RenderTarget> target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_upscale_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_combine_ = nullptr;
};

VolumetricsStage::RequireType VolumetricsStage::Impl::required_inputs;
VolumetricsStage::RequireType VolumetricsStage::Impl::required_pipes = { "ShadedScene", "GBuffer" };

VolumetricsStage::Impl::Impl(VolumetricsStage& self): self_(self)
{
}

void VolumetricsStage::Impl::create(void)
{
    if (enable_volumetric_shadows_)
    {
        target_ = self_.create_target("ComputeVolumetrics");
        target_->set_size(-2);
        target_->add_color_attachment(16, true);
        target_->prepare_buffer();

        target_upscale_ = self_.create_target("Upscale");
        target_upscale_->add_color_attachment(16, true);
        target_upscale_->prepare_buffer();

        target_upscale_->set_shader_input(ShaderInput("SourceTex", target_->get_color_tex()));
        target_upscale_->set_shader_input(ShaderInput("upscaleWeights", LVecBase2f(0.001, 0.001)));
    }

    target_combine_ = self_.create_target("CombineVolumetrics");
    target_combine_->add_color_attachment(16);
    target_combine_->prepare_buffer();

    if (enable_volumetric_shadows_)
        target_combine_->set_shader_input(ShaderInput("VolumetricsTex", target_upscale_->get_color_tex()));
}

void VolumetricsStage::Impl::reload_shaders(void)
{
    if (enable_volumetric_shadows_)
    {
        target_->set_shader(self_.load_plugin_shader({"compute_volumetric_shadows.frag.glsl"}));
        target_upscale_->set_shader(self_.load_plugin_shader({"/$$rp/shader/bilateral_upscale.frag.glsl"}));
    }

    target_combine_->set_shader(self_.load_plugin_shader({"apply_volumetrics.frag.glsl"}));
}

// ************************************************************************************************

VolumetricsStage::VolumetricsStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "VolumetricsStage"), impl_(std::make_unique<Impl>(*this))
{
}

VolumetricsStage::~VolumetricsStage(void) = default;

VolumetricsStage::RequireType& VolumetricsStage::get_required_inputs(void) const
{
    return impl_->required_inputs;
}

VolumetricsStage::RequireType& VolumetricsStage::get_required_pipes(void) const
{
    return impl_->required_pipes;
}

VolumetricsStage::ProduceType VolumetricsStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", impl_->target_combine_->get_color_tex()),
    };
}

void VolumetricsStage::create(void)
{
    impl_->create();
}

void VolumetricsStage::reload_shaders(void)
{
    impl_->reload_shaders();
}

std::string VolumetricsStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

void VolumetricsStage::set_enable_volumetric_shadows(bool enable_volumetric_shadows)
{
    impl_->enable_volumetric_shadows_ = enable_volumetric_shadows;
}

}
