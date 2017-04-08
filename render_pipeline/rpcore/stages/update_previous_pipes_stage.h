#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpcore {

class RenderTarget;

/**
 * This stage is constructed by the StageManager and stores all the
 * current pipes in the previous pipe storage.
 * This stage is a bit special, and not like the other stages, it does not
 * specify inputs, since the StageManager passes all required inputs on demand.
 * Also this stage does not load any shaders, but creates them on the fly.
 */
class UpdatePreviousPipesStage: public RenderStage
{
public:
	UpdatePreviousPipesStage(RenderPipeline& pipeline): RenderStage(pipeline, "UpdatePreviousPipesStage") {}

	virtual RequireType& get_required_inputs(void) const;
	virtual RequireType& get_required_pipes(void) const;

	/**
	 * Adds a new texture which should be copied from "from_tex" to
	 * "to_tex". This should be called before the stage gets constructed.
	 * The source texture is expected to have the same size as the render
	 * resolution.
	 */
	void add_transfer(Texture* from_tex, Texture* to_tex);

	virtual void create(void);

	/** Sets the dimensions on all targets. See RenderTarget::set_dimensions. */
	virtual void set_dimensions(void) override;

	virtual void reload_shaders(void) override;

	std::string get_sampler_type(Texture* tex, bool can_write=false);

	/**
	 * Returns the matching GLSL sampler lookup for a texture, storing the
	 * result in the given glsl variable.
	 */
	std::string get_sampler_lookup(Texture* tex, const std::string& dest_name,
		const std::string& sampler_name, const std::string& coord_var);

	/** Returns the matching GLSL code to store the given data in a given texture. */
	std::string get_store_code(Texture* tex, const std::string& sampler_name,
		const std::string& coord_var, const std::string& data_var);

private:
	virtual std::string get_plugin_id(void) const;

	std::shared_ptr<RenderTarget> _target = nullptr;
	std::vector<std::pair<Texture*, Texture*>> _transfers;
};

inline void UpdatePreviousPipesStage::add_transfer(Texture* from_tex, Texture* to_tex)
{
	_transfers.push_back({from_tex, to_tex});
}

}	// namespace rpcore
