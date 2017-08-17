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

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rpcore/util/shader_input_blocks.hpp>

namespace rppanda {
class ShowBase;
}

namespace rpcore {

class RenderPipeline;

/**
 * This class manages the loading and binding of commonly used resources,
 * such as textures, models, but also shader inputs.
 */
class CommonResources: public RPObject
{
public:
    CommonResources(RenderPipeline& pipeline);

    /** Generates the shader configuration for the common inputs. */
    void write_config();

    NodePath load_default_skybox();

    /** Updates the commonly used resources, mostly the shader inputs. */
    void update();

private:
    /** Loads the default font used for rendering and assigns it to Globals.font for further usage. */
    void load_fonts();

    void load_textures();

    void load_environment_cubemap();

    /** Loads the prefiltered brdf. */
    void load_prefilter_brdf();

    /**
     * Creates commonly used shader inputs such as the current mvp and
     * registers them to the stage manager so they can be used for rendering.
     */
    void setup_inputs();

    /** Loads the skydome. */
    void load_skydome();

    RenderPipeline& pipeline_;
    rppanda::ShowBase* showbase_;
    std::shared_ptr<GroupedInputBlock> input_ubo_;
};

}
