#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

namespace rpplugins {

class PSSMPlugin: public rpcore::BasePlugin
{
public:
    PSSMPlugin(rpcore::RenderPipeline& pipeline);

    RequrieType& get_required_plugins(void) const final;

    void on_stage_setup(void) final;
    void on_pipeline_created(void) final;
    void on_pre_render_update(void) final;

    virtual void request_focus(const LVecBase3f& focus_point, float focus_size);
    virtual bool get_last_focus(LVecBase3f& focus_point, float& focus_size) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
