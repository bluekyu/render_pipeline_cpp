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

#include <memory>
#include <string>

#include <render_pipeline/rpcore/rpobject.hpp>

class Texture;
class Shader;

namespace rpcore {

class RenderStage;

/**
 * Class for creating bilateral upscale targets, with features like filling
 * invalid pixels.
 */
class RPCPP_DECL BilateralUpscaler: public RPObject
{
public:
    // Controls how many entries to process in one row.Needs to match the
    // definition in the fillin shader
    static const int ROW_WIDTH = 512;

    /**
     * Creates a new upscaler with the given name. Percentage controls
     * the maximum amount of invalid pixels which can be processed, for example
     * a value of 0.05 means that 5% of all pixels may be invalid.
     */
    BilateralUpscaler(RenderStage& parent_stage, bool halfres=false, Texture* source_tex=nullptr, const std::string& name="", float percentage=0.05f, bool stereo=false);
    ~BilateralUpscaler();

    /** Returns the final upscaled texture. */
    Texture* result_tex() const;

    /** Sets all required shaders. */
    void set_shaders(Shader* upscale_shader, Shader* fillin_shader);

    /** Adapts the targets to the current resolution. */
    void set_dimensions();

    /** Updates all targets and buffers. */
    void update();

    void set_layer(int layer);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
