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

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpcore {
class Image;
}

namespace rpplugins {

class EnvironmentProbe;

/** This stage renders the scene to a cubemap. */
class EnvironmentCaptureStage : public rpcore::RenderStage
{
public:
    EnvironmentCaptureStage(rpcore::RenderPipeline& pipeline);

    RequireType& get_required_inputs() const final { return required_inputs; }
    RequireType& get_required_pipes() const final { return required_pipes; }

    void create() final;
    void reload_shaders() final;

    void update() final;

    void set_shader_input(const ShaderInput& inp) final;

    void set_probe(EnvironmentProbe* probe);

    void set_resolution(int resolution);
    void set_diffuse_resolution(int diffuse_resolution);

    void set_storage_tex(Texture* storage_tex);
    void set_storage_tex_diffuse(Texture* storage_tex_diffuse);

private:
    std::string get_plugin_id() const final;

    /** Setups the cameras to render a cubemap. */
    void setup_camera_rig();

    /** Creates the targets which copy the result texture into the actual storage. */
    void create_store_targets();

    /** Generates the targets which filter the specular cubemap. */
    void create_filter_targets();

    static RequireType required_inputs;
    static RequireType required_pipes;

    int _resolution = 128;
    int _diffuse_resolution = 4;
    NodePath rig_node;
    PTA_int _pta_index;

    std::vector<PT(DisplayRegion)> regions;
    std::vector<NodePath> cameras;

    PT(Texture) _storage_tex = nullptr;
    PT(Texture) _storage_tex_diffuse = nullptr;
    std::unique_ptr<rpcore::Image> _temporary_diffuse_map;

    rpcore::RenderTarget* _target;
    rpcore::RenderTarget* _target_store;
    rpcore::RenderTarget* _target_store_diff;
    rpcore::RenderTarget* _filter_diffuse_target;
    std::vector<rpcore::RenderTarget*> _filter_targets;
};

inline void EnvironmentCaptureStage::set_resolution(int resolution)
{
    _resolution = resolution;
}

inline void EnvironmentCaptureStage::set_diffuse_resolution(int diffuse_resolution)
{
    _diffuse_resolution = diffuse_resolution;
}

inline void EnvironmentCaptureStage::set_storage_tex(Texture* storage_tex)
{
    _storage_tex = storage_tex;
}

inline void EnvironmentCaptureStage::set_storage_tex_diffuse(Texture* storage_tex_diffuse)
{
    _storage_tex_diffuse = storage_tex_diffuse;
}

}    // namespace rpplugins
