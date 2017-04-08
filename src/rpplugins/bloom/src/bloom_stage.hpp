#pragma once

#include <render_pipeline/rpcore/render_stage.h>
#include <render_pipeline/rpcore/image.h>

namespace rpplugins {

class BloomStage: public rpcore::RenderStage
{
public:
	BloomStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "BloomStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	virtual void set_dimensions(void) override;

	void set_num_mips(int num_mips);

	void set_remove_fireflies(bool remove_fireflies);

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

    bool stereo_mode_ = false;

	int _num_mips = 6;
	bool _remove_fireflies = false;

	std::shared_ptr<rpcore::RenderTarget> _target_firefly = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_apply = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_extract = nullptr;
	std::shared_ptr<rpcore::Image> _scene_target_img;
	std::vector<std::shared_ptr<rpcore::RenderTarget>> _downsample_targets;
	std::vector<std::shared_ptr<rpcore::RenderTarget>> _upsample_targets;
};

inline void BloomStage::set_num_mips(int num_mips)
{
	_num_mips = num_mips;
}

inline void BloomStage::set_remove_fireflies(bool remove_fireflies)
{
	_remove_fireflies = remove_fireflies;
}

}	// namespace rpplugins
