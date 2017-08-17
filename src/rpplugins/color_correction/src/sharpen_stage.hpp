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

class SharpenStage: public rpcore::RenderStage
{
public:
    SharpenStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "SharpenStage") {}

    RequireType& get_required_inputs() const final { return required_inputs; }
    RequireType& get_required_pipes() const final { return required_pipes; }
    ProduceType get_produced_pipes() const final;

    void create() final;
    void reload_shaders() final;

    bool get_sharpen_twice() const { return _sharpen_twice; }
    void set_sharpen_twice(bool sharpen_twice) { _sharpen_twice = sharpen_twice; }

private:
    std::string get_plugin_id() const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;

    std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target2 = nullptr;

    bool _sharpen_twice = true;
};

}    // namespace rpplugins
