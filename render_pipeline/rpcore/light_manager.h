#pragma once

#include <LVecBase2.h>
#include <pta_int.h>

#include <render_pipeline/rpcore/rpobject.h>

namespace rpcore {

class RenderPipeline;
class RPLight;
class InternalLightManager;
class ShadowManager;
class GPUCommandQueue;
class Image;

class ShadowStage;

/**
 * This class is a wrapper around the InternalLightManager, and provides
 * additional functionality like setting up all required stages and defines.
 */
class RPCPP_DECL LightManager: public RPObject
{
public:
	// Maximum amount of lights, has to match the definitions in the native code
	static const int MAX_LIGHTS = 65535;

	// Maximum amount of shadow sources
	static const int MAX_SOURCES = 2048;

public:
	LightManager(RenderPipeline* pipeline);
	~LightManager(void);

	/** Returns the total amount of tiles. */
	int get_total_tiles(void) const;

	/** Returns the amount of stored light. */
	size_t get_num_lights(void) const;

	/** Returns the amount of stored shadow sources. */
	size_t get_num_shadow_sources(void) const;

	/** Returns the shadow atlas coverage in percentage. */
	float get_shadow_atlas_coverage(void) const;

	/** Adds a new light. */
	void add_light(RPLight* light);

	/** Removes a light. */
	void remove_light(RPLight* light);

	void update(void);

	/** Reloads all assigned shaders. */
	void reload_shaders(void);

	/** Computes how many tiles there are on screen. */
	void compute_tile_size(void);

	/** Inits the command queue. */
	void init_command_queue(void);

	/** Inits the shadow manager. */
	void initshadow_manager(void);

	/**
	 * Inits the shadows, this has to get called after the stages were
	 * created, because we need the GraphicsOutput of the shadow atlas, which
	 * is not available earlier.
	 */
	void init_shadows(void);

	/** Creates the light storage manager and the buffer to store the light data. */
	void init_internal_manager(void);

	/** Inits all required stages for the lighting. */
	void init_stages(void);

	/** Inits the common defines. */
	void init_defines(void);

	const LVecBase2i& get_num_tiles(void) const;

	GPUCommandQueue* get_cmd_queue(void) const;

private:
	RenderPipeline* pipeline;
	LVecBase2i tile_size;
	LVecBase2i num_tiles;
	InternalLightManager* internal_mgr = nullptr;
	ShadowManager* shadow_manager = nullptr;
	GPUCommandQueue* cmd_queue = nullptr;

	std::shared_ptr<Image> img_light_data = nullptr;
	std::shared_ptr<Image> img_source_data = nullptr;

	PTA_int pta_max_light_index;

	std::shared_ptr<ShadowStage> shadow_stage = nullptr;
};

// ************************************************************************************************
inline const LVecBase2i& LightManager::get_num_tiles(void) const
{
	return num_tiles;
}

inline GPUCommandQueue* LightManager::get_cmd_queue(void) const
{
	return cmd_queue;
}

}	// namespace rpcore
