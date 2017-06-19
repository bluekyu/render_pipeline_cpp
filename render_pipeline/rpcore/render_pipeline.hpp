#pragma once

#include <memory>

#include <render_pipeline/rpcore/effect.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

class PandaFramework;
class WindowFramework;
class NodePath;

namespace YAML {
class Node;
}

namespace rppanda {
class ShowBase;
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

class RENDER_PIPELINE_DECL RenderPipeline: public RPObject
{
private:
    /** This is not implemented. */
    template <class Type> Type get_setting(const std::string& setting_path) const;
    template <class Type> Type get_setting(const std::string& setting_path, const Type& fallback) const;

public:
    static RenderPipeline* get_global_ptr(void);

    RenderPipeline(void);
    ~RenderPipeline(void);

    /** Call ShowBase::Run() */
    void run(void);

    /**
     * Loads the pipeline configuration from a given filename. Usually
     * this is the 'config/pipeline.ini' file. If you call this more than once,
     * only the settings of the last file will be used.
     */
    void load_settings(const std::string& path);

    /**
     * Reloads all shaders. This will reload the shaders of all plugins,
     * as well as the pipelines internally used shaders. Because of the
     * complexity of some shaders, this operation take might take several
     * seconds. Also notice that all applied effects will be lost, and instead
     * the default effect will be set on all elements again. Due to this fact,
     * this method is primarly useful for fast iterations when developing new
     * shaders.
     */
    void reload_shaders(void);

    /**
     * Setups all required pipeline settings and configuration which have
     * to be set before the showbase is setup. This is called by create(),
     * in case the showbase was not initialized, however you can (and have to)
     * call it manually before you init your custom showbase instance.
     * See the 00-Loading the pipeline sample for more information.
     */
    void pre_showbase_init(void);

    void create(PandaFramework* framework);
    void create(PandaFramework* framework, WindowFramework* window_framework);

    /**
     * Tells the pipeline to use the default loading screen, which consists
     * of a simple loading image. The image source should be a fullscreen
     * 16:9 image, and not too small, to avoid being blurred out.
     */
    void set_loading_screen_image(const std::string& image_source);

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
    size_t load_ies_profile(const std::string& filename);

    /**
     * Sets an effect to the given object, using the specified options.
     * Check out the effect documentation for more information about possible
     * options and configurations. The object should be a nodepath, and the
     * effect will be applied to that nodepath and all nodepaths below whose
     * current effect sort is less than the new effect sort (passed by the
     * sort parameter).
     */
    void set_effect(NodePath& nodepath, const std::string& effect_src,
        const Effect::OptionType& options={}, int sort=30);

    /**
     * Clear applied effect on the node path.
     */
    void clear_effect(NodePath& nodepath);

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

    /**
     * Exports a list of all materials found in the current scene in a
     * serialized format to the given path.
     */
    void export_materials(const std::string& pth);

    void compute_render_resolution(float resolution_scale);
    void compute_render_resolution(int width, int height);

    /** Get setting value iun pipeline setting. */
    ///@{
    /** Get YAML node from given flatten path in pipeline setting. */
    const YAML::Node& get_setting(const std::string& setting_path) const;

    /** Get bool value from given flatten path in pipeline setting. */
    template <> bool get_setting<bool>(const std::string& setting_path) const;
    template <> bool get_setting<bool>(const std::string& setting_path, const bool& fallback) const;

    /** Get int value from given flatten path in pipeline setting. */
    template <> int get_setting(const std::string& setting_path) const;
    template <> int get_setting(const std::string& setting_path, const int& fallback) const;

    /** Get unsigned int value from given flatten path in pipeline setting. */
    template <> unsigned int get_setting(const std::string& setting_path) const;
    template <> unsigned int get_setting(const std::string& setting_path, const unsigned int& fallback) const;

    /** Get size_t value from given flatten path in pipeline setting. */
    template <> size_t get_setting(const std::string& setting_path) const;
    template <> size_t get_setting(const std::string& setting_path, const size_t& fallback) const;

    /** Get float value from given flatten path in pipeline setting. */
    template <> float get_setting(const std::string& setting_path) const;
    template <> float get_setting(const std::string& setting_path, const float& fallback) const;

    /** Get double value from given flatten path in pipeline setting. */
    template <> double get_setting(const std::string& setting_path) const;
    template <> double get_setting(const std::string& setting_path, const double& fallback) const;

    /** Get string value from given flatten path in pipeline setting. */
    template <> std::string get_setting(const std::string& setting_path) const;
    template <> std::string get_setting(const std::string& setting_path, const std::string& fallback) const;
    ///@}

    rppanda::ShowBase* get_showbase(void) const;
    MountManager* get_mount_mgr(void) const;
    StageManager* get_stage_mgr(void) const;
    TagStateManager* get_tag_mgr(void) const;
    LightManager* get_light_mgr(void) const;
    PluginManager* get_plugin_mgr(void) const;
    TaskScheduler* get_task_scheduler(void) const;
    DayTimeManager* get_daytime_mgr(void) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
