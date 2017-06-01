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
    ~BilateralUpscaler(void);

    /** Returns the final upscaled texture. */
    Texture* result_tex(void) const;

    /** Sets all required shaders. */
    void set_shaders(Shader* upscale_shader, Shader* fillin_shader);

    /** Adapts the targets to the current resolution. */
    void set_dimensions(void);

    /** Updates all targets and buffers. */
    void update(void);

    void set_layer(int layer);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
