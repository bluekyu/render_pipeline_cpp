#include <dtoolbase.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"

#include <cctype>
#include <chrono>

#include <asyncTask.h>
#include <pandaFramework.h>
#include <graphicsWindow.h>
#include <pandaSystem.h>
#include <virtualFileSystem.h>
#include <load_prc_file.h>
#include <nodePathCollection.h>
#include <pointLight.h>
#include <spotLight.h>
#include <materialAttrib.h>
#include <genericAsyncTask.h>
#include <geomTristrips.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/mount_manager.hpp"
#include "render_pipeline/rpcore/light_manager.hpp"
#include "render_pipeline/rpcore/util/task_scheduler.hpp"
#include "render_pipeline/rpcore/pluginbase/day_manager.hpp"
#include "render_pipeline/rpcore/pluginbase/manager.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rpcore/logger.hpp"

#include "render_pipeline/rpcore/stages/ambient_stage.hpp"
#include "render_pipeline/rpcore/stages/combine_velocity_stage.hpp"
#include "render_pipeline/rpcore/stages/downscale_z_stage.hpp"
#include "render_pipeline/rpcore/stages/final_stage.hpp"
#include "render_pipeline/rpcore/stages/gbuffer_stage.hpp"
#include "render_pipeline/rpcore/stages/upscale_stage.hpp"

#include "render_pipeline/rpcore/native/tag_state_manager.h"
#include "render_pipeline/rpcore/native/rp_point_light.h"
#include "render_pipeline/rpcore/native/rp_spot_light.h"

#include "rpcore/common_resources.hpp"
#include "rpcore/gui/debugger.hpp"
#include "rpcore/gui/error_message_display.hpp"
#include "rpcore/gui/loading_screen.hpp"
#include "rpcore/util/ies_profile_loader.hpp"
#include "rplibs/yaml.hpp"

namespace rpcore {

static RenderPipeline* global_ptr_ = nullptr;

class RenderPipeline::Impl
{
public:
    static const char* stages[];

    Impl(RenderPipeline& self);
    ~Impl(void);

    /**
     * Task which repeatedly clears the state cache to avoid storing
     * unused states. While running once a while, this task prevents over-polluting
     * the state-cache with unused states. This complements Panda3D's internal
     * state garbarge collector, which does a great job, but still cannot clear
     * up all states.
     */
    static AsyncTask::DoneStatus clear_state_cache(GenericAsyncTask* task, void* user_data);

    /** Update task which gets called before the rendering, and updates all managers. */
    static AsyncTask::DoneStatus manager_update_task(GenericAsyncTask* task, void* user_data);

    /**
     * Updates the commonly used inputs each frame. This is a seperate
     * task to be able view detailed performance information in pstats, since
     * a lot of matrix calculations are involved here.
     */
    static AsyncTask::DoneStatus update_inputs_and_stages(GenericAsyncTask* task, void* user_data);

    /**
     * Update task which gets called before the rendering, and updates the
     * plugins. This is a seperate task to split the work, and be able to do
     * better performance analysis in pstats later on.
     */
    static AsyncTask::DoneStatus plugin_pre_render_update(GenericAsyncTask* task, void* user_data);

    /**
     * Update task which gets called after the rendering, and should cleanup
     * all unused states and objects. This also triggers the plugin post-render
     * update hook.
     */
    static AsyncTask::DoneStatus plugin_post_render_update(GenericAsyncTask* task, void* user_data);

    /**
     * Checks for window events. This mainly handles incoming resizes,
     * and calls the required handlers.
     */
    static void handle_window_event(const Event* ev, void* user_data);

    void reload_shaders(void);

    bool create(void);

    /**
     * Re-applies all custom shaders the user applied, to avoid them getting
     * removed when the shaders are reloaded.
     */
    void apply_custom_shaders(void);

    /**
     * Internal method to create all managers and instances. This also
     * initializes the commonly used render stages, which are always required,
     * independently of which plugins are enabled.
     */
    void create_managers(void);

    /**
     * Prints information about the system used, including information
     * about the used Panda3D build. Also checks if the Panda3D build is out
     * of date.
     */
    void analyze_system(void);

    /**
     * Internal method to initialize all managers, after they have been
     * created earlier in _create_managers. The creation and initialization
     * is seperated due to the fact that plugins and various other subprocesses
     * have to get initialized inbetween.
     */
    void initialize_managers(void);

    /**
     * Internal method to initialize the GUI-based debugger. In case debugging
     * is disabled, this constructs a dummy debugger, which does nothing.
     * The debugger itself handles the various onscreen components.
     */
    void init_debugger(void);

    /**
     * Inits all global bindings. This includes references to the global
     * ShowBase instance, as well as the render resolution, the GUI font,
     * and various global logging and output methods.
     */
    void init_globals(void);

    /**
     * Sets the default effect used for all objects if not overridden, this
     * just calls set_effect with the default effect and options as parameters.
     * This uses a very low sort, to make sure that overriding the default
     * effect does not require a custom sort parameter to be passed.
     */
    void set_default_effect(void);

    /**
     * Sets the default camera settings, this includes the cameras
     * near and far plane, as well as FoV. The reason for this is, that pandas
     * default field of view is very small, and thus we increase it.
     */
    void adjust_camera_settings(void);

    void adjust_lens_setting(void);

    /**
     * Initialize the internally used render resolution. This might differ
     * from the window dimensions in case a resolution scale is set.
     */
    void compute_render_resolution(void);

    /**
     * Inits the the given showbase object. This is part of an alternative
     * method of initializing the showbase. In case base is None, a new
     * ShowBase instance will be created and initialized. Otherwise base() is
     * expected to either be an uninitialized ShowBase instance, or an
     * initialized instance with pre_showbase_init() called inbefore.
     */
    bool init_showbase(void);

    /**
     * Internal method to init the tasks and keybindings. This constructs
     * the tasks to be run on a per-frame basis.
     */
    void init_bindings(void);

    /** Creates commonly used defines for the shader configuration. */
    void create_common_defines(void);

    /**
     * Inits the commonly used stages, which don't belong to any plugin,
     * but yet are necessary and widely used.
     */
    void init_common_stages(void);

    /**
     * Returns the default skybox, with a scale of <size>, and all
     * proper effects and shaders already applied. The skybox is already
     * parented to render as well.
     */
    NodePath create_default_skybox(float size=40000);

    void internal_set_effect(NodePath nodepath, const std::string& effect_src,
        const Effect::OptionType& options=Effect::OptionType(), int sort=30);

    void clear_effect(NodePath& nodepath);

    void handle_window_resize(void);

    template <class T>
    T get_setting(const std::string& setting_path) const;

    template <class T>
    T get_setting(const std::string& setting_path, const T& fallback) const;

public:
    RenderPipeline& self_;

    std::shared_ptr<PandaFramework> panda_framework_;

    rplibs::YamlFlatType settings;
    LVecBase2i last_window_dims;
    std::chrono::system_clock::time_point* first_frame_ = nullptr;
    std::vector<std::tuple<NodePath, std::string, Effect::OptionType, int>> applied_effects;

    bool pre_showbase_initialized = false;

    Debugger* debugger = nullptr;
    LoadingScreen* loading_screen = nullptr;
    CommonResources* common_resources = nullptr;

    rppanda::ShowBase* showbase_ = nullptr;
    TaskScheduler* task_scheduler_ = nullptr;
    TagStateManager* tag_mgr_ = nullptr;
    PluginManager* plugin_mgr_ = nullptr;
    MountManager* mount_mgr_ = nullptr;
    StageManager* stage_mgr_ = nullptr;
    LightManager* light_mgr_ = nullptr;
    DayTimeManager* daytime_mgr_ = nullptr;
    IESProfileLoader* ies_loader_ = nullptr;
};

const char* RenderPipeline::Impl::stages[] = { "gbuffer", "shadow", "voxelize", "envmap", "forward" };

RenderPipeline::Impl::Impl(RenderPipeline& self): self_(self)
{
}

RenderPipeline::Impl::~Impl(void)
{
    self_.debug("Destructing RenderPipeline.");

    delete first_frame_;

    delete common_resources;
    delete ies_loader_;
    delete daytime_mgr_;
    delete light_mgr_;
    delete stage_mgr_;
    delete tag_mgr_;
    delete task_scheduler_;
    delete showbase_;
    delete debugger;

    // should delete at last to delete resources in DLL module.
    delete plugin_mgr_;
    delete loading_screen;

    delete mount_mgr_;

    global_ptr_ = nullptr;
}

void RenderPipeline::Impl::internal_set_effect(NodePath nodepath, const std::string& effect_src, const Effect::OptionType& options, int sort)
{
    const auto& effect = Effect::load(effect_src, options);
    if (!effect)
    {
        self_.error("Could not apply effect");
        return;
    }

    for (size_t i = 0; i < std::extent<decltype(stages)>::value; ++i)
    {
        const std::string& stage = stages[i];
        if (!effect->get_option("render_" + stage))
        {
            nodepath.hide(tag_mgr_->get_mask(stage));
        }
        else
        {
            Shader* shader = effect->get_shader_obj(stage);
            if (stage == "gbuffer")
            {
                nodepath.set_shader(shader, 25);
            }
            else
            {
                tag_mgr_->apply_state(stage, nodepath, shader, std::to_string(effect->get_effect_id()), 25 + 10 * i + sort);
            }
            nodepath.show_through(tag_mgr_->get_mask(stage));
        }
    }

    if (effect->get_option("render_gbuffer") && effect->get_option("render_forward"))
    {
        self_.error("You cannot render an object forward and deferred at the "
            "same time! Either use render_gbuffer or use render_forward, "
            "but not both.");
    }
}

void RenderPipeline::Impl::clear_effect(NodePath& nodepath)
{
    auto iter = applied_effects.begin();
    const auto iter_end = applied_effects.end();
    for (; iter != iter_end; ++iter)
    {
        if (std::get<0>(*iter) == nodepath)
            break;
    }

    if (iter == iter_end)
        return;

    auto options = Effect::get_default_options();
    options.insert(std::get<2>(*iter).begin(), std::get<2>(*iter).end());

    for (size_t i = 0; i < std::extent<decltype(stages)>::value; ++i)
    {
        const std::string& stage = stages[i];
        if (options.at("render_" + stage))
        {
            if (stage == "gbuffer")
            {
                nodepath.clear_shader();
            }
            else
            {
                tag_mgr_->cleanup_state(stage, nodepath);
            }
        }
        nodepath.show(tag_mgr_->get_mask(stage));
    }
    applied_effects.erase(iter);
}

AsyncTask::DoneStatus RenderPipeline::Impl::clear_state_cache(GenericAsyncTask* task, void* user_data)
{
    task->set_delay(2.0f);
    TransformState::clear_cache();
    RenderState::clear_cache();
    return AsyncTask::DS_again;
}

AsyncTask::DoneStatus RenderPipeline::Impl::manager_update_task(GenericAsyncTask* task, void* user_data)
{
    RenderPipeline* rp = reinterpret_cast<RenderPipeline*>(user_data);
    rp->impl_->task_scheduler_->step();
    // TODO: implements
    //self._listener.update()
    if (rp->impl_->debugger)
        rp->impl_->debugger->update();
    rp->impl_->daytime_mgr_->update();
    rp->impl_->light_mgr_->update();

    if (rpcore::Globals::clock->get_frame_count() == 10)
    {
        rp->debug("Hiding loading screen after 10 pre-rendered frames.");
        rp->impl_->loading_screen->remove();
    }

    return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus RenderPipeline::Impl::update_inputs_and_stages(GenericAsyncTask* task, void* user_data)
{
    RenderPipeline* rp = reinterpret_cast<RenderPipeline*>(user_data);
    rp->impl_->common_resources->update();
    rp->impl_->stage_mgr_->update();
    return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus RenderPipeline::Impl::plugin_pre_render_update(GenericAsyncTask* task, void* user_data)
{
    RenderPipeline* rp = reinterpret_cast<RenderPipeline*>(user_data);
    rp->impl_->plugin_mgr_->on_pre_render_update();
    return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus RenderPipeline::Impl::plugin_post_render_update(GenericAsyncTask* task, void* user_data)
{
    RenderPipeline* rp = reinterpret_cast<RenderPipeline*>(user_data);
    rp->impl_->plugin_mgr_->on_post_render_update();
    if (rp->impl_->first_frame_)
    {
        const std::chrono::duration<float>& duration = std::chrono::system_clock::now() - *rp->impl_->first_frame_;
        rp->debug(fmt::format("Took {} s until first frame", duration.count()));
        delete rp->impl_->first_frame_;
        rp->impl_->first_frame_ = nullptr;
    }
    return AsyncTask::DS_cont;
}

void RenderPipeline::Impl::handle_window_event(const Event* ev, void* user_data)
{
    RenderPipeline* rp = reinterpret_cast<RenderPipeline*>(user_data);
    const auto& rp_impl = rp->impl_;

    auto last_resolution = Globals::resolution;

    LVecBase2i window_dims(rp_impl->showbase_->get_win()->get_size());
    if (window_dims != rp_impl->last_window_dims && window_dims != Globals::native_resolution)
    {
        rp_impl->last_window_dims = window_dims;

        // Ensure the dimensions are a multiple of 4, and if not, correct it
        if ((window_dims.get_x() & 0x3) != 0 || (window_dims.get_y() & 0x3) != 0)
        {
            rp->debug(fmt::format("Correcting non-multiple of 4 window size: ({}, {})", window_dims.get_x(), window_dims.get_y()));
            window_dims.set_x(window_dims.get_x() - (window_dims.get_x() & 0x3));
            window_dims.set_y(window_dims.get_y() - (window_dims.get_y() & 0x3));
            WindowProperties props = WindowProperties::size(window_dims.get_x(), window_dims.get_y());
            rp_impl->showbase_->get_win()->request_properties(props);
        }

        rp->debug(fmt::format("Resizing to {} x {}", window_dims.get_x(), window_dims.get_y()));
        Globals::native_resolution = window_dims;
        rp_impl->compute_render_resolution();
        rp_impl->handle_window_resize();
    }

    // set lens parameter after window event.
    // and set highest priority for running first.
    rp_impl->showbase_->add_task([](GenericAsyncTask* task, void* user_data) -> AsyncTask::DoneStatus
    {
        RenderPipeline::Impl* rp_impl = reinterpret_cast<RenderPipeline::Impl*>(user_data);
        rp_impl->adjust_lens_setting();
        return AsyncTask::DS_done;
    }, rp_impl.get(), "RP_HandleWindowResize", -100);
}

void RenderPipeline::Impl::reload_shaders(void)
{
    if (debugger)
    {
        self_.debug("Reloading shaders ..");
        debugger->get_error_msg_handler()->clear_messages();
        debugger->set_reload_hint_visible(true);

        for (int i = 0; i < 2; ++i)
        {
            showbase_->get_task_mgr()->poll();
            showbase_->get_graphics_engine()->render_frame();
        }
    }

    tag_mgr_->cleanup_states();
    stage_mgr_->reload_shaders();
    light_mgr_->reload_shaders();
    set_default_effect();
    plugin_mgr_->on_shader_reload();
    if (debugger)
        debugger->set_reload_hint_visible(false);
    apply_custom_shaders();
}

bool RenderPipeline::Impl::create(void)
{
    const auto& start_time = std::chrono::system_clock::now();
    if (!init_showbase())
        return false;

    if (!showbase_->get_win()->get_gsg()->get_supports_compute_shaders())
    {
        self_.fatal("Sorry, your GPU does not support compute shaders! Make sure\n"
            "you have the latest drivers. If you already have, your gpu might\n"
            "be too old, or you might be using the open source drivers on linux.");
        return false;
    }

    init_globals();
    loading_screen->create();
    adjust_camera_settings();
    create_managers();
    plugin_mgr_->load();
    plugin_mgr_->on_load();
    daytime_mgr_->load_settings();
    common_resources->write_config();
    init_debugger();

    plugin_mgr_->on_stage_setup();
    plugin_mgr_->on_post_stage_setup();

    create_common_defines();
    initialize_managers();
    create_default_skybox();

    plugin_mgr_->on_pipeline_created();

    // TODO: implement
    //self._listener = NetworkCommunication(self)
    set_default_effect();

    // Measure how long it took to initialize everything, and also store
    // when we finished, so we can measure how long it took to render the
    // first frame (where the shaders are actually compiled)
    const std::chrono::duration<float>& init_duration = std::chrono::system_clock::now() - start_time;
    first_frame_ = new auto(std::chrono::system_clock::now());
    self_.debug(fmt::format("Finished initialization in {} s, first frame: {}", init_duration.count(), rpcore::Globals::clock->get_frame_count()));

    return true;
}

void RenderPipeline::Impl::apply_custom_shaders(void)
{
    self_.debug(fmt::format("Re-applying {} custom shaders", applied_effects.size()));
    for (auto& args: applied_effects)
        internal_set_effect(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args));
}

void RenderPipeline::Impl::create_managers(void)
{
    self_.trace("Creating managers ...");

    task_scheduler_ = new TaskScheduler(&self_);
    tag_mgr_ = new TagStateManager(Globals::base->get_cam());
    plugin_mgr_ = new PluginManager(self_);
    stage_mgr_ = new StageManager(self_);
    light_mgr_ = new LightManager(&self_);
    daytime_mgr_ = new DayTimeManager(self_);
    ies_loader_ = new IESProfileLoader(&self_);
    common_resources = new CommonResources(&self_);
    init_common_stages();
}

void RenderPipeline::Impl::analyze_system(void)
{
    self_.debug(fmt::format("Using C++ with architecture {}", PandaSystem::get_platform()));
    self_.debug(fmt::format("Using Panda3D {} built on {}", PandaSystem::get_version_string(), PandaSystem::get_build_date()));
    if (!PandaSystem::get_git_commit().empty())
        self_.debug(fmt::format("Using git commit {}", PandaSystem::get_git_commit()));
    else
        self_.debug("Using custom Panda3D build");

    // C++ does not require checking version.
    //if (check_version())
    //{
    //    fatal("Your Panda3D version is outdated! Please update to the newest \n"
    //    "git version! Checkout https://github.com/panda3d/panda3d to "
    //    "compile panda from source, or get a recent buildbot build.")
    //}
}

void RenderPipeline::Impl::initialize_managers(void)
{
    self_.trace("Initializing managers ...");

    stage_mgr_->setup();
    stage_mgr_->reload_shaders();
    light_mgr_->reload_shaders();
    init_bindings();
    light_mgr_->init_shadows();
}

void RenderPipeline::Impl::init_debugger(void)
{
    if (self_.get_setting<bool>("pipeline.display_debugger"))
    {
        debugger = new Debugger(&self_);
    }
    else
    {
        // C++ does not implement this part.
    }
}

void RenderPipeline::Impl::init_globals(void)
{
    self_.trace("Initailizing global parameters.");

    Globals::load(showbase_);
    Globals::native_resolution = showbase_->get_win()->get_size();

    last_window_dims = Globals::native_resolution;
    compute_render_resolution();

    // This is unused code.
    //RenderTarget.RT_OUTPUT_FUNC = lambda *args: RPObject.global_warn("RenderTarget", *args[1:])

    // TODO: implement
    RenderTarget::USE_R11G11B10 = self_.get_setting<bool>("pipeline.use_r11_g11_b10", false);

    if (settings.find("pipeline.stereo_mode") == settings.end())
    {
        self_.warn("Cannot find 'pipeline.stereo_mode' setting, so set to default (false).");
        settings.insert({"pipeline.stereo_mode", YAML::Node(false)});
    }
}

void RenderPipeline::Impl::set_default_effect(void)
{
    self_.set_effect(Globals::render, "effects/default.yaml", {}, -10);
}

void RenderPipeline::Impl::adjust_camera_settings(void)
{
    adjust_lens_setting();

    if (self_.get_setting<bool>("pipeline.stereo_mode"))
    {
        NodePath cam = showbase_->get_cam();

        if (cam.find("left_eye").is_empty())
            cam.attach_new_node("left_eye");

        if (cam.find("right_eye").is_empty())
            cam.attach_new_node("right_eye");
    }
}

void RenderPipeline::Impl::adjust_lens_setting(void)
{
    Lens* lens = showbase_->get_cam_lens();
    lens->set_near_far(0.1f, 70000.0f);
    lens->set_fov(40.0f);
    lens->set_film_size(Globals::resolution.get_x(), Globals::resolution.get_y());
}

void RenderPipeline::Impl::compute_render_resolution(void)
{
    const float scale_factor = self_.get_setting<float>("pipeline.resolution_scale", 1.0f);
    int resolution_width;
    int resolution_height;

    if (scale_factor == 0)
    {
        resolution_width = self_.get_setting<int>("pipeline.resolution_width", Globals::native_resolution.get_x());
        resolution_height = self_.get_setting<int>("pipeline.resolution_height", Globals::native_resolution.get_y());
    }
    else
    {
        resolution_width = int(float(Globals::native_resolution.get_x()) * scale_factor);
        resolution_height = int(float(Globals::native_resolution.get_y()) * scale_factor);
    }

    // Make sure the resolution is a multiple of 4
    resolution_width = resolution_width - (resolution_width & 0x3);
    resolution_height = resolution_height - (resolution_height & 0x3);
    self_.debug(fmt::format("Render resolution is {} x {}", resolution_width, resolution_height));
    Globals::resolution = LVecBase2i(resolution_width, resolution_height);
}

bool RenderPipeline::Impl::init_showbase(void)
{
    // C++ Panda3D has no ShowBase.
    //if (!base)
    //{
    if (!self_.pre_showbase_init())
        return false;
    showbase_ = new rppanda::ShowBase(panda_framework_.get());
    //}
    //else
    //{
    //    if (!pre_showbase_initialized)
    //    {
    //        fatal(    "You constructed your own ShowBase object but you\n"
    //                "did not call pre_show_base_init() on the render\n"
    //                "pipeline object before! Checkout the 00-Loading the\n"
    //                "pipeline sample to see how to initialize the RP.");
    //    }
    //    showbase = base;
    //}

    // Now that we have a showbase and a window, we can print out driver info
    auto gsg = showbase_->get_win()->get_gsg();
    self_.debug(fmt::format("Driver Version = {}", gsg->get_driver_version()));
    self_.debug(fmt::format("Driver Vendor = {}", gsg->get_driver_vendor()));
    self_.debug(fmt::format("Driver Renderer = {}", gsg->get_driver_renderer()));

    return true;
}

void RenderPipeline::Impl::init_bindings(void)
{
    showbase_->add_task(&Impl::manager_update_task, &self_, "RP_UpdateManagers", 10);
    showbase_->add_task(&Impl::plugin_pre_render_update, &self_, "RP_Plugin_BeforeRender", 12);
    showbase_->add_task(&Impl::update_inputs_and_stages, &self_, "RP_UpdateInputsAndStages", 18);

    // igloop has 50 sorting value.
    showbase_->add_task(&Impl::plugin_post_render_update, &self_, "RP_Plugin_AfterRender", 51);
    showbase_->do_method_later(0.5f, &Impl::clear_state_cache, "RP_ClearStateCache", nullptr);
    showbase_->accept("window-event", &Impl::handle_window_event, &self_);
}

void RenderPipeline::Impl::create_common_defines(void)
{
    static const double round_ratio = std::pow(10.0, 10.0);

    self_.trace("Creating common defines ...");

    auto& defines = stage_mgr_->get_defines();
    defines["CAMERA_NEAR"] = std::to_string(std::round(double(Globals::base->get_cam_lens()->get_near()) * round_ratio) / round_ratio);
    defines["CAMERA_FAR"] = std::to_string(std::round(double(Globals::base->get_cam_lens()->get_far()) * round_ratio) / round_ratio);

    // Work arround buggy nvidia driver, which expects arrays to be const
    if (showbase_->get_win()->get_gsg()->get_driver_version().find("NVIDIA 361.43") != std::string::npos)
        defines["CONST_ARRAY"] = std::string("const");
    else
        defines["CONST_ARRAY"] = std::string("");

    // Provide driver vendor as a define
    std::string vendor = showbase_->get_win()->get_gsg()->get_driver_vendor();
    std::transform(vendor.begin(), vendor.end(), vendor.begin(), [](std::string::value_type c) { return std::tolower(c); });
    defines["IS_NVIDIA"] = std::string(vendor.find("nvidia") != std::string::npos ? "1" : "0");
    defines["IS_AMD"] = std::string(vendor.find("amd") != std::string::npos ? "1" : "0");
    defines["IS_INTEL"] = std::string(vendor.find("intel") != std::string::npos ? "1" : "0");

    defines["REFERENCE_MODE"] = std::string(self_.get_setting<bool>("pipeline.reference_mode", false) ? "1" : "0");

    bool stereo_mode = self_.get_setting<bool>("pipeline.stereo_mode");
    bool has_nv_stereo_view = rpcore::Globals::base->get_win()->get_gsg()->has_extension("GL_NV_stereo_view_rendering") &&
        self_.get_setting<bool>("pipeline.nvidia_stereo_view", false);

    defines["STEREO_MODE"] = std::string(stereo_mode ? "1" : "0");
    defines["NVIDIA_STEREO_VIEW"] = std::string(has_nv_stereo_view ? "1" : "0");

    light_mgr_->init_defines();
    plugin_mgr_->init_defines();
}

void RenderPipeline::Impl::init_common_stages(void)
{
    self_.trace("Initailizing common stages ...");

    stage_mgr_->add_stage(std::make_shared<AmbientStage>(self_));
    stage_mgr_->add_stage(std::make_shared<GBufferStage>(self_));
    stage_mgr_->add_stage(std::make_shared<FinalStage>(self_));
    stage_mgr_->add_stage(std::make_shared<DownscaleZStage>(self_));
    stage_mgr_->add_stage(std::make_shared<CombineVelocityStage>(self_));

    // Add an upscale/downscale stage in case we render at a different resolution
    if (Globals::resolution != Globals::native_resolution)
        stage_mgr_->add_stage(std::make_shared<UpscaleStage>(self_));
}

NodePath RenderPipeline::Impl::create_default_skybox(float size)
{
    NodePath skybox = common_resources->load_default_skybox();
    skybox.set_scale(size);
    skybox.reparent_to(Globals::render);
    skybox.set_bin("unsorted", 10000);

    self_.set_effect(skybox, "effects/skybox.yaml", {
        { "render_shadow", false },
        { "render_envmap", false },
        { "render_voxelize", false },
        { "alpha_testing", false },
        { "normal_mapping", false },
        { "parallax_mapping", false },
    }, 1000);

    return skybox;
}

void RenderPipeline::Impl::handle_window_resize(void)
{
    adjust_lens_setting();

    light_mgr_->compute_tile_size();
    stage_mgr_->handle_window_resize();
    if (debugger)
        debugger->handle_window_resize();
    plugin_mgr_->on_window_resized();
}

template <class T>
T RenderPipeline::Impl::get_setting(const std::string& setting_path) const
{
    return settings.at(setting_path).as<T>();
}

template <class T>
T RenderPipeline::Impl::get_setting(const std::string& setting_path, const T& fallback) const
{
    if (settings.find(setting_path) == settings.end())
        return fallback;
    else
        return settings.at(setting_path).as<T>(fallback);
}

// ************************************************************************************************

RenderPipeline* RenderPipeline::get_global_ptr(void)
{
    return global_ptr_;
}

RenderPipeline::RenderPipeline(int& argc, char**& argv): RPObject("RenderPipeline"), impl_(std::make_unique<Impl>(*this))
{
    if (!RPLogger::get_instance().is_created())
        RPLogger::get_instance().create("render_pipeline.log");

    global_ptr_ = this;

    debug("Constructing render pipeline ...");

    impl_->panda_framework_ = std::make_shared<PandaFramework>();
    impl_->panda_framework_->open_framework(argc, argv);

    impl_->analyze_system();

    impl_->mount_mgr_ = new MountManager;
    impl_->pre_showbase_initialized = false;
    set_loading_screen_image("/$$rp/data/gui/loading_screen_bg.txo");
}

RenderPipeline::RenderPipeline(PandaFramework* framework): RPObject("RenderPipeline"), impl_(std::make_unique<Impl>(*this))
{
    if (!RPLogger::get_instance().is_created())
        RPLogger::get_instance().create("render_pipeline.log");

    if (!framework)
    {
        error("PandaFramework is nullptr!");
        return;
    }

#if _MSC_VER >= 1900
    impl_->panda_framework_ = std::shared_ptr<PandaFramework>(framework, [](auto){});
#else
    impl_->panda_framework_ = std::shared_ptr<PandaFramework>(framework, [](PandaFramework*){});
#endif

    global_ptr_ = this;

    debug("Constructing render pipeline ...");

    impl_->analyze_system();

    impl_->mount_mgr_ = new MountManager;
    impl_->pre_showbase_initialized = false;
    set_loading_screen_image("/$$rp/data/gui/loading_screen_bg.txo");
}

RenderPipeline::~RenderPipeline(void) = default;

void RenderPipeline::run(void)
{
    if (impl_->showbase_)
    {
        impl_->showbase_->run();
    }
    else
    {
        error("ShowBase is not initialized! Call RenderPipeline::create() function, first!");
    }
}

bool RenderPipeline::load_settings(const std::string& path)
{
    impl_->settings = rplibs::load_yaml_file_flat(path);
    if (impl_->settings.empty())
        return false;

    // set log level
    const auto& level = get_setting<std::string>("pipeline.logging_level", "debug");
    if (level == "trace")
    {
        RPLogger::get_instance().get_internal_logger()->set_level(spdlog::level::trace);
    }
    else if (level == "debug")
    {
        RPLogger::get_instance().get_internal_logger()->set_level(spdlog::level::debug);
    }
    else if (level == "info")
    {
        RPLogger::get_instance().get_internal_logger()->set_level(spdlog::level::info);
    }
    else if (level == "warn")
    {
        RPLogger::get_instance().get_internal_logger()->set_level(spdlog::level::warn);
    }
    else if (level == "error")
    {
        RPLogger::get_instance().get_internal_logger()->set_level(spdlog::level::err);
    }
    else
    {
        error(fmt::format("Invalid logging level: {}. Fallback to debug level", level));
        RPLogger::get_instance().get_internal_logger()->set_level(spdlog::level::debug);
    }

    return true;
}

void RenderPipeline::reload_shaders(void)
{
    impl_->reload_shaders();
}

bool RenderPipeline::pre_showbase_init(void)
{
    if (!impl_->mount_mgr_->is_mounted())
    {
        debug("Mount manager was not mounted, mounting now ...");
        impl_->mount_mgr_->mount();
    }

    if (impl_->settings.empty())
    {
        debug("No settings loaded, loading from default location");
        if (!load_settings("/$$rpconfig/pipeline.yaml"))
            return false;
    }

    // C++ does not require checking install.flag.
    //if (rppanda::isfile("/$$rp/data/install.flag"))
    //    fatal("You didn't setup the pipeline yet! Please run setup.py.");

    load_prc_file("/$$rpconfig/panda3d-config.prc");
    impl_->pre_showbase_initialized = true;

    return true;
}

bool RenderPipeline::create(void)
{
    return impl_->create();
}

void RenderPipeline::set_loading_screen_image(const std::string& image_source)
{
    impl_->loading_screen = new LoadingScreen(this, image_source);
}

void RenderPipeline::add_light(RPLight* light)
{
    impl_->light_mgr_->add_light(light);
}

void RenderPipeline::remove_light(RPLight* light)
{
    impl_->light_mgr_->remove_light(light);
}

size_t RenderPipeline::load_ies_profile(const std::string& filename)
{
    return impl_->ies_loader_->load(filename);
}

void RenderPipeline::set_effect(NodePath& nodepath, const std::string& effect_src, const Effect::OptionType& options, int sort)
{
    decltype(Impl::applied_effects)::value_type args(NodePath(nodepath), effect_src, options, sort);
    impl_->applied_effects.push_back(args);
    impl_->internal_set_effect(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args));
}

void RenderPipeline::clear_effect(NodePath& nodepath)
{
    impl_->clear_effect(nodepath);
}

void RenderPipeline::prepare_scene(const NodePath& scene)
{
    std::vector<RPLight*> lights;

    NodePathCollection pl_npc = scene.find_all_matches("**/+PointLight");
    if (!scene.is_empty() && scene.node()->is_of_type(PointLight::get_class_type()))
        pl_npc.add_path(scene);
    for (int k=0, k_end=pl_npc.get_num_paths(); k < k_end; ++k)
    {
        NodePath light = pl_npc[k];

        PointLight* light_node = DCAST(PointLight, light.node());
        RPPointLight* rp_light = new RPPointLight;
        rp_light->set_pos(light.get_pos(Globals::base->get_render()));
        rp_light->set_radius(light_node->get_max_distance());
        rp_light->set_energy(20.0f * light_node->get_color().get_w());
        rp_light->set_color(light_node->get_color().get_xyz());
        rp_light->set_casts_shadows(light_node->is_shadow_caster());
        rp_light->set_shadow_map_resolution(light_node->get_shadow_buffer_size().get_x());
        rp_light->set_inner_radius(0.4);

        add_light(rp_light);
        light.remove_node();
        lights.push_back(rp_light);
    }

    NodePathCollection sp_npc = scene.find_all_matches("**/+Spotlight");
    if (!scene.is_empty() && scene.node()->is_of_type(Spotlight::get_class_type()))
        sp_npc.add_path(scene);
    for (int k=0, k_end=sp_npc.get_num_paths(); k < k_end; ++k)
    {
        NodePath light = sp_npc[k];

        Spotlight* light_node = DCAST(Spotlight, light.node());
        RPSpotLight* rp_light = new RPSpotLight;
        rp_light->set_pos(light.get_pos(Globals::base->get_render()));
        rp_light->set_radius(light_node->get_max_distance());
        rp_light->set_energy(20.0f * light_node->get_color().get_w());
        rp_light->set_color(light_node->get_color().get_xyz());
        rp_light->set_casts_shadows(light_node->is_shadow_caster());
        rp_light->set_shadow_map_resolution(light_node->get_shadow_buffer_size().get_x());
        rp_light->set_fov(light_node->get_exponent() / MathNumbers::pi * 180.0f);
        LVecBase3f lpoint = light.get_mat(Globals::base->get_render()).xform_vec(LVecBase3f(0, 0, -1));
        rp_light->set_direction(lpoint);

        add_light(rp_light);
        light.remove_node();
        lights.push_back(rp_light);
    }

    bool tristrips_warning_emitted = false;
    NodePathCollection gn_npc = scene.find_all_matches("**/+GeomNode");
    if (!scene.is_empty() && scene.node()->is_of_type(GeomNode::get_class_type()))
        gn_npc.add_path(scene);
    for (int k=0, k_end=gn_npc.get_num_paths(); k < k_end; ++k)
    {
        NodePath geom_np = gn_npc.get_path(k);
        GeomNode* geom_node = DCAST(GeomNode, geom_np.node());
        const int geom_count = geom_node->get_num_geoms();
        bool has_transparency = false;

        for (int i = 0; i < geom_count; ++i)
        {
            const RenderState* state = geom_node->get_geom_state(i);
            CPT(Geom) geom = geom_node->get_geom(i);

            bool needs_conversion = false;
            for (int prim_index=0, prim_end=geom->get_num_primitives(); prim_index < prim_end; ++prim_index)
            {
                CPT(GeomPrimitive) prim = geom->get_primitive(prim_index);
                if (prim->is_of_type(GeomTristrips::get_class_type()))
                {
                    needs_conversion = true;
                    if (!tristrips_warning_emitted)
                    {
                        warn(fmt::format("At least one GeomNode ({} and possible more..) contains tristrips.", geom_node->get_name()));
                        warn("Due to a NVIDIA Driver bug, we have to convert them to triangles now.");
                        warn("Consider exporting your models with the Bam Exporter to avoid this.");
                        tristrips_warning_emitted = true;
                        break;
                    }
                }
            }

            if (needs_conversion)
                geom_node->modify_geom(i)->decompose_in_place();

            if (!state->has_attrib(MaterialAttrib::get_class_type()))
            {
                warn(fmt::format("Geom {} has no material! Please fix this.", geom_node->get_name()));
                continue;
            }

            Material* material = DCAST(MaterialAttrib, state->get_attrib(MaterialAttrib::get_class_type()))->get_material();
            float shading_model = material->get_emission().get_x();
            if (!has_transparency && shading_model == 3)
                has_transparency = true;
        }

        // SHADING_MODEL_TRANSPARENT
        if (has_transparency)
        {
            if (geom_count > 1)
            {
                error(fmt::format("Transparent materials must be on their own geom!\n"
                    "If you are exporting from blender, split them into\n"
                    "seperate meshes, then re-export your scene. The\n"
                    "problematic mesh is: {}", geom_np.get_name()));
                continue;
            }
            set_effect(geom_np, "effects/default.yaml",
                {{"render_forward", true}, {"render_gbuffer", false}, {"render_shadow", false}}, 100);
        }
    }

    impl_->plugin_mgr_->on_prepare_scene(scene);
}

void RenderPipeline::export_materials(const std::string& pth)
{
    // TODO: implement
}

void RenderPipeline::compute_render_resolution(float resolution_scale)
{
    impl_->settings["pipeline.resolution_scale"].reset(YAML::Node(resolution_scale));

    auto last_resolution = Globals::resolution;
    impl_->compute_render_resolution();
    if (Globals::resolution != last_resolution)
        impl_->handle_window_resize();
}

void RenderPipeline::compute_render_resolution(int width, int height)
{
    impl_->settings["pipeline.resolution_width"].reset(YAML::Node(width));
    impl_->settings["pipeline.resolution_height"].reset(YAML::Node(height));

    auto last_resolution = Globals::resolution;
    impl_->compute_render_resolution();
    if (Globals::resolution != last_resolution)
        impl_->handle_window_resize();
}

const YAML::Node& RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->settings.at(setting_path);
}

template <>
bool RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->get_setting<bool>(setting_path);
}

template <>
bool RenderPipeline::get_setting(const std::string& setting_path, const bool& fallback) const
{
    return impl_->get_setting<bool>(setting_path, fallback);
}

template <>
int RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->get_setting<int>(setting_path);
}

template <>
int RenderPipeline::get_setting(const std::string& setting_path, const int& fallback) const
{
    return impl_->get_setting<int>(setting_path, fallback);
}

template <>
unsigned int RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->get_setting<unsigned int>(setting_path);
}

template <>
unsigned int RenderPipeline::get_setting(const std::string& setting_path, const unsigned int& fallback) const
{
    return impl_->get_setting<unsigned int>(setting_path, fallback);
}

template <>
size_t RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->get_setting<size_t>(setting_path);
}

template <>
size_t RenderPipeline::get_setting(const std::string& setting_path, const size_t& fallback) const
{
    return impl_->get_setting<size_t>(setting_path, fallback);
}

template <>
float RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->get_setting<float>(setting_path);
}

template <>
float RenderPipeline::get_setting(const std::string& setting_path, const float& fallback) const
{
    return impl_->get_setting<float>(setting_path, fallback);
}

template <>
double RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->get_setting<double>(setting_path);
}

template <>
double RenderPipeline::get_setting(const std::string& setting_path, const double& fallback) const
{
    return impl_->get_setting<double>(setting_path, fallback);
}

template <>
std::string RenderPipeline::get_setting(const std::string& setting_path) const
{
    return impl_->get_setting<std::string>(setting_path);
}

template <>
std::string RenderPipeline::get_setting(const std::string& setting_path, const std::string& fallback) const
{
    return impl_->get_setting<std::string>(setting_path, fallback);
}

PandaFramework* RenderPipeline::get_panda_framework(void) const
{
    return impl_->panda_framework_.get();
}

rppanda::ShowBase* RenderPipeline::get_showbase(void) const
{
    return impl_->showbase_;
}

MountManager* RenderPipeline::get_mount_mgr(void) const
{
    return impl_->mount_mgr_;
}

StageManager* RenderPipeline::get_stage_mgr(void) const
{
    return impl_->stage_mgr_;
}

TagStateManager* RenderPipeline::get_tag_mgr(void) const
{
    return impl_->tag_mgr_;
}

LightManager* RenderPipeline::get_light_mgr(void) const
{
    return impl_->light_mgr_;
}

PluginManager* RenderPipeline::get_plugin_mgr(void) const
{
    return impl_->plugin_mgr_;
}

TaskScheduler* RenderPipeline::get_task_scheduler(void) const
{
    return impl_->task_scheduler_;
}

DayTimeManager* RenderPipeline::get_daytime_mgr(void) const
{
    return impl_->daytime_mgr_;
}

}
