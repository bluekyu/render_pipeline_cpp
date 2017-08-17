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

/** This stage does the actual SMAA. */
class SMAAStage: public rpcore::RenderStage
{
public:
    SMAAStage(rpcore::RenderPipeline& pipeline, bool use_reprojection);

    RequireType& get_required_inputs() const final { return required_inputs; }
    RequireType& get_required_pipes() const final { return required_pipes; }
    ProduceType get_produced_pipes() const final;

    void create() final;
    void reload_shaders() final;

    /** Sets the current jitter index. */
    void set_jitter_index(int idx);

    bool use_reprojection() const { return use_reprojection_; }

    void set_area_tex(Texture* area_tex);
    void set_search_tex(Texture* search_tex);

private:
    std::string get_plugin_id() const final;

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
