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

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

class AOStage: public rpcore::RenderStage
{
public:
    AOStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "AOStage") {}

    RequireType& get_required_inputs(void) const final;
    RequireType& get_required_pipes(void) const final;
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

    void set_quality(const std::string& quality);

private:
    virtual std::string get_plugin_id(void) const;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;
    bool enable_small_scale_ao_;

    std::string quality_;

    std::shared_ptr<rpcore::RenderTarget> target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_upscale_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_detail_ao_ = nullptr;
    std::vector<std::shared_ptr<rpcore::RenderTarget>> blur_targets_;
    std::shared_ptr<rpcore::RenderTarget> target_resolve_ = nullptr;
};

inline AOStage::RequireType& AOStage::get_required_inputs(void) const
{
    return required_inputs;
}

inline AOStage::RequireType& AOStage::get_required_pipes(void) const
{
    return required_pipes;
}

inline void AOStage::set_quality(const std::string& quality)
{
    quality_ = quality;
}

}
