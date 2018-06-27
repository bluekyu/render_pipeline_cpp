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

#include <memory>

#include <render_pipeline/rpcore/effect.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

class PandaFramework;
class NodePath;
class Filename;

namespace YAML {
class Node;
}

namespace rppanda {
class ShowBase;
}

namespace rpplugins {
class EnvironmentProbe;
}

namespace rpcore {

class StageManager;
class RenderStage;
class TaskScheduler;
class MountManager;
class TagStateManager;
class LightManager;
class RPLight;
class DayTimeManager;
class IESProfileLoader;
class PluginManager;

class RENDER_PIPELINE_DECL RenderPipeline : public RPObject
{
public:
    enum class StereoMode : int
    {
        none = 0,
        left = 1,
        right = 2,
        stereo = 3
    };

public:
    static const std::string& get_version();
    static bool get_version(int& major, int& minor, int& patch);
    static const std::string& get_build_data();
    static const std::string& get_git_commit();

    /** Initialize RenderPipeline. */
    RenderPipeline();

    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline(RenderPipeline&&);

    ~RenderPipeline();

    RenderPipeline& operator=(const RenderPipeline&) = delete;
    RenderPipeline& operator=(RenderPipeline&&);

    /** Call ShowBase::Run() */
    void run();

    /**
     * Loads the pipeline configuration from a given filename. Usually
     * this is the 'config/pipeline.ini' file. If you call this more than once,
     * only the settings of the last file will be used.
     */
    bool load_settings(const Filename& path);

    /**
     * Reloads all shaders. This will reload the shaders of all plugins,
     * as well as the pipelines internally used shaders. Because of the
     * complexity of some shaders, this operation take might take several
     * seconds. Also notice that all applied effects will be lost, and instead
     * the default effect will be set on all elements again. Due to this fact,
     * this method is primarly useful for fast iterations when developing new
     * shaders.
     */
    void reload_shaders();

    /**
     * Setups all required pipeline settings and configuration which have
     * to be set before the showbase is setup. This is called by create(),
     * in case the showbase was not initialized, however you can (and have to)
     * call it manually before you init your custom showbase instance.
     * See the 00-Loading the pipeline sample for more information.
     */
    bool pre_showbase_init();

    /**
     * This creates the pipeline, and setups all buffers. It also
     * constructs the showbase. The settings should have been loaded before
     * calling this, and also the base and write path should have been
     * initialized properly (see MountManager).
     *
     * If base is None, the showbase used in the RenderPipeline constructor
     * will be used and initialized. Otherwise it is assumed that base is an
     * initialized ShowBase object. In this case, you should call
     * pre_showbase_init() before initializing the ShowBase
     */
    bool create(int argc, char* argv[], rppanda::ShowBase* base = nullptr);
    bool create(PandaFramework* framework, rppanda::ShowBase* base = nullptr);

    /**
     * Tells the pipeline to use the default loading screen, which consists
     * of a simple loading image. The image source should be a fullscreen
     * 16:9 image, and not too small, to avoid being blurred out.
     */
    void set_loading_screen_image(const Filename& image_source);

    /**
     * Adds a new light to the rendered lights, check out the LightManager
     * add_light documentation for further information.
     */
    void add_light(RPLight* light);

    void remove_light(RPLight* light);

    /**
     * Loads an IES profile from a given filename and returns a handle which
     * can be used to set an ies profile on a light.
     */
    size_t load_ies_profile(const Filename& filename);

    /**
     * Sets an effect to the given object, using the specified options.
     * Check out the effect documentation for more information about possible
     * options and configurations. The object should be a nodepath, and the
     * effect will be applied to that nodepath and all nodepaths below whose
     * current effect sort is less than the new effect sort (passed by the
     * sort parameter).
     */
    void set_effect(const NodePath& nodepath, const Filename& effect_src,
        const Effect::OptionType& options={}, int sort=30);

    /**
     * Clear applied effect on the node path.
     */
    void clear_effect(NodePath& nodepath);

    /**
     * Constructs a new environment probe and returns the handle, so that
     * the probe can be modified. In case the env_probes plugin is not activated,
     * this returns a dummy object which can be modified but has no impact.
     */
    rpplugins::EnvironmentProbe* add_environment_probe();

    /**
     * Prepares a given scene, by converting panda lights to render pipeline
     * lights. This also converts all empties with names starting with 'ENVPROBE'
     * to environment probes. Conversion of blender to render pipeline lights
     * is done by scaling their intensity by 100 to match lumens.
     *
     * Additionally, this finds all materials with the 'TRANSPARENT' shading
     * model, and sets the proper effects on them to ensure they are rendered
     * properly.
     *
     * This method also returns a dictionary with handles to all created
     * objects, that is lights, environment probes, and transparent objects.
     * This can be used to store them and process them later on, or delete
     * them when a newer scene is loaded.
     */
    void prepare_scene(const NodePath& scene);

    void compute_render_resolution(float resolution_scale);
    void compute_render_resolution(int width, int height);
    void compute_render_resolution(float resolution_scale, int width, int height);

    bool is_stereo_mode() const;
    StereoMode get_stereo_mode() const;

    /** Get setting value iun pipeline setting. */
    ///@{
    /** Get YAML node from given flatten path in pipeline setting. */
    const YAML::Node& get_setting(const std::string& setting_path) const;

    /** This is not implemented. */
    template <class Type> Type get_setting(const std::string& setting_path) const;
    template <class Type> Type get_setting(const std::string& setting_path, const Type& fallback) const;
    ///@}

    rppanda::ShowBase* get_showbase() const;
    MountManager* get_mount_mgr() const;
    StageManager* get_stage_mgr() const;
    TagStateManager* get_tag_mgr() const;
    LightManager* get_light_mgr() const;
    PluginManager* get_plugin_mgr() const;
    TaskScheduler* get_task_scheduler() const;
    DayTimeManager* get_daytime_mgr() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// ************************************************************************************************

inline bool RenderPipeline::is_stereo_mode() const
{
    return get_stereo_mode() != StereoMode::none;
}

/** Get bool value from given flatten path in pipeline setting. */
template <> RENDER_PIPELINE_DECL bool RenderPipeline::get_setting(const std::string& setting_path) const;
template <> RENDER_PIPELINE_DECL bool RenderPipeline::get_setting(const std::string& setting_path, const bool& fallback) const;

/** Get int value from given flatten path in pipeline setting. */
template <> RENDER_PIPELINE_DECL int RenderPipeline::get_setting(const std::string& setting_path) const;
template <> RENDER_PIPELINE_DECL int RenderPipeline::get_setting(const std::string& setting_path, const int& fallback) const;

/** Get unsigned int value from given flatten path in pipeline setting. */
template <> RENDER_PIPELINE_DECL unsigned int RenderPipeline::get_setting(const std::string& setting_path) const;
template <> RENDER_PIPELINE_DECL unsigned int RenderPipeline::get_setting(const std::string& setting_path, const unsigned int& fallback) const;

/** Get size_t value from given flatten path in pipeline setting. */
template <> RENDER_PIPELINE_DECL size_t RenderPipeline::get_setting(const std::string& setting_path) const;
template <> RENDER_PIPELINE_DECL size_t RenderPipeline::get_setting(const std::string& setting_path, const size_t& fallback) const;

/** Get float value from given flatten path in pipeline setting. */
template <> RENDER_PIPELINE_DECL float RenderPipeline::get_setting(const std::string& setting_path) const;
template <> RENDER_PIPELINE_DECL float RenderPipeline::get_setting(const std::string& setting_path, const float& fallback) const;

/** Get double value from given flatten path in pipeline setting. */
template <> RENDER_PIPELINE_DECL double RenderPipeline::get_setting(const std::string& setting_path) const;
template <> RENDER_PIPELINE_DECL double RenderPipeline::get_setting(const std::string& setting_path, const double& fallback) const;

/** Get string value from given flatten path in pipeline setting. */
template <> RENDER_PIPELINE_DECL std::string RenderPipeline::get_setting(const std::string& setting_path) const;
template <> RENDER_PIPELINE_DECL std::string RenderPipeline::get_setting(const std::string& setting_path, const std::string& fallback) const;

}
