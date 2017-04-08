#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage uses the PSSM Shadow map to render the shadows. */
class PSSMStage: public rpcore::RenderStage
{
public:
    PSSMStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "PSSMStage") {}

	static RequireType& get_global_required_inputs(void) { return required_inputs; }
	static RequireType& get_global_required_pipes(void) { return required_pipes; }

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

    /** Toggle whether to render shadows or whether to just pass through the scene color. */
	void set_render_shadows(bool enabled);

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	bool stereo_mode_ = false;

    bool _enabled;
    std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_shadows = nullptr;
};

}	// namespace rpplugins
