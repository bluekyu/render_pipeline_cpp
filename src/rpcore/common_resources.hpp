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
    CommonResources(RenderPipeline* pipeline);

    /** Generates the shader configuration for the common inputs. */
    void write_config(void);

    NodePath load_default_skybox(void);

    /** Updates the commonly used resources, mostly the shader inputs. */
    void update(void);

private:
    /** Loads the default font used for rendering and assigns it to Globals.font for further usage. */
    void load_fonts(void);

    void load_textures(void);

    void load_environment_cubemap(void);

    /** Loads the prefiltered brdf. */
    void load_prefilter_brdf(void);

    /**
     * Creates commonly used shader inputs such as the current mvp and
     * registers them to the stage manager so they can be used for rendering.
     */
    void setup_inputs(void);

    /** Loads the skydome. */
    void load_skydome(void);

    RenderPipeline* _pipeline;
    rppanda::ShowBase* _showbase;
    std::shared_ptr<GroupedInputBlock> _input_ubo;
};

}
