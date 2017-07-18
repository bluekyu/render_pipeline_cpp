#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/render_stage.hpp>
#include <render_pipeline/rpcore/image.hpp>

namespace rpplugins {

class EnvironmentProbe;

/** This stage renders the scene to a cubemap. */
class EnvironmentCaptureStage: public rpcore::RenderStage
{
public:
    EnvironmentCaptureStage(rpcore::RenderPipeline& pipeline);

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }

    void create(void) final;
    void reload_shaders(void) final;

    void update(void) final;

    void set_shader_input(const ShaderInput& inp) final;

    void set_probe(const std::shared_ptr<EnvironmentProbe>& probe);

    void set_resolution(int resolution);
    void set_diffuse_resolution(int diffuse_resolution);

    void set_storage_tex(Texture* storage_tex);
    void set_storage_tex_diffuse(Texture* storage_tex_diffuse);

private:
    std::string get_plugin_id(void) const final;

    /** Setups the cameras to render a cubemap. */
    void setup_camera_rig(void);

    /** Creates the targets which copy the result texture into the actual storage. */
    void create_store_targets(void);

    /** Generates the targets which filter the specular cubemap. */
    void create_filter_targets(void);

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
    std::shared_ptr<rpcore::Image> _temporary_diffuse_map;

    std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_store = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_store_diff = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _filter_diffuse_target = nullptr;
    std::vector<std::shared_ptr<rpcore::RenderTarget>> _filter_targets;
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
