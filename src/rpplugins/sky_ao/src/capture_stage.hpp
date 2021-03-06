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

#include <camera.h>
#include <orthographicLens.h>

namespace rpplugins {

/** This stage captures the sky ao by rendering the scene from above. */
class SkyAOCaptureStage : public rpcore::RenderStage
{
public:
    SkyAOCaptureStage(rpcore::RenderPipeline& pipeline);

    RequireType& get_required_inputs() const final { return required_inputs; }
    RequireType& get_required_pipes() const final { return required_pipes; }
    ProduceType get_produced_pipes() const final;
    ProduceType get_produced_inputs() const final;

    RENDER_PIPELINE_STAGE_DOWNCAST();

    void create() final;
    void update() final;
    void reload_shaders() final;

    void set_resolution(int resolution);
    void set_capture_height(float capture_height);
    void set_max_radius(float max_radius);

private:
    std::string get_plugin_id() const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    PTA_LVecBase3f pta_position_;
    int resolution_ = 512;
    float capture_height_ = 100.0f;
    float max_radius_ = 100.0f;

    rpcore::RenderTarget* target_;
    rpcore::RenderTarget* target_convert_;

    PT(Camera) _camera;
    PT(OrthographicLens) _cam_lens;
    NodePath _cam_node;
};

inline void SkyAOCaptureStage::set_resolution(int resolution)
{
    resolution_ = resolution;
}

inline void SkyAOCaptureStage::set_capture_height(float capture_height)
{
    capture_height_ = capture_height;
}

inline void SkyAOCaptureStage::set_max_radius(float max_radius)
{
    max_radius_ = max_radius;
}

}    // namespace rpplugins
