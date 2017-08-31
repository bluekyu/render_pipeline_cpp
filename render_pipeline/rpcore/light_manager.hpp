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

#include <lvecBase2.h>
#include <pta_int.h>

#include <render_pipeline/rpcore/rpobject.hpp>

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
class RENDER_PIPELINE_DECL LightManager : public RPObject
{
public:
    // Maximum amount of lights, has to match the definitions in the native code
    static const int max_lights_ = 65535;

    // Maximum amount of shadow sources
    static const int max_sources_ = 2048;

public:
    LightManager(RenderPipeline& pipeline);
    LightManager(const LightManager&) = delete;
    LightManager(LightManager&&) = delete;

    ~LightManager();

    LightManager& operator=(const LightManager&) = delete;
    LightManager& operator=(LightManager&&) = delete;

    /** Returns the total amount of tiles. */
    int get_total_tiles() const;

    /** Returns the amount of stored light. */
    size_t get_num_lights() const;

    /** Returns the amount of stored shadow sources. */
    size_t get_num_shadow_sources() const;

    /** Returns the shadow atlas coverage in percentage. */
    float get_shadow_atlas_coverage() const;

    /** Adds a new light. */
    void add_light(RPLight* light);

    /** Removes a light. */
    void remove_light(RPLight* light);

    void update();

    /** Reloads all assigned shaders. */
    void reload_shaders();

    /** Computes how many tiles there are on screen. */
    void compute_tile_size();

    /** Inits the command queue. */
    void init_command_queue();

    /** Inits the shadow manager. */
    void initshadow_manager();

    /**
     * Inits the shadows, this has to get called after the stages were
     * created, because we need the GraphicsOutput of the shadow atlas, which
     * is not available earlier.
     */
    void init_shadows();

    /** Creates the light storage manager and the buffer to store the light data. */
    void init_internal_manager();

    /** Inits all required stages for the lighting. */
    void init_stages();

    /** Inits the common defines. */
    void init_defines();

    const LVecBase2i& get_num_tiles() const;

    GPUCommandQueue* get_cmd_queue() const;

private:
    RenderPipeline& pipeline_;
    LVecBase2i tile_size_;
    LVecBase2i num_tiles_;
    InternalLightManager* internal_mgr_ = nullptr;
    ShadowManager* shadow_manager_ = nullptr;
    GPUCommandQueue* cmd_queue_ = nullptr;

    std::shared_ptr<Image> img_light_data_ = nullptr;
    std::shared_ptr<Image> img_source_data_ = nullptr;

    PTA_int pta_max_light_index_;

    std::shared_ptr<ShadowStage> shadow_stage_ = nullptr;
};

// ************************************************************************************************
inline const LVecBase2i& LightManager::get_num_tiles() const
{
    return num_tiles_;
}

inline GPUCommandQueue* LightManager::get_cmd_queue() const
{
    return cmd_queue_;
}

}
