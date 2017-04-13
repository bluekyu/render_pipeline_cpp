#pragma once

#include <unordered_map>

#include <shader.h>

#include <render_pipeline/rpcore/rpobject.h>
#include <render_pipeline/rpcore/image.h>

#include "../include/scattering_plugin.hpp"

namespace rpplugins {

/** Base class for all scattering methods. */
class ScatteringMethod: public rpcore::RPObject
{
public:
	ScatteringMethod(ScatteringPlugin& plugin, const std::string& name="ScatteringMethod"): RPObject(name), handle_(plugin) {}

    ScatteringPlugin& get_handle(void) const { return handle_; }

	virtual void load(void) = 0;
	virtual void compute(void) = 0;

protected:
    ScatteringPlugin& handle_;
};

// ************************************************************************************************
/** Precomputed atmospheric scattering by Eric Bruneton. */
class ScatteringMethodEricBruneton: public ScatteringMethod
{
public:
	ScatteringMethodEricBruneton(ScatteringPlugin& plugin): ScatteringMethod(plugin, "ScatteringMethodEricBruneton") {}

	/** Inits parameters, those should match with the ones specified in common.glsl. */
	void load(void) final;

	/**
	 * Executes a compute shader. The shader object should be a shader
	 * loaded with Shader.load_compute, the shader inputs should be a dict where
	 * the keys are the names of the shader inputs and the values are the
	 * inputs. The workgroup_size has to match the size defined in the
	 * compute shader.
	 */
	void exec_compute_shader(const Shader* shader_obj, const std::vector<ShaderInput>& shader_inputs,
		const LVecBase3i& exec_size, const LVecBase3i& workgroup_size=LVecBase3i(16, 16, 1));

	/** Precomputes the scattering. */
	void compute(void) final;

	/** Creates all textures required for the scattering. */
	void create_textures(void);

	/** Creates all the shaders used for precomputing. */
	void create_shaders(void);

private:
	bool _use_32_bit;

	int _trans_w;
	int _trans_h;

	int _sky_w;
	int _sky_h;

	int _res_r;
	int _res_mu;
	int _res_mu_s;
	int _res_nu;

	int _res_mu_s_nu;

	std::unordered_map<std::string, std::shared_ptr<rpcore::Image>> _textures;
	std::unordered_map<std::string, PT(Shader)> _shaders;
};

}	// namespace rpplugins
