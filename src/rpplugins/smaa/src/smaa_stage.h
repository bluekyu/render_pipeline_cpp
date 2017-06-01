#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

/** This stage does the actual SMAA. */
class SMAAStage: public rpcore::RenderStage
{
public:
    SMAAStage(rpcore::RenderPipeline& pipeline, bool use_reprojection);

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

    /** Sets the current jitter index. */
    void set_jitter_index(int idx);

    bool use_reprojection(void) const { return use_reprojection_; }

    void set_area_tex(Texture* area_tex);
    void set_search_tex(Texture* search_tex);

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;

    bool use_reprojection_ = true;
    PTA_int jitter_index_;

    std::shared_ptr<rpcore::RenderTarget> edge_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> blend_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> neighbor_target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> resolve_target_ = nullptr;

    PT(Texture) area_tex_ = nullptr;
    PT(Texture) search_tex_ = nullptr;
};

inline void SMAAStage::set_jitter_index(int idx)
{
    jitter_index_[0] = idx;
}

inline void SMAAStage::set_area_tex(Texture* area_tex)
{
    area_tex_ = area_tex;
}

inline void SMAAStage::set_search_tex(Texture* search_tex)
{
    search_tex_ = search_tex;
}

}
